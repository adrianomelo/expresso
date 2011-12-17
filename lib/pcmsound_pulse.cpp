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

#include <QFile>
#include <QCoreApplication>
#include <pulse/pulseaudio.h>

#include "pcmsound_pulse.h"


class PulseClient : public QObject
{
    Q_OBJECT

public:
    PulseClient();
    ~PulseClient();

    bool isReady() const { return m_ready; }

    void lock() { pa_threaded_mainloop_lock(m_loop); }
    void unlock() { pa_threaded_mainloop_unlock(m_loop); }

    pa_context *context() const { return m_context; }
    pa_mainloop_api *mainloopApi() const { return m_mainloopApi; }
    const pa_sample_spec *sampleSpec() const { return &m_sampleSpec; }

signals:
    void ready();

private:
    static void context_state_callback(pa_context *context, void *userData);

    bool m_ready;
    pa_context *m_context;
    pa_sample_spec m_sampleSpec;
    pa_threaded_mainloop *m_loop;
    pa_mainloop_api *m_mainloopApi;
};

Q_GLOBAL_STATIC(PulseClient, pulse_instance);


class PulseLocker
{
public:
    PulseLocker() { pulse_instance()->lock(); }
    ~PulseLocker() { pulse_instance()->unlock(); }
};


PulseClient::PulseClient()
    : QObject(),
      m_ready(false),
      m_context(0),
      m_loop(0),
      m_mainloopApi(0)
{
    m_sampleSpec.channels = 1;
    m_sampleSpec.rate = 16000;
    m_sampleSpec.format = pa_parse_sample_format("s16le");

    if (!pa_sample_spec_valid(&m_sampleSpec)) {
        qWarning("PulsePcmSound: Invalid sample spec");
        qApp->quit();
        return;
    }

    if (!(m_loop = pa_threaded_mainloop_new())) {
        qWarning("PulsePcmSound: Unable to create threaded mainloop");
        qApp->quit();
        return;
    }

    m_mainloopApi = pa_threaded_mainloop_get_api(m_loop);

    pa_signal_init(m_mainloopApi);

    const char *clientName = QString("PulseClient:%1").arg(::getpid()).toAscii().data();

    if (!(m_context = pa_context_new(m_mainloopApi, clientName))) {
        qWarning("PulsePcmSound: Unable to create context");
        qApp->quit();
        return;
    }

    pa_context_set_state_callback(m_context, context_state_callback, this);

    // connect context
    if (pa_context_connect(m_context, 0, (pa_context_flags_t)0, 0) < 0) {
        qWarning("PulsePcmSound: Unable to connect to context (%s)",
                 pa_strerror(pa_context_errno(m_context)));
        qApp->quit();
        return;
    }

    if (pa_threaded_mainloop_start(m_loop) != 0)
        qWarning("PulsePcmSound: Unable to start threaded mainloop");
}

PulseClient::~PulseClient()
{
    if (m_context) {
        lock();
        pa_context_disconnect(m_context);
        m_context = 0;
        unlock();
    }

    if (m_loop) {
        pa_threaded_mainloop_stop(m_loop);
        pa_threaded_mainloop_free(m_loop);
        m_loop = 0;
    }
}

void PulseClient::context_state_callback(pa_context *c, void *userData)
{
    PulseClient *pulse = reinterpret_cast<PulseClient *>(userData);

    switch (pa_context_get_state(c)) {
    case PA_CONTEXT_CONNECTING:
    case PA_CONTEXT_AUTHORIZING:
    case PA_CONTEXT_SETTING_NAME:
        break;

    case PA_CONTEXT_READY:
        pulse->m_ready = true;
        emit pulse->ready();
        break;

    case PA_CONTEXT_TERMINATED:
        qApp->quit();
        break;

    case PA_CONTEXT_FAILED:
    default:
        qWarning("PulseClientAudio: Connection failure (%s)", pa_strerror(pa_context_errno(c)));
        qApp->quit();
        break;
    }
}


PulsePcmSound::PulsePcmSound(const WavFile &file, QObject *parent)
    : AbstractPcmSound(file, parent),
      m_ready(false),
      m_position(0),
      m_playCount(0),
      m_loopCount(1),
      m_streamIndex(-1),
      m_muted(false),
      m_volume(1),
      m_data(file.data()),
      m_stream(0)
{
    PulseClient *pulse = pulse_instance();

    if (pulse->isReady())
        onContextReady();
    else
        connect(pulse, SIGNAL(ready()), this, SLOT(onContextReady()));
}

PulsePcmSound::~PulsePcmSound()
{
    PulseLocker locker;

    if (m_stream) {
        pa_stream_set_state_callback(m_stream, 0, 0);
        pa_stream_set_write_callback(m_stream, 0, 0);
        pa_stream_set_suspended_callback(m_stream, 0, 0);
        pa_stream_set_underflow_callback(m_stream, 0, 0);
        pa_stream_set_overflow_callback(m_stream,  0, 0);

        pa_stream_disconnect(m_stream);
        pa_stream_unref(m_stream);
        m_stream = 0;
    }
}

