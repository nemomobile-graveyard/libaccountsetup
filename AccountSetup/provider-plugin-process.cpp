/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2011 Nokia Corporation.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "provider-plugin-process-priv.h"

#include <Accounts/Account>
#include <Accounts/Manager>

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QLocalSocket>
#include <QVariant>

using namespace AccountSetup;

static ProviderPluginProcess *plugin_instance = 0;
const int cancelId = -1;

ProviderPluginProcessPrivate::ProviderPluginProcessPrivate(ProviderPluginProcess *parent):
    q_ptr(parent),
    setupType(Unset),
    windowId(0),
    goToAccountsPage(false),
    exitData(),
    editExistingAccount(false),
    existingAccountId(0)
{
    account = 0;
    manager = new Accounts::Manager(this);

    /* parse command line options */
    QStringList args = QCoreApplication::arguments();
    for (int i = 0; i < args.length(); ++i)
    {
        Q_ASSERT(args[i] != NULL);

        if (args[i] == QLatin1String("--create"))
        {
            setupType = CreateNew;

            i++;
            if (i < args.length()) {
                account = manager->createAccount(args[i]);
            }
        }
        else if (args[i] == QLatin1String("--edit"))
        {
            setupType = EditExisting;

            i++;
            if (i < args.length())
                account = manager->account(args[i].toInt());
        }
        else if (args[i] == QLatin1String("--windowId"))
        {
            i++;
            if (i < args.length())
                windowId = (WId)args[i].toUInt();
            Q_ASSERT(windowId != 0);
        }
        else if (args[i] == QLatin1String("--socketName"))
        {
            i++;
            if (i < args.length())
                socketName = args[i];
            Q_ASSERT(socketName != 0);
        }
        else if (args[i] == QLatin1String("--serviceType"))
        {
            i++;
            if (i < args.length())
                serviceType = args[i];
            Q_ASSERT(serviceType != 0);
        }
    }
}

ProviderPluginProcessPrivate::~ProviderPluginProcessPrivate()
{
}

void ProviderPluginProcessPrivate::sendResultToCaller()
{
    if (!socketName.isEmpty()) {
        QLocalSocket *socket = new QLocalSocket();
        connect(socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
                this, SLOT(onSocketError(QLocalSocket::LocalSocketError)));

        socket->connectToServer(socketName);
        socket->waitForConnected();

        QByteArray ba;
        QDataStream stream(&ba, QIODevice::WriteOnly);

        if (editExistingAccount)
            stream << existingAccountId;
        else if (!goToAccountsPage)
            stream << account->id();
        else
            stream << cancelId;

        stream << exitData;
        socket->write(ba);
        socket->flush();

        socket->close();
        delete socket;
    } else {
        QByteArray ba;
        if (editExistingAccount)
            ba = QString::number(existingAccountId).toAscii();
        else if (!goToAccountsPage)
            ba = QString::number(account->id()).toAscii();
        else
            ba = QString::number(cancelId).toAscii();

        QFile output;
        output.open(STDOUT_FILENO, QIODevice::WriteOnly);
        output.write(ba.constData());
        output.close();
    }
}

void ProviderPluginProcessPrivate::onSocketError(QLocalSocket::LocalSocketError status)
{
    qDebug() << Q_FUNC_INFO << status;
}

ProviderPluginProcess::ProviderPluginProcess(QObject *object):
    QObject(object),
    d_ptr(new ProviderPluginProcessPrivate(this))
{
    if (plugin_instance != 0)
        qWarning() << "ProviderPluginProcess already instantiated";
    plugin_instance = this;
}

ProviderPluginProcess::~ProviderPluginProcess()
{
    Q_D(ProviderPluginProcess);
    delete d;
}

ProviderPluginProcess *ProviderPluginProcess::instance()
{
    return plugin_instance;
}

SetupType ProviderPluginProcess::setupType() const
{
    Q_D(const ProviderPluginProcess);
    return d->setupType;
}

Accounts::Account *ProviderPluginProcess::account() const
{
    Q_D(const ProviderPluginProcess);
    return d->account;
}

QString ProviderPluginProcess::serviceType() const
{
    Q_D(const ProviderPluginProcess);
    return d->serviceType;
}

WId ProviderPluginProcess::parentWindowId() const
{
    Q_D(const ProviderPluginProcess);
    return d->windowId;
}

void ProviderPluginProcess::setReturnToAccountsList(bool value)
{
    Q_D(ProviderPluginProcess);
    /* goToAccountsPage is only true when plugin is stopped in between without
      creating the account */
    d->goToAccountsPage = value;
    quit();

}

void ProviderPluginProcess::setExitData(const QVariant &data)
{
    Q_D(ProviderPluginProcess);
    d->exitData = data;
}

void ProviderPluginProcess::setEditExistingAccount(Accounts::AccountId accountId)
{
    Q_D(ProviderPluginProcess);
    d->editExistingAccount = true;
    d->existingAccountId = accountId;
}

void ProviderPluginProcess::quit()
{
    Q_D(ProviderPluginProcess);
    d->sendResultToCaller();
    QCoreApplication::exit(0);
}

