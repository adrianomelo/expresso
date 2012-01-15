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

#ifndef PCMSOUND_PULSE_H
#define PCMSOUND_PULSE_H

#include "pcmsound.h"

#include <QObject>
#include <pulse/pulseaudio.h>


class PulsePcmSound : public AbstractPcmSound
{
    Q_OBJECT

public:
    PulsePcmSound(QObject *parent = 0);
    ~PulsePcmSound();

    QUrl source() const { return m_source; }
    void setSource(const QUrl &url);

    bool isPlaying() const { return m_isPlaying; }
    void setPlaying(bool playing);

    int loopCount() const { return m_loopCount; }
    void setLoopCount(int loopCount);

    bool isPaused() const { return m_isPaused; }
    void setPaused(bool paused);

    bool isMuted() const { return m_isMuted; }
    void setMuted(bool muted);

    qreal volume() const { return m_volume; }
    void setVolume(qreal value);

protected slots:
    void createStream();
    void deleteStream();
    void updateVolume();

protected:
    void uploadSample();

    static pa_sample_format sampleFormat(WavFile::Format format);

    static void stream_state_callback(pa_stream *stream, void *userData);
    static void stream_write_callback(pa_stream *stream, size_t length, void *userData);
    static void stream_suspended_callback(pa_stream *stream, void *userData);
    static void stream_underflow_callback(pa_stream *stream, void *userData);
    static void stream_overflow_callback(pa_stream *stream, void *userData);

private:
    bool m_isReady;
    int m_position;
    int m_playCount;
    int m_loopCount;
    int m_streamIndex;
    bool m_isMuted;
    bool m_isPaused;
    qreal m_volume;
    bool m_isPlaying;
    QUrl m_source;
    QByteArray m_data;
    pa_stream *m_stream;
    pa_sample_spec m_sampleSpec;
};

typedef PulsePcmSound PcmSound;

#endif
