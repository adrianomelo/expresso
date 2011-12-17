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

#include "rawconnection.h"

#define Q_SLOT_STR(a)   QString("1%1").arg(a).toAscii().data()
#define Q_SIGNAL_STR(a) QString("2%1").arg(a).toAscii().data()


RawConnection::RawConnection(QObject *parent)
    : QObject(parent),
      m_completed(false),
      m_connected(false),
      m_sender(0),
      m_receiver(0)
{

}

void RawConnection::setSender(QObject *sender)
{
    if (m_sender != sender) {
        checkOldConnection();
        m_sender = sender;
        checkNewConnection();
        emit senderChanged();
    }
}

void RawConnection::setReceiver(QObject *receiver)
{
    if (m_receiver != receiver) {
        checkOldConnection();
        m_receiver = receiver;
        checkNewConnection();
        emit receiverChanged();
    }
}

void RawConnection::setSenderSignal(const QString &signature)
{
    if (m_senderSignal != signature) {
        checkOldConnection();
        m_senderSignal = signature;
        checkNewConnection();
        emit senderSignalChanged();
    }
}

void RawConnection::setReceiverSlot(const QString &signature)
{
    if (m_receiverSlot != signature) {
        checkOldConnection();
        m_receiverSlot = signature;
        checkNewConnection();
        emit receiverSlotChanged();
    }
}

void RawConnection::componentComplete()
{
    m_completed = true;
    checkNewConnection();
}

void RawConnection::checkOldConnection()
{
    if (!m_completed || !m_connected)
        return;

    m_connected = false;
    QObject::disconnect(m_sender, Q_SIGNAL_STR(m_senderSignal),
                        m_receiver, Q_SLOT_STR(m_receiverSlot));
}

void RawConnection::checkNewConnection()
{
    if (!m_completed || !m_sender || !m_receiver)
        return;

    if (m_senderSignal.isEmpty() || m_receiverSlot.isEmpty())
        return;

    m_connected = QObject::connect(m_sender, Q_SIGNAL_STR(m_senderSignal),
                                   m_receiver, Q_SLOT_STR(m_receiverSlot));
}
