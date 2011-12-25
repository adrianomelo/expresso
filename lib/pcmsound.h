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
#include <QDeclarativeParserStatus>

class WavFile
{
public:
    WavFile();

    bool isLoaded() const { return m_loaded; }
    QByteArray data() const { return m_data; }

    int channels() const { return m_channels; }
    int sampleRate() const { return m_sampleRate; }

    bool load(const QUrl &url);

private:
    QUrl m_url;
    bool m_loaded;
    QByteArray m_data;
    int m_channels;
    int m_sampleRate;
};


class AbstractPcmSound : public QObject
{
    Q_OBJECT

public:
    AbstractPcmSound(const WavFile &file, QObject *parent = 0)
        : QObject(parent) { Q_UNUSED(file); }

    virtual int loopCount() const = 0;
    virtual void setLoopCount(int loopCount) = 0;

    virtual bool isMuted() const = 0;
    virtual void setMuted(bool muted) = 0;

    virtual void setVolume(qreal value) = 0;

    virtual bool isPaused() const = 0;
    virtual void setPaused(bool paused) = 0;

signals:
    void finished();

public slots:
    virtual void play() = 0;
    virtual void stop() = 0;
};


class PcmSound : public QObject,
                 public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(int loopCount READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)

public:
    PcmSound(QObject *parent = 0);

    QUrl source() const;
    void setSource(const QUrl &url);

    int loopCount() const;
    void setLoopCount(int count);

    bool isPaused() const;
    void setPaused(bool paused);

    bool isMuted() const;
    void setMuted(bool muted);

    void classBegin() {}
    void componentComplete();

public slots:
    void play();
    void stop();

signals:
    void mutedChanged();
    void pausedChanged();
    void sourceChanged();
    void loopCountChanged();

private:
    void updatePcmSound();

    AbstractPcmSound *m_sound;
    QUrl m_source;
    int m_loopCount;
    bool m_muted;
    bool m_paused;
    bool m_doPlay;
    bool m_completed;
};

#endif
