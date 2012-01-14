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

#ifndef PCMSOUND_DUMMY_H
#define PCMSOUND_DUMMY_H

#include "pcmsound.h"


class DummyPcmSound : public AbstractPcmSound
{
    Q_OBJECT

public:
    inline DummyPcmSound(QObject *parent = 0);

    QUrl source() const { return m_source; }
    inline void setSource(const QUrl &url);

    bool isPlaying() const { return false; }
    void setPlaying(bool playing) {}

    int loopCount() const { return m_loopCount; }
    inline void setLoopCount(int loopCount);

    bool isPaused() const { return m_isPaused; }
    inline void setPaused(bool paused);

    bool isMuted() const { return m_isMuted; }
    inline void setMuted(bool muted);

    qreal volume() const { return m_volume; }
    inline void setVolume(qreal volume);

private:
    QUrl m_source;
    int m_loopCount;
    bool m_isMuted;
    bool m_isPaused;
    qreal m_volume;
};

typedef DummyPcmSound PcmSound;


DummyPcmSound::DummyPcmSound(QObject *parent)
    : AbstractPcmSound(parent),
      m_loopCount(1),
      m_isMuted(false),
      m_isPaused(false),
      m_volume(1.0)
{
    qWarning("PcmSound: No sound engine");
}

void DummyPcmSound::setSource(const QUrl &url)
{
    if (m_source != url) {
        m_source = url;
        emit sourceChanged();
    }
}

void DummyPcmSound::setLoopCount(int count)
{
    if (m_loopCount != count) {
        m_loopCount = count;
        emit loopsChanged();
    }
}

void DummyPcmSound::setPaused(bool paused)
{
    if (m_isPaused != paused) {
        m_isPaused = paused;
        emit pausedChanged();
    }
}

void DummyPcmSound::setMuted(bool muted)
{
    if (m_isMuted != muted) {
        m_isMuted = muted;
        emit mutedChanged();
    }
}

void DummyPcmSound::setVolume(qreal volume)
{
    volume = qBound<qreal>(0.0, volume, 1.0);

    if (m_volume != volume) {
        m_volume = volume;
        emit volumeChanged();
    }
}

#endif
