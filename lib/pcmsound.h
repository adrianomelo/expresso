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
    enum Format {
        Invalid,
        U8,
        S16LE,
        S24LE,
        S32LE,
    };

    WavFile();

    bool isLoaded() const { return m_loaded; }
    QByteArray data() const { return m_data; }

    int channels() const { return m_channels; }
    int sampleRate() const { return m_sampleRate; }
    Format format() const { return m_format; }


    bool load(const QUrl &url);

private:
    QUrl m_url;
    bool m_loaded;
    QByteArray m_data;
    int m_channels;
    int m_sampleRate;
    Format m_format;
};


class AbstractPcmSound : public QObject,
                         public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_ENUMS(Loops)
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(int loops READ loopCount WRITE setLoopCount NOTIFY loopsChanged)

public:
    enum Loops {
        Infinite = -2
    };

    AbstractPcmSound(QObject *parent = 0)
        : QObject(parent) {}

    virtual QUrl source() const = 0;
    virtual void setSource(const QUrl &url) = 0;

    virtual bool isPlaying() const = 0;
    virtual void setPlaying(bool playing) = 0;

    virtual int loopCount() const = 0;
    virtual void setLoopCount(int count) = 0;

    virtual bool isPaused() const = 0;
    virtual void setPaused(bool paused) = 0;

    virtual bool isMuted() const = 0;
    virtual void setMuted(bool muted) = 0;

    virtual qreal volume() const = 0;
    virtual void setVolume(qreal volume) = 0;

    void classBegin() {}
    void componentComplete() {}

public slots:
    void play() { setPlaying(true); }
    void stop() { setPlaying(false); }

signals:
    void mutedChanged();
    void pausedChanged();
    void sourceChanged();
    void volumeChanged();
    void playingChanged();
    void loopsChanged();
};

#endif
