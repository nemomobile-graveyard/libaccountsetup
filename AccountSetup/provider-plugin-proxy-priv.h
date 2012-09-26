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

#ifndef ACCOUNTSETUP_PROVIDER_PLUGIN_PROXY_PRIV_H
#define ACCOUNTSETUP_PROVIDER_PLUGIN_PROXY_PRIV_H

//libAccountSetup
#include "provider-plugin-proxy.h"

//Qt
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLocalServer>
#include <QPointer>
#include <QProcess>
#include <QWidget>

using namespace Accounts;
using namespace AccountSetup;

namespace AccountSetup {

class ProviderPluginProxyPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProviderPluginProxy)

public:
    ProviderPluginProxyPrivate(ProviderPluginProxy *parent):
        q_ptr(parent),
        pluginName(),
        process(0),
        socketName(QString()),
        createdAccountId(0),
        error(ProviderPluginProxy::NoError),
        setupType(Unset),
        providerName(),
        exitData()
    {
        pluginDirs << QString::fromLatin1("/usr/lib/AccountSetup");
    }
    ~ProviderPluginProxyPrivate();

    void startProcess(Provider provider, AccountId accountId,
                      const QString &serviceType);
    bool findPlugin(Provider provider, QString &pluginPath,
                    QString &pluginFileName);

private Q_SLOTS:
    void onReadStandardError();
    void onError(QProcess::ProcessError);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onNewConnection();
    void setCommunicationChannel();

private:
    mutable ProviderPluginProxy *q_ptr;
    QString pluginName;
    QProcess *process;
    QString socketName;
    AccountId createdAccountId;
    QStringList pluginDirs;
    ProviderPluginProxy::Error error;
    QPointer<QWidget> parentWidget;
    QStringList additionalParameters;
    QByteArray pluginOutput;
    SetupType setupType;
    QString providerName;
    QVariant exitData;
};

}; // namespace

#endif // ACCOUNTSETUP_PROVIDER_PLUGIN_PROXY_PRIV_H

