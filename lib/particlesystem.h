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

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <QUrl>
#include <QTimer>
#include <QVariantList>
#include <QDeclarativeItem>


struct Particle;

class ParticleSystem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(int rate READ rate WRITE setRate NOTIFY rateChanged)
    Q_PROPERTY(int lifeSpan READ lifeSpan WRITE setLifeSpan NOTIFY lifeSpanChanged)
    Q_PROPERTY(QVariantList sources READ sources WRITE setSources NOTIFY sourcesChanged)
    Q_PROPERTY(qreal xVelocity READ xVelocity WRITE setXVelocity NOTIFY xVelocityChanged)
    Q_PROPERTY(qreal yVelocity READ yVelocity WRITE setYVelocity NOTIFY yVelocityChanged)
    Q_PROPERTY(QRect originRect READ originRect WRITE setOriginRect NOTIFY originRectChanged)
    Q_PROPERTY(bool automatic READ isAutomatic WRITE setAutomatic NOTIFY automaticChanged)

public:
    ParticleSystem(QDeclarativeItem *parent = 0);
    ~ParticleSystem();

    int rate() const { return m_rate; }
    void setRate(int value);

    int lifeSpan() const { return m_lifeSpan; }
    void setLifeSpan(int value);

    QRect originRect() const { return m_rect; }
    void setOriginRect(const QRect &rect);

    qreal xVelocity() const { return m_xVelocity; }
    void setXVelocity(qreal value);

    qreal yVelocity() const { return m_yVelocity; }
    void setYVelocity(qreal value);

    bool isAutomatic() const { return m_automatic; }
    void setAutomatic(bool enabled);

    QVariantList sources() const { return m_sources; }
    void setSources(const QVariantList &sources);

    void advance(int phase);
    void componentComplete();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget = 0);

signals:
    void rateChanged();
    void sourcesChanged();
    void lifeSpanChanged();
    void originRectChanged();
    void xVelocityChanged();
    void yVelocityChanged();
    void automaticChanged();

protected slots:
    void tick();

protected:
    Particle *createParticle();

private:
    int m_rate;
    bool m_automatic;
    int m_lifeSpan;
    int m_rateCount;
    QList<Particle *> m_particles;
    QRect m_rect;
    QVariantList m_sources;
    QList<QPixmap> m_pixmaps;
    qreal m_xVelocity;
    qreal m_yVelocity;
    QTimer m_timer;
};

#endif