void PulsePcmSound::play()
{
    if (m_loopCount < 2)
        m_position = 0;
    m_playCount = m_loopCount;
    uploadSample();
}

void PulsePcmSound::stop()
{
    m_position = 0;
    m_playCount = 0;
}

int PulsePcmSound::loopCount() const
{
    return m_loopCount;
}

void PulsePcmSound::setLoopCount(int loopCount)
{
    m_loopCount = loopCount;
}

void PulsePcmSound::setVolume(qreal volume)
{
    m_volume = volume;
    updateVolume();
}

void PulsePcmSound::setMuted(bool muted)
{
    m_muted = muted;
    updateVolume();
}

void PulsePcmSound::updateVolume()
{
    if (m_streamIndex < 0)
        return;

    PulseClient *pulse = pulse_instance();

    PulseLocker locker;

    pa_cvolume volume;
    volume.channels = pulse->sampleSpec()->channels;

    for (int i = 0; i < volume.channels; ++i)
        volume.values[i] = m_muted ? PA_VOLUME_MUTED : PA_VOLUME_NORM * m_volume;

    pa_operation_unref(pa_context_set_sink_input_volume(pulse->context(), m_streamIndex, &volume, 0, 0));
}

void PulsePcmSound::onContextReady()
{
    disconnect(pulse_instance(), SIGNAL(ready()), this, SLOT(onContextReady()));

    createStream();
    uploadSample();
}

void PulsePcmSound::createStream()
{
    PulseClient *pulse = pulse_instance();
    pa_context *context = pulse->context();

    PulseLocker locker;
    const char *streamName = QString("PulsePcmSound:%1").arg(::getpid()).toAscii().data();

    if (!(m_stream = pa_stream_new(context, streamName, pulse->sampleSpec(), 0))) {
        qWarning("PulsePcmSound: Unable to create stream (%s)", pa_strerror(pa_context_errno(context)));
        qApp->quit();
        return;
    }

    pa_stream_set_state_callback(m_stream, stream_state_callback, this);
    pa_stream_set_write_callback(m_stream, stream_write_callback, this);
    pa_stream_set_suspended_callback(m_stream, stream_suspended_callback, this);
    pa_stream_set_underflow_callback(m_stream, stream_underflow_callback, this);
    pa_stream_set_overflow_callback(m_stream, stream_overflow_callback, this);

    if (pa_stream_connect_playback(m_stream, 0, 0, (pa_stream_flags_t)0, 0, 0) < 0) {
        qWarning("PulsePcmSound: Unable to connect playback (%s)", pa_strerror(pa_context_errno(context)));
        qApp->quit();
        return;
    }

    updateVolume();
}

void PulsePcmSound::uploadSample()
{
    if (!m_ready || m_playCount == 0)
        return;

    if (m_position == m_data.size()) {
        m_position = 0;
        if (--m_playCount == 0) {
            QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
            return;
        }
    }

    pa_context *context = pulse_instance()->context();

    int writableSize = pa_stream_writable_size(m_stream);
    int bufferLength = qMin<int>(writableSize, m_data.size() - m_position);

    if (pa_stream_write(m_stream, reinterpret_cast<uint8_t *>(m_data.data()) + m_position,
                        bufferLength, 0, 0, PA_SEEK_RELATIVE) < 0) {
        qWarning("PulsePcmSound: Unable to write to stream (%s)", pa_strerror(pa_context_errno(context)));
        return;
    }

    m_position += bufferLength;
}

void PulsePcmSound::stream_write_callback(pa_stream *stream, size_t length, void *userData)
{
    Q_UNUSED(stream);
    Q_UNUSED(length);

    PulsePcmSound *sound = reinterpret_cast<PulsePcmSound *>(userData);
    sound->uploadSample();
}

void PulsePcmSound::stream_state_callback(pa_stream *stream, void *userData)
{
    PulsePcmSound *sound = reinterpret_cast<PulsePcmSound *>(userData);

    switch (pa_stream_get_state(stream)) {
    case PA_STREAM_CREATING:
    case PA_STREAM_TERMINATED:
        break;

    case PA_STREAM_READY:
        sound->m_ready = true;
        sound->m_streamIndex = pa_stream_get_index(stream);
        sound->uploadSample();
        break;

    case PA_STREAM_FAILED:
    default:
        qWarning("PulsePcmSound: Stream error (%s)",
                 pa_strerror(pa_context_errno(pa_stream_get_context(stream))));
        qApp->quit();
        break;
    }
}

void PulsePcmSound::stream_suspended_callback(pa_stream *stream, void *userData)
{
    Q_UNUSED(stream);
    Q_UNUSED(userData);
}

void PulsePcmSound::stream_underflow_callback(pa_stream *stream, void *userData)
{
    Q_UNUSED(stream);
    Q_UNUSED(userData);
}

void PulsePcmSound::stream_overflow_callback(pa_stream *stream, void *userData)
{
    Q_UNUSED(stream);
    Q_UNUSED(userData);
}

#include "wavsound_pulse.moc"
