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

#ifndef RAWCONNECTION_H
#define RAWCONNECTION_H

#include <QObject>
#include <QDeclarativeParserStatus>


class RawConnection : public QObject,
                      public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QObject *sender READ sender WRITE setSender NOTIFY senderChanged)
    Q_PROPERTY(QObject *receiver READ receiver WRITE setReceiver NOTIFY receiverChanged)
    Q_PROPERTY(QString senderSignal READ senderSignal WRITE setSenderSignal NOTIFY senderSignalChanged)
    Q_PROPERTY(QString receiverSlot READ receiverSlot WRITE setReceiverSlot NOTIFY receiverSlotChanged)

public:
    RawConnection(QObject *parent = 0);

    QObject *sender() const { return m_sender; }
    void setSender(QObject *sender);

    QObject *receiver() const { return m_receiver; }
    void setReceiver(QObject *receiver);

    QString senderSignal() const { return m_senderSignal; }
    void setSenderSignal(const QString &signature);

    QString receiverSlot() const { return m_receiverSlot; }
    void setReceiverSlot(const QString &signature);

    void classBegin() {}
    void componentComplete();

signals:
    void senderChanged();
    void receiverChanged();
    void senderSignalChanged();
    void receiverSlotChanged();

private:
    void checkOldConnection();
    void checkNewConnection();

    bool m_completed;
    bool m_connected;
    QObject *m_sender;
    QObject *m_receiver;
    QString m_senderSignal;
    QString m_receiverSlot;
};

#endif
