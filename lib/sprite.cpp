/****************************************************************************
**
** Copyright (C) 2011 Nokia Institute of Technology.
** All rights reserved.
** Contact: Manager (renato.chencarek@openbossa.org)
**
** This file is part of the Expresso project.
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "sprite.h"
#include "common.h"
#include <QPainter>
#include <QGraphicsScene>
#include <qmath.h>


const int q_spriteSceneTick = 30;

SpriteScene::SpriteScene(QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
      m_automaticStep(true)
{
    connect(&m_timer, SIGNAL(timeout()), SLOT(tick()));
}

bool SpriteScene::automaticStep() const
{
    return m_automaticStep;
}

void SpriteScene::setAutomaticStep(bool enabled)
{
    if (m_automaticStep == enabled)
        return;

    m_automaticStep = enabled;
    emit automaticStepChanged();

    if (isComponentComplete()) {
        if (enabled)
            m_timer.start(q_spriteSceneTick);
        else
            m_timer.stop();
    }
}

void SpriteScene::tick()
{
    if (!scene())
        return;

    scene()->advance();
}

void SpriteScene::componentComplete()
{
    QDeclarativeItem::componentComplete();

    if (m_automaticStep)
        m_timer.start(q_spriteSceneTick);
}

void SpriteScene::advance(int phase)
{
    if (phase != 0)
        emit advanced();
}


Sprite::Sprite(QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
      m_flip(false),
      m_state(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

bool Sprite::flipHorizontally() const
{
    return m_flip;
}

void Sprite::setFlipHorizontally(bool flip)
{
    if (m_flip != flip) {
        m_flip = flip;
        emit flipHorizontallyChanged();
        update();
    }
}

QDeclarativeListProperty<SpriteState> Sprite::spriteStates()
{
    return QDeclarativeListProperty<SpriteState>(this, m_states);
}

SpriteState *Sprite::spriteState() const
{
    return m_state;
}

void Sprite::setSpriteState(SpriteState *state)
{
    if (state == m_state)
        return;

    if (m_state)
        emit m_state->deactivated();

    m_state = state;

    if (m_state)
        emit m_state->activated();

    emit spriteStateChanged();
    update();
}

void Sprite::componentComplete()
{
    QDeclarativeItem::componentComplete();

    if (!m_state && !m_states.isEmpty())
        setSpriteState(m_states.at(0));
}

void Sprite::advance(int phase)
{
    if (phase == 0)
        return;

    if (m_state) {
        if (m_state->advance()) {
            update();
        } else if (m_state->nextState()) {
            setSpriteState(m_state->nextState());
            update();
        }
    }

    emit advanced();
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget)
{
    Q_UNUSED(options);
    Q_UNUSED(widget);

    if (!m_state)
        return;

    const QPixmap &pixmap = m_state->pixmap();

    if (pixmap.isNull())
        return;

    if (!m_state->isTiled()) {
        // paint frame
        painter->save();
        if (m_flip) {
            painter->scale(-1, 1);
            painter->translate(-width(), 0);
        }
        painter->drawPixmap(0, 0, pixmap);
        painter->restore();
        return;
    }

    QRect dest = m_state->sourceRect();

    if (dest.isNull())
        return;

    // adjust target geometry
    if (dest.width() > width())
        dest.setWidth(width());

    if (dest.height() > height())
        dest.setHeight(height());

    // adjust position
    dest.moveTo(width() / 2 - dest.width() / 2,
                height() / 2 - dest.height() / 2);

    // paint frame
    painter->save();
    if (m_flip) {
        painter->scale(-1, 1);
        painter->translate(-width(), 0);
    }
    painter->drawPixmap(dest, m_state->pixmap(), m_state->sourceRect());
    painter->restore();
}


SpriteState::SpriteState(QObject *parent)
    : QObject(parent),
      m_frameCount(1),
      m_frameWidth(0),
      m_frameHeight(0),
      m_frameRate(1),
      m_nextState(0),
      m_frame(0),
      m_loopCount(-1),
      m_currentLoop(0)
{

}

int SpriteState::frameCount() const
{
    return m_frameCount;
}

void SpriteState::setFrameCount(int count)
{
    if (m_frameCount != count) {
        m_frameCount = count;
        emit frameCountChanged();
    }
}

int SpriteState::frameWidth() const
{
    return m_frameWidth;
}

void SpriteState::setFrameWidth(int width)
{
    if (m_frameWidth != width) {
        m_frameWidth = width;
        emit frameWidthChanged();
    }
}

int SpriteState::frameHeight() const
{
    return m_frameHeight;
}

void SpriteState::setFrameHeight(int height)
{
    if (m_frameHeight != height) {
        m_frameHeight = height;
        emit frameHeightChanged();
    }
}

qreal SpriteState::frameRate() const
{
    return m_frameRate;
}

void SpriteState::setFrameRate(qreal rate)
{
    if (m_frameRate != rate) {
        m_frameRate = rate;
        emit frameRateChanged();
    }
}

QUrl SpriteState::source() const
{
    return m_source;
}

void SpriteState::setSource(const QUrl &source)
{
    if (m_source != source) {
        m_source = source;
        m_pixmaps.clear();
        m_pixmap = loadPixmap(this, source);
        emit sourceChanged();
    }
}

void SpriteState::setSources(const QVariantList &sources)
{
    if (m_sources == sources)
        return;

    m_pixmap = QPixmap();
    m_pixmaps.clear();
    m_sources = sources;

    foreach (const QVariant &v, sources) {
        const QUrl &url = v.toUrl();

        if (url.isValid())
            m_pixmaps << loadPixmap(this, url);
    }

    emit sourcesChanged();
}

SpriteState *SpriteState::nextState() const
{
    return m_nextState;
}

void SpriteState::setNextState(SpriteState *state)
{
    if (m_nextState != state) {
        m_nextState = state;
        emit nextStateChanged();
    }
}

int SpriteState::loopCount() const
{
    return m_loopCount;
}

void SpriteState::setLoopCount(int count)
{
    if (m_loopCount != count) {
        m_loopCount = count;
        emit loopCountChanged();
    }
}

QRect SpriteState::sourceRect() const
{
    if (m_frameWidth <= 0)
        return QRect();

    const int cols = m_pixmap.width() / m_frameWidth;

    if (cols <= 0)
        return QRect();

    const int idx = qFloor(m_frame);
    const int j = idx / cols;
    const int i = idx % cols;

    return QRect(i * m_frameWidth, j * m_frameHeight, m_frameWidth, m_frameHeight);
}

bool SpriteState::advance()
{
    const qreal frame = m_frame + m_frameRate;

    if (frame < m_frameCount) {
        m_frame = frame;
        return true;
    }

    if (m_loopCount >= 0 && m_currentLoop >= m_loopCount)
        return false;

    m_currentLoop++;
    m_frame = frame - qFloor(frame);

    if (m_loopCount >= 0 && m_currentLoop >= m_loopCount) {
        m_frame = 0;
        m_currentLoop = 0;
        return false;
    }

    return true;
}

bool SpriteState::isTiled() const
{
    return m_pixmaps.empty();
}

QPixmap SpriteState::pixmap() const
{
    if (isTiled())
        return m_pixmap;
    else {
        const int idx = qFloor(m_frame);

        if (idx < 0 || idx >= m_pixmaps.count())
            return QPixmap();
        else
            return m_pixmaps.at(idx);
    }
}
