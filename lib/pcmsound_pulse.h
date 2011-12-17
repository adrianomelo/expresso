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
#include <QByteArray>

struct pa_stream;

class PulsePcmSound : public AbstractPcmSound
{
    Q_OBJECT

public:
    PulsePcmSound(const WavFile &file, QObject *parent = 0);
    ~PulsePcmSound();

    int loopCount() const;
    void setLoopCount(int loopCount);

    void setMuted(bool muted);
    void setVolume(qreal value);

signals:
    void finished();

public slots:
    void play();
    void stop();

protected slots:
    void onContextReady();

protected:
    void createStream();
    void uploadSample();
    void updateVolume();

    static void stream_state_callback(pa_stream *stream, void *userData);
    static void stream_write_callback(pa_stream *stream, size_t length, void *userData);
    static void stream_suspended_callback(pa_stream *stream, void *userData);
    static void stream_underflow_callback(pa_stream *stream, void *userData);
    static void stream_overflow_callback(pa_stream *stream, void *userData);

private:
    bool m_ready;
    int m_position;
    int m_playCount;
    int m_loopCount;
    int m_streamIndex;
    bool m_muted;
    qreal m_volume;
    QByteArray m_data;
    pa_stream *m_stream;
};

#endif
