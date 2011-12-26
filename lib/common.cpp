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

#include "common.h"
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeImageProvider>


QPixmap loadPixmap(QObject *obj, const QUrl &source)
{
    if (source.scheme() != "image")
        return QPixmap(source.toLocalFile());

    QDeclarativeContext *ctxt = QDeclarativeEngine::contextForObject(obj);

    if (ctxt) {
        QDeclarativeEngine *engine = ctxt->engine();

        if (engine) {
            QString path = source.path();

            if (path.startsWith('/'))
                path = path.remove(0, 1);

            QDeclarativeImageProvider *prov = engine->imageProvider(source.host());

            if (prov) {
                QSize sz;
                QSize rsz;
                return prov->requestPixmap(path, &sz, rsz);
            }
        }
    }

    return QPixmap();
}
