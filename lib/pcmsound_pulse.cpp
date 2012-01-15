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

#include "pcmsound_pulse.h"


class PulseClient : public QObject
{
    Q_OBJECT

public:
    PulseClient();
    ~PulseClient();

    bool isReady() const { return m_isReady; }

    void lock() { pa_threaded_mainloop_lock(m_loop); }
    void unlock() { pa_threaded_mainloop_unlock(m_loop); }

    pa_context *context() const { return m_context; }

signals:
    void ready();

private:
    static void context_state_callback(pa_context *context, void *userData);

    bool m_isReady;
    pa_context *m_context;
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
      m_isReady(false),
      m_context(0),
      m_loop(0),
      m_mainloopApi(0)
{
    m_loop = pa_threaded_mainloop_new();

    if (!m_loop) {
        qWarning("PcmSound(pulse): Unable to create threaded mainloop");
        qApp->quit();
        return;
    }

    m_mainloopApi = pa_threaded_mainloop_get_api(m_loop);

    pa_signal_init(m_mainloopApi);

    const char *clientName = QString("PulseClient:%1").arg(::getpid()).toAscii().data();

    m_context = pa_context_new(m_mainloopApi, clientName);

    if (!m_context) {
        qWarning("PcmSound(pulse): Unable to create context");
        qApp->quit();
        return;
    }

    pa_context_set_state_callback(m_context, context_state_callback, this);

    // connect context
    if (pa_context_connect(m_context, 0, (pa_context_flags_t)0, 0) < 0) {
        qWarning("PcmSound(pulse): Unable to connect to context (%s)",
                 pa_strerror(pa_context_errno(m_context)));
        qApp->quit();
        return;
    }

    if (pa_threaded_mainloop_start(m_loop) != 0)
        qWarning("PcmSound(pulse): Unable to start threaded mainloop");
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
        pulse->m_isReady = true;
        emit pulse->ready();
        break;

    case PA_CONTEXT_TERMINATED:
        qApp->quit();
        break;

    case PA_CONTEXT_FAILED:
    default:
        qWarning("PcmSound(pulse): Connection failure (%s)", pa_strerror(pa_context_errno(c)));
        qApp->quit();
        break;
    }
}


PulsePcmSound::PulsePcmSound(QObject *parent)
    : AbstractPcmSound(parent),
      m_isReady(false),
      m_position(0),
      m_playCount(0),
      m_loopCount(1),
      m_streamIndex(-1),
      m_isMuted(false),
      m_isPaused(false),
      m_volume(1),
      m_isPlaying(false),
      m_stream(0)
{
    connect(pulse_instance(), SIGNAL(ready()), this, SLOT(createStream()));
}

PulsePcmSound::~PulsePcmSound()
{
    deleteStream();
}

void PulsePcmSound::setSource(const QUrl &url)
{
    if (m_source == url)
        return;

    m_source = url;
    emit sourceChanged();

    deleteStream();

    WavFile file;

    if (!file.load(url.toLocalFile())) {
        m_data = QByteArray();
        qWarning("PcmSound(pulse): Unable to load file '%s'",
                 url.toLocalFile().toLatin1().data());
        return;
    }

    m_data = file.data();
    m_sampleSpec.channels = file.channels();
    m_sampleSpec.rate = file.sampleRate();
    m_sampleSpec.format = sampleFormat(file.format());

    if (pulse_instance()->isReady())
        createStream();
}

pa_sample_format PulsePcmSound::sampleFormat(WavFile::Format format)
{
    switch (format) {
    case WavFile::U8:
        return PA_SAMPLE_U8;
    case WavFile::S16LE:
        return PA_SAMPLE_S16LE;
    case WavFile::S24LE:
        return PA_SAMPLE_S24LE;
    case WavFile::S32LE:
        return PA_SAMPLE_S32LE;
    default:
        return PA_SAMPLE_INVALID;
    }
}

void PulsePcmSound::setPlaying(bool playing)
{
    if (m_isPlaying == playing)
        return;

    m_position = 0;
    m_isPlaying = playing;
    emit playingChanged();

    if (!playing) {
        m_playCount = 0;
    } else {
        m_playCount = m_loopCount;
        uploadSample();
    }
}

void PulsePcmSound::setPaused(bool paused)
{
    if (m_isPaused == paused)
        return;

    m_isPaused = paused;
    emit pausedChanged();

    if (!paused)
        uploadSample();
}

void PulsePcmSound::setLoopCount(int loopCount)
{
    if (m_loopCount == loopCount)
        return;

    m_loopCount = loopCount;
    emit loopsChanged();
}

