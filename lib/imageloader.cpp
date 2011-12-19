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

#include "imageloader.h"

#include <QDir>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

ImageWorker::ImageWorker(QObject *parent)
    : QObject(parent)
{

}

void ImageWorker::load()
{
    foreach (const QString &path, m_paths) {
        QImage image(path);
        emit imageReady(path, image);
    }

    emit loaded();
}

void ImageWorker::resolvePath(const QString &prefix)
{
    m_paths = listSubPaths(prefix);
}

QStringList ImageWorker::listSubPaths(const QString &rootPath)
{
    QStringList result;

    QDir rootDir(rootPath);
    const QFileInfoList &lst = rootDir.entryInfoList(QDir::Dirs | QDir::Files |
                                                     QDir::NoDotAndDotDot);

    foreach (const QFileInfo &info, lst) {
        // do not cache special files
        if (info.fileName().startsWith("_"))
            continue;

        if (info.isDir()) {
            result.append(listSubPaths(info.absoluteFilePath()));
        } else if (info.suffix() == "png" || info.suffix() == "jpg") {
            result.append(info.absoluteFilePath());
        }
    }

    return result;
}


ImageProvider::ImageProvider(ImageLoader *loader)
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap),
      m_loader(loader)
{

}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size,
                                     const QSize &requestedSize)
{
    Q_UNUSED(size);
    Q_UNUSED(requestedSize);

    return m_loader->findPixmap(id);
}


ImageLoader::ImageLoader(QObject *parent)
    : QObject(parent),
      QDeclarativeParserStatus(),
      m_status(Null),
      m_completed(false),
      m_prefix("image")
{
    m_worker = new ImageWorker();

    connect(m_worker, SIGNAL(loaded()), this, SLOT(onLoaded()));
    connect(m_worker, SIGNAL(loaded()), &m_thread, SLOT(quit()));
    connect(m_worker, SIGNAL(imageReady(const QString &, const QImage &)),
            this, SLOT(onImageReady(const QString &, const QImage &)));

    m_worker->connect(&m_thread, SIGNAL(started()), SLOT(load()));
    m_worker->moveToThread(&m_thread);
}

void ImageLoader::setStatus(Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}


void ImageLoader::setFolder(const QUrl &folder)
{
    if (m_folder != folder) {
        m_folder = folder;
        updateDataSet();
        emit folderChanged();
    }
}

void ImageLoader::setPrefix(const QString &prefix)
{
    if (m_prefix != prefix) {
        m_prefix = prefix;
        updateProvider();
        emit prefixChanged();
    }
}

void ImageLoader::componentComplete()
{
    m_completed = true;
    updateProvider();
    updateDataSet();
}

void ImageLoader::updateProvider()
{
    if (!m_completed)
        return;

    QDeclarativeContext *ctxt = QDeclarativeEngine::contextForObject(this);

    if (ctxt) {
        QDeclarativeEngine *engine = ctxt->engine();

        if (engine) {
            if (!m_oldPrefix.isEmpty() && m_oldPrefix != m_prefix)
                engine->removeImageProvider(m_oldPrefix);

            m_oldPrefix = m_prefix;
            engine->addImageProvider(m_prefix, new ImageProvider(this));
        }
    }
}

void ImageLoader::updateDataSet()
{
    if (!m_completed)
        return;

    setStatus(Loading);

    m_pixmaps.clear();
    m_worker->resolvePath(m_folder.toLocalFile());
    m_thread.start();
}

QPixmap ImageLoader::findPixmap(const QString &key)
{
    const QDir fp(m_folder.toLocalFile());

    const QString &path = fp.absoluteFilePath(key);

    if (m_pixmaps.contains(path))
        return m_pixmaps.value(path);
    else {
        QPixmap pixmap(path);
        m_pixmaps.insert(path, pixmap);
        return pixmap;
    }
}

void ImageLoader::onLoaded()
{
    setStatus(Loaded);
}

void ImageLoader::onImageReady(const QString &path, const QImage &image)
{
    m_pixmaps.insert(path, QPixmap::fromImage(image));
}
