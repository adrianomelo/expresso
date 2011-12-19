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

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QUrl>
#include <QHash>
#include <QObject>
#include <QThread>
#include <QDeclarativeParserStatus>
#include <QDeclarativeImageProvider>


class ImageLoader;

class ImageWorker : public QObject
{
    Q_OBJECT

public:
    ImageWorker(QObject *parent = 0);

    void resolvePath(const QString &prefix);

public slots:
    void load();

signals:
    void loaded();
    void imageReady(const QString &path, const QImage &image);

private:
    QStringList listSubPaths(const QString &rootPath);

    QStringList m_paths;
};


class ImageProvider : public QDeclarativeImageProvider
{
public:
    ImageProvider(ImageLoader *loader);

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

private:
    ImageLoader *m_loader;
};


class ImageLoader : public QObject,
                    public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl folder READ folder WRITE setFolder NOTIFY folderChanged)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)

public:
    enum Status {
        Null,
        Loading,
        Loaded
    };

    ImageLoader(QObject *parent = 0);

    Status status() const { return m_status; }

    QUrl folder() const { return m_folder; }
    void setFolder(const QUrl &url);

    QString prefix() const { return m_prefix; }
    void setPrefix(const QString &prefix);

    QPixmap findPixmap(const QString &key);

    void classBegin() {}
    void componentComplete();

signals:
    void folderChanged();
    void prefixChanged();
    void statusChanged();

protected slots:
    void onLoaded();
    void onImageReady(const QString &path, const QImage &image);

private:
    void updateDataSet();
    void updateProvider();
    void setStatus(Status status);

    QUrl m_folder;
    Status m_status;
    bool m_completed;
    QThread m_thread;
    QString m_prefix;
    QString m_oldPrefix;
    ImageWorker *m_worker;
    ImageProvider *m_provider;
    QHash<QString, QPixmap> m_pixmaps;
};

#endif
