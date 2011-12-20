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

#include "plugin.h"
#include "sprite.h"
#include "pixmaptext.h"
#include "rawconnection.h"


void ExpressoPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Expresso"));
    qmlRegisterType<Sprite>(uri, 1, 0, "Sprite");
    qmlRegisterType<SpriteState>(uri, 1, 0, "SpriteState");
    qmlRegisterType<SpriteScene>(uri, 1, 0, "SpriteScene");
    qmlRegisterType<PixmapText>(uri, 1, 0, "PixmapText");
    qmlRegisterType<RawConnection>(uri, 1, 0, "RawConnection");
}

Q_EXPORT_PLUGIN2(expressoplugin, ExpressoPlugin);
