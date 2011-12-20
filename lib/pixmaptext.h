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

#ifndef PIXMAPTEXT_H
#define PIXMAPTEXT_H

#include <QUrl>
#include <QVariantMap>
#include <QDeclarativeItem>


class PixmapText : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QVariantMap charMap READ charMap WRITE setCharMap NOTIFY charMapChanged)

public:
    PixmapText(QDeclarativeItem *parent = 0);

    QString value() const { return m_value; }
    void setValue(const QString &value);

    QUrl source() const { return m_source; }
    void setSource(const QUrl &url);

    QVariantMap charMap() const { return m_charMap; }
    void setCharMap(const QVariantMap &map);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget = 0);

signals:
    void valueChanged();
    void sourceChanged();
    void charMapChanged();

protected:
    void updateGeometry();

private:
    QUrl m_source;
    QString m_value;
    QPixmap m_pixmap;
    QVariantMap m_charMap;
    QList<QRect> m_rects;
};

#endif
