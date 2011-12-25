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

#include "pcmsound.h"

#ifdef PCMSOUND_PULSE
#  include "pcmsound_pulse.h"
#else
#  include "pcmsound_dummy.h"
#endif

#include <QFile>
#include <QDebug>

WavFile::WavFile()
    : m_loaded(false),
      m_channels(1),
      m_sampleRate(16000)
{

}

bool WavFile::load(const QUrl &url)
{
    m_url = url;

    QFile fp(url.toLocalFile());

    if (!fp.open(QIODevice::ReadOnly)) {
        m_loaded = false;
        return false;
    }

    struct header {
        char chunkId[4];
        quint32 chunkSIze;
        char format[4];
        char subChunk1Id[4];
        quint32 subChunk1Size;
        quint16 audioFormat;
        quint16 numChannels;
        quint32 sampleRate;
        quint32 byteRate;
        quint16 blockAlign;
        quint16 bitsPerSample;
        char subChunk2Id[4];
        quint32 subChunk2Size;
    } header;

    const int sz = sizeof(struct header);
    if (fp.read(reinterpret_cast<char *>(&header), sz) != sz)
        return false;

    const QByteArray cid(header.chunkId, 4);
    const QByteArray format(header.format, 4);
    const QByteArray subCid(header.subChunk1Id, 4);

    if (cid != "RIFF" || format != "WAVE" || subCid != "fmt " ||
        header.subChunk1Size != 16 || header.audioFormat != 1) {
        qWarning() << "PcmSound: Invalid sound format. " << url.toLocalFile();
        return false;
    }

    m_channels = header.numChannels;
    m_sampleRate = header.sampleRate;

    m_data = fp.readAll();
    return true;
}


PcmSound::PcmSound(QObject *parent)
  : QObject(parent),
    m_sound(0),
    m_loopCount(1),
    m_muted(false),
    m_paused(false),
    m_doPlay(false),
    m_completed(false)
{

}

void PcmSound::componentComplete()
{
    m_completed = true;
    updatePcmSound();

    if (m_doPlay)
        play();
}

void PcmSound::play()
{
    if (m_sound)
        m_sound->play();

    m_doPlay = true;
}

void PcmSound::stop()
{
    if (m_sound)
        m_sound->stop();

    m_doPlay = false;
}

bool PcmSound::isPaused() const
{
    return m_paused;
}

void PcmSound::setPaused(bool paused)
{
    if (m_paused == paused)
        return;

    m_paused = paused;
    emit pausedChanged();

    if (m_sound)
        m_sound->setPaused(paused);
}

bool PcmSound::isMuted() const
{
    return m_muted;
}

void PcmSound::setMuted(bool muted)
{
    if (m_muted == muted)
        return;

    m_muted = muted;
    emit mutedChanged();

    if (m_sound)
        m_sound->setMuted(muted);
}

QUrl PcmSound::source() const
{
    return m_source;
}

void PcmSound::setSource(const QUrl &url)
{
    if (m_source != url) {
        m_source = url;
        updatePcmSound();
        emit sourceChanged();
    }
}

int PcmSound::loopCount() const
{
    return m_loopCount;
}

void PcmSound::setLoopCount(int count)
{
    if (m_loopCount == count)
        return;

    m_loopCount = count;
    emit loopCountChanged();

    if (m_sound)
        m_sound->setLoopCount(count);
}

void PcmSound::updatePcmSound()
{
    if (!m_completed)
        return;

    if (m_sound)
        delete m_sound;

    WavFile file;
    file.load(m_source.toLocalFile());

#ifdef PCMSOUND_PULSE
    m_sound = new PulsePcmSound(file, this);
#else
    m_sound = new DummyPcmSound(file, this);
#endif

    m_sound->setMuted(m_muted);
    m_sound->setPaused(m_paused);
    m_sound->setLoopCount(m_loopCount);
}
