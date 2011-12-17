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

#ifndef PCMSOUND_H
#define PCMSOUND_H

#include <QUrl>
#include <QObject>
#include <QByteArray>


class WavFile
{
public:
    WavFile();

    bool isLoaded() const { return m_loaded; }
    QByteArray data() const { return m_data; }

    bool load(const QUrl &url);

private:
    QUrl m_url;
    bool m_loaded;
    QByteArray m_data;
};


class AbstractPcmSound : public QObject
{
    Q_OBJECT

public:
    AbstractPcmSound(const WavFile &file, QObject *parent = 0)
        : QObject(parent) { Q_UNUSED(file); }

    virtual int loopCount() const = 0;
    virtual void setLoopCount(int loopCount) = 0;

    virtual void setMuted(bool muted) = 0;
    virtual void setVolume(qreal value) = 0;

signals:
    void finished();

public slots:
    virtual void play() = 0;
    virtual void stop() = 0;
};


class PcmSound : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int loopCount READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)

public:
    PcmSound(QObject *parent = 0);

    QUrl source() const;
    void setSource(const QUrl &url);

    int loopCount() const;
    void setLoopCount(int count);

public slots:
    void play();
    void stop();

signals:
    void sourceChanged();
    void loopCountChanged();

private:
    AbstractPcmSound *m_sound;
    QUrl m_source;
    int m_loopCount;
};

#endif
