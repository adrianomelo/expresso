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

#include <QFile>
#include <QDebug>


WavFile::WavFile()
    : m_loaded(false),
      m_channels(1),
      m_sampleRate(16000),
      m_format(Invalid)
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
        qint16 audioFormat;
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

    const bool validChunk = (header.subChunk1Size == 16 && header.audioFormat == 1) ||
        (header.subChunk1Size == 40 && header.audioFormat == -2);

    if (cid != "RIFF" || format != "WAVE" || subCid != "fmt " || !validChunk) {
        qWarning() << "PcmSound: Invalid sound format. " << url.toLocalFile();
        return false;
    }

    switch (header.bitsPerSample) {
    case 8:
        m_format = WavFile::U8;
        break;
    case 16:
        m_format = WavFile::S16LE;
        break;
    case 24:
        m_format = WavFile::S24LE;
        break;
    case 32:
        m_format = WavFile::S32LE;
        break;
    default:
        m_format = WavFile::Invalid;
        break;
    }

    m_channels = header.numChannels;
    m_sampleRate = header.sampleRate;

    m_data = fp.readAll();
    return true;
}
