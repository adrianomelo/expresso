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


WavFile::WavFile()
    : m_loaded(false)
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

    // XXX: read header info
    fp.seek(44);

    m_data = fp.readAll();
    return true;
}


PcmSound::PcmSound(QObject *parent)
  : QObject(parent),
    m_sound(0),
    m_loopCount(1)
{

}

void PcmSound::play()
{
    if (m_sound)
        m_sound->play();
}

void PcmSound::stop()
{
    if (m_sound)
        m_sound->stop();
}

QUrl PcmSound::source() const
{
    return m_source;
}

void PcmSound::setSource(const QUrl &url)
{
    if (m_source == url)
        return;

    m_source = url;

    if (m_sound)
        delete m_sound;

    WavFile file;
    file.load(url.toLocalFile());

#ifdef PCMSOUND_PULSE
    m_sound = new PulsePcmSound(file, this);
#else
    m_sound = new DummyPcmSound(file, this);
#endif

    m_sound->setLoopCount(m_loopCount);

    emit sourceChanged();
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

    if (m_sound)
        m_sound->setLoopCount(count);

    emit loopCountChanged();
}
