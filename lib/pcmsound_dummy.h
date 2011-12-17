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
    DummyPcmSound(const WavFile &file, QObject *parent = 0)
        : AbstractPcmSound(file, parent),
          m_loopCount(-1)
    {
        qWarning("PcmSound: No sound engine");
    }

    int loopCount() const { return m_loopCount; }
    void setLoopCount(int loopCount) { m_loopCount = loopCount; }

    void setMuted(bool) {}
    void setVolume(qreal) {}

signals:
    void finished();

public slots:
    void play() {}
    void stop() {}

private:
    int m_loopCount;
};

#endif
