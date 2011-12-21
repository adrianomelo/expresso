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

#include "expresso.h"

#include <QDir>
#include <QFileInfoList>


Expresso::Expresso(QObject *parent)
    : QObject(parent)
{

}

QVariantList Expresso::listDirFiles(const QUrl &url, const QString &nameFilter) const
{
    QDir dir(url.toLocalFile());
    dir.setFilter(QDir::Files | QDir::Hidden);

    QVariantList result;
    const QFileInfoList &list = dir.entryInfoList(QStringList(nameFilter));

    foreach (const QFileInfo &info, list)
        result.append(QUrl::fromLocalFile(info.absoluteFilePath()));

    return result;
}
