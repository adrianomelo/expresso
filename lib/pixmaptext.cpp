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

#include "pixmaptext.h"
#include "common.h"

#include <QPainter>


PixmapText::PixmapText(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

void PixmapText::setValue(const QString &value)
{
    if (m_value != value) {
        m_value = value;
        emit valueChanged();
        updateGeometry();
    }
}

void PixmapText::setSource(const QUrl &url)
{
    if (m_source != url) {
        m_source = url;
        m_pixmap = loadPixmap(this, url);
        emit sourceChanged();
        updateGeometry();
    }
}

void PixmapText::setCharMap(const QVariantMap &map)
{
    if (m_charMap != map) {
        m_charMap = map;
        emit charMapChanged();
        updateGeometry();
    }
}

void PixmapText::updateGeometry()
{
    const int len = m_value.length();

    m_rects.clear();

    int w = 0, h = 0;

    for (int i = 0; i < len; i++) {
        const QChar &c = m_value.at(i);

        if (!m_charMap.contains(c))
            continue;

        const QVariant &v = m_charMap.value(c);

        if (!v.canConvert<QRect>())
            continue;

        const QRect &rect = v.toRect();

        m_rects.append(rect);

        w += rect.width();
        h = qMax<int>(h, rect.height());
    }

    setImplicitWidth(w);
    setImplicitHeight(h);

    update();
}

void PixmapText::paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget)
{
    Q_UNUSED(options);
    Q_UNUSED(widget);

    int x = 0;
    const int w = width();
    const int h = height();

    const int len = m_rects.length();

    for (int i = 0; i < len; i++) {
        const QRect &rect = m_rects.at(i);

        if (x + rect.width() > w)
            break;

        QRect target = rect;

        // shrink
        if (target.height() > h)
            target.setHeight(h);

        target.moveTo(x, h - target.height());
        x += rect.width();

        painter->drawPixmap(target, m_pixmap, rect);
    }
}
