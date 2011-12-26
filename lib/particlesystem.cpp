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

#include "particlesystem.h"
#include "common.h"
#include <QPainter>


struct Particle
{
    Particle(ParticleSystem *system);

    int id;
    QPoint pos;
    int rotation;
    int lifeCount;
    ParticleSystem *system;

    void advance();
};


Particle::Particle(ParticleSystem *system)
    : rotation(0),
      lifeCount(0),
      system(system)
{

}

void Particle::advance()
{
    lifeCount++;
    rotation += qrand() % 8;
    pos.rx() += system->xVelocity();
    pos.ry() += system->yVelocity();
}


ParticleSystem::ParticleSystem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
      m_rate(1),
      m_automatic(false),
      m_lifeSpan(100),
      m_rateCount(0),
      m_xVelocity(0),
      m_yVelocity(5)
{
    connect(&m_timer, SIGNAL(timeout()), SLOT(tick()));
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

ParticleSystem::~ParticleSystem()
{
    qDeleteAll(m_particles);
}

void ParticleSystem::setRate(int value)
{
    if (m_rate != value) {
        m_rate = value;
        m_rateCount = 0;
        emit rateChanged();
    }
}

void ParticleSystem::setLifeSpan(int value)
{
    if (m_lifeSpan != value) {
        m_lifeSpan = value;
        emit lifeSpanChanged();
    }
}

void ParticleSystem::setOriginRect(const QRect &rect)
{
    if (m_rect != rect) {
        m_rect = rect;
        emit originRectChanged();
    }
}

void ParticleSystem::setXVelocity(qreal value)
{
    if (m_xVelocity != value) {
        m_xVelocity = value;
        emit xVelocityChanged();
    }
}

void ParticleSystem::setYVelocity(qreal value)
{
    if (m_yVelocity != value) {
        m_yVelocity = value;
        emit yVelocityChanged();
    }
}

void ParticleSystem::setAutomatic(bool enabled)
{
    if (m_automatic != enabled) {
        m_automatic = enabled;

        if (enabled)
            m_timer.start(16);
        else
            m_timer.stop();

        emit automaticChanged();
    }
}

void ParticleSystem::setSources(const QVariantList &sources)
{
    if (m_sources == sources)
        return;

    m_pixmaps.clear();
    m_sources = sources;

    foreach (const QVariant &v, sources) {
        const QUrl &url = v.toUrl();

        if (url.isValid())
            m_pixmaps << loadPixmap(this, url);
    }

    emit sourcesChanged();
}

void ParticleSystem::advance(int phase)
{
    if (phase == 0 || m_automatic)
        return;

    tick();
}

void ParticleSystem::tick()
{
    QList<Particle *> deadParticles;

    foreach (Particle *particle, m_particles) {
        particle->advance();

        if (particle->lifeCount > m_lifeSpan)
            deadParticles.append(particle);
    }

    foreach (Particle *particle, deadParticles) {
        m_particles.removeOne(particle);
        delete particle;
    }

    m_rateCount++;

    if (m_rateCount > m_rate) {
        m_rateCount = 0;

        Particle *particle = createParticle();

        if (!particle)
            qWarning("ParticleSystem: Unable to create particle");
        else
            m_particles.append(particle);
    }

    update();
}

void ParticleSystem::componentComplete()
{
    QDeclarativeItem::componentComplete();
}

Particle *ParticleSystem::createParticle()
{
    QRect r = m_rect;

    if (r.isNull())
        r = boundingRect().toRect();

    const int w = r.width();
    const int h = r.height();

    Particle *particle = new Particle(this);
    particle->id = qrand() % m_pixmaps.count();
    particle->rotation = qrand() % 300;
    particle->pos = QPoint(w > 0 ? r.x() + qrand() % w : 0,
                           h > 0 ? r.y() + qrand() % h : 0);

    return particle;
}

void ParticleSystem::paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget)
{
    Q_UNUSED(options);
    Q_UNUSED(widget);

    if (m_pixmaps.isEmpty() || m_particles.isEmpty())
        return;

    painter->save();
    painter->setClipRect(boundingRect());

    foreach (Particle *particle, m_particles) {
        if (particle->id >= m_pixmaps.count())
            continue;

        painter->translate(particle->pos);
        painter->rotate(particle->rotation);

        painter->drawPixmap(0, 0, m_pixmaps.at(particle->id));

        painter->rotate(-particle->rotation);
        painter->translate(-particle->pos);
    }

    painter->restore();
}
