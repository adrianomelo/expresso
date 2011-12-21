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

#ifndef EXPRESSO_H
#define EXPRESSO_H

#include <QUrl>
#include <QObject>
#include <QVariantList>


class Expresso : public QObject
{
    Q_OBJECT

public:
    Expresso(QObject *parent = 0);

    Q_INVOKABLE QVariantList listDirFiles(const QUrl &url, const QString &nameFilter) const;
};

#endif
