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

#ifndef SPRITE_H
#define SPRITE_H

#include <QTimer>
#include <QDeclarativeItem>


class SpriteState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int frameCount READ frameCount WRITE setFrameCount NOTIFY frameCountChanged)
    Q_PROPERTY(int frameWidth READ frameWidth WRITE setFrameWidth NOTIFY frameWidthChanged)
    Q_PROPERTY(int frameHeight READ frameHeight WRITE setFrameHeight NOTIFY frameHeightChanged)
    Q_PROPERTY(qreal frameRate READ frameRate WRITE setFrameRate NOTIFY frameRateChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int loopCount READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)
    Q_PROPERTY(SpriteState *nextState READ nextState WRITE setNextState NOTIFY nextStateChanged)
    Q_PROPERTY(QVariantList sources READ sources WRITE setSources NOTIFY sourcesChanged)

public:
    SpriteState(QObject *parent = 0);

    int frameCount() const;
    void setFrameCount(int count);

    int frameWidth() const;
    void setFrameWidth(int width);

    int frameHeight() const;
    void setFrameHeight(int height);

    qreal frameRate() const;
    void setFrameRate(qreal rate);

    QUrl source() const;
    void setSource(const QUrl &source);

    QVariantList sources() const { return m_sources; }
    void setSources(const QVariantList &sources);

    int loopCount() const;
    void setLoopCount(int count);

    SpriteState *nextState() const;
    void setNextState(SpriteState *state);

signals:
    void activated();
    void deactivated();
    void sourceChanged();
    void sourcesChanged();
    void loopCountChanged();
    void nextStateChanged();
    void frameRateChanged();
    void frameCountChanged();
    void frameWidthChanged();
    void frameHeightChanged();

protected:
    bool advance();
    QRect sourceRect() const;
    bool isTiled() const;
    QPixmap pixmap() const;

private:
    QUrl m_source;
    int m_frameCount;
    int m_frameWidth;
    int m_frameHeight;
    qreal m_frameRate;
    SpriteState *m_nextState;
    QPixmap m_pixmap;
    qreal m_frame;
    int m_loopCount;
    int m_currentLoop;
    QVariantList m_sources;
    QList<QPixmap> m_pixmaps;

    friend class Sprite;
};


class Sprite : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(SpriteState *spriteState READ spriteState WRITE setSpriteState NOTIFY spriteStateChanged)
    Q_PROPERTY(QDeclarativeListProperty<SpriteState> spriteStates READ spriteStates)
    Q_PROPERTY(bool flipHorizontally READ flipHorizontally WRITE setFlipHorizontally NOTIFY flipHorizontallyChanged)

public:
    Sprite(QDeclarativeItem *parent = 0);

    QDeclarativeListProperty<SpriteState> spriteStates();

    void componentComplete();

    bool flipHorizontally() const;
    void setFlipHorizontally(bool flip);

    SpriteState *spriteState() const;
    void setSpriteState(SpriteState *state);

    void advance(int phase);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget = 0);

signals:
    void advanced();
    void spriteStateChanged();
    void flipHorizontallyChanged();

private:
    bool m_flip;
    SpriteState *m_state;
    QList<SpriteState *> m_states;
};


class SpriteScene : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(bool automaticStep READ automaticStep WRITE setAutomaticStep NOTIFY automaticStepChanged)

public:
    SpriteScene(QDeclarativeItem *parent = 0);

    bool automaticStep() const;
    void setAutomaticStep(bool enabled);

    void advance(int phase);

    void componentComplete();

public slots:
    void tick();

signals:
    void advanced();
    void automaticStepChanged();

private:
    QTimer m_timer;
    bool m_automaticStep;
};

#endif
