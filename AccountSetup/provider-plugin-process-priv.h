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

#ifndef ACCOUNTSETUP_PROVIDER_PLUGIN_PROCESS_PRIV_H
#define ACCOUNTSETUP_PROVIDER_PLUGIN_PROCESS_PRIV_H

//libAccountSetup
#include "provider-plugin-process.h"

//Accounts
#include <Accounts/account.h>
#include <Accounts/manager.h>

//Qt
#include <QLocalSocket>

namespace AccountSetup {

class ProviderPluginProcessPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProviderPluginProcess)

public:
    ProviderPluginProcessPrivate(ProviderPluginProcess *parent);
    ~ProviderPluginProcessPrivate();

    void sendResultToCaller();

public Q_SLOTS:
    void onSocketError(QLocalSocket::LocalSocketError errorStatus);

private:
    mutable ProviderPluginProcess *q_ptr;
    SetupType setupType;
    WId windowId;
    Accounts::Manager *manager;
    Accounts::Account *account;
    QString serviceType;
    bool returnToApp;
    QString socketName;
    bool goToAccountsPage;
    QVariant exitData;
    bool editExistingAccount;
    Accounts::AccountId existingAccountId;
};

} // namespace
#endif // ACCOUNTSETUP_PROVIDER_PLUGIN_PROCESS_PRIV_H