void PulsePcmSound::setVolume(qreal volume)
{
    volume = qBound<qreal>(0.0, volume, 1.0);

    if (m_volume == volume)
        return;

    m_volume = volume;
    emit volumeChanged();

    updateVolume();
}

void PulsePcmSound::setMuted(bool muted)
{
    if (m_isMuted == muted)
        return;

    m_isMuted = muted;
    emit mutedChanged();

    updateVolume();
}

void PulsePcmSound::updateVolume()
{
    if (!m_isReady)
        return;

    PulseClient *pulse = pulse_instance();

    PulseLocker locker;

    pa_cvolume volume;
    volume.channels = m_sampleSpec.channels;

    for (int i = 0; i < volume.channels; ++i)
        volume.values[i] = m_isMuted ? PA_VOLUME_MUTED : PA_VOLUME_NORM * m_volume;

    pa_operation_unref(pa_context_set_sink_input_volume(pulse->context(), m_streamIndex, &volume, 0, 0));
}

void PulsePcmSound::createStream()
{
    if (m_stream || m_data.isEmpty())
        return;

    PulseClient *pulse = pulse_instance();
    pa_context *context = pulse->context();

    PulseLocker locker;
    const char *streamName = QString("PulsePcmSound:%1").arg(::getpid()).toAscii().data();

    if (!pa_sample_spec_valid(&m_sampleSpec)) {
        qWarning("PcmSound(pulse): Invalid sample spec");
        return;
    }

    if (!(m_stream = pa_stream_new(context, streamName, &m_sampleSpec, 0))) {
        qWarning("PcmSound(pulse): Unable to create stream (%s)", pa_strerror(pa_context_errno(context)));
        return;
    }

    pa_stream_set_state_callback(m_stream, stream_state_callback, this);
    pa_stream_set_write_callback(m_stream, stream_write_callback, this);
    pa_stream_set_suspended_callback(m_stream, stream_suspended_callback, this);
    pa_stream_set_underflow_callback(m_stream, stream_underflow_callback, this);
    pa_stream_set_overflow_callback(m_stream, stream_overflow_callback, this);

    if (pa_stream_connect_playback(m_stream, 0, 0, (pa_stream_flags_t)0, 0, 0) < 0) {
        qWarning("PcmSound(pulse): Unable to connect playback (%s)", pa_strerror(pa_context_errno(context)));
        return;
    }
}

void PulsePcmSound::deleteStream()
{
    if (!m_stream)
        return;

    PulseLocker locker;

    pa_stream_set_state_callback(m_stream, 0, 0);
    pa_stream_set_write_callback(m_stream, 0, 0);
    pa_stream_set_suspended_callback(m_stream, 0, 0);
    pa_stream_set_underflow_callback(m_stream, 0, 0);
    pa_stream_set_overflow_callback(m_stream,  0, 0);

    pa_stream_disconnect(m_stream);
    pa_stream_unref(m_stream);

    m_stream = 0;
    m_isReady = false;
    m_position = 0;
    m_playCount = 0;
    m_streamIndex = -1;
}

void PulsePcmSound::uploadSample()
{
    if (!m_isReady || m_isPaused || m_playCount == 0)
        return;

    if (m_position >= m_data.size()) {
        m_position = 0;

        // It's Infinite when playCount is negative.
        if (m_playCount > 0) {
            m_playCount--;
            if (m_playCount == 0) {
                setPlaying(false);
                return;
            }
        }
    }

    pa_context *context = pulse_instance()->context();

    int writableSize = pa_stream_writable_size(m_stream);
    int bufferLength = qMin<int>(writableSize, m_data.size() - m_position);

    if (pa_stream_write(m_stream, reinterpret_cast<uint8_t *>(m_data.data()) + m_position,
                        bufferLength, 0, 0, PA_SEEK_RELATIVE) < 0) {
        qWarning("PcmSound(pulse): Unable to write to stream (%s)", pa_strerror(pa_context_errno(context)));
        return;
    }

    // flush content
    if (bufferLength < writableSize)
        pa_stream_drain(m_stream, 0, 0);

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
        sound->m_isReady = true;
        sound->m_streamIndex = pa_stream_get_index(stream);
        sound->uploadSample();
        // Adjust volume out of this callback since it's a locking operation
        QMetaObject::invokeMethod(sound, "updateVolume", Qt::QueuedConnection);
        break;

    case PA_STREAM_FAILED:
    default:
        qWarning("PcmSound(pulse): Stream error (%s)",
                 pa_strerror(pa_context_errno(pa_stream_get_context(stream))));
        sound->m_isReady = false;
        sound->m_streamIndex = -1;
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

#include "pcmsound_pulse.moc"
