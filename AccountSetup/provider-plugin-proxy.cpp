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

#include "provider-plugin-proxy.h"
#include "provider-plugin-proxy-priv.h"

#include <Accounts/Manager>

#include <QDebug>
#include <QLocalServer>
#include <QLocalSocket>

using namespace Accounts;
using namespace AccountSetup;


ProviderPluginProxyPrivate::~ProviderPluginProxyPrivate()
{
    qDebug() << Q_FUNC_INFO;
    if (process) {
        process->disconnect();
        process->close();
        delete process;
    }
}

void ProviderPluginProxyPrivate::startProcess(Provider *provider,
                                              AccountId accountId,
                                              const QString &serviceType)
{
    Q_Q(ProviderPluginProxy);

    error = ProviderPluginProxy::NoError;
    createdAccountId = 0;
    pluginOutput.clear();

    QString processName;
    QString pluginFileName;

    if (!findPlugin(provider, processName, pluginFileName)) {
        error = ProviderPluginProxy::PluginNotFound;
        emit q->finished();
        return;
    }
    providerName = provider->name();
    pid_t pid = getpid();
    socketName = provider->name() + QString::number(pid);

    QStringList arguments;
    arguments << QLatin1String("--socketName") << socketName;

    if (parentWidget != 0) {
        WId windowId = parentWidget->effectiveWinId();
        arguments << QLatin1String("--windowId") << QString::number(windowId);
    }

    if (accountId != 0) {
        arguments << QLatin1String("--edit") << QString::number(accountId);
        setupType = EditExisting;
    } else {
        arguments << QLatin1String("--create") << provider->name();
        setupType = CreateNew;
    }

    if (!serviceType.isEmpty())
        arguments << QLatin1String("--serviceType") << serviceType;

    arguments += additionalParameters;

#ifndef QT_NO_DEBUG_OUTPUT
    arguments << QLatin1String("-output-level") << QLatin1String("debug");
#endif

    if (!process)
        process = new QProcess();

    pluginName = pluginFileName;

    qDebug() << Q_FUNC_INFO << processName << arguments;

    connect(process, SIGNAL(readyReadStandardError()),
            this, SLOT(onReadStandardError()));
    connect(process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(onError(QProcess::ProcessError)));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onFinished(int, QProcess::ExitStatus)));
    connect(process, SIGNAL(started()), this, SLOT(setCommunicationChannel()));

    process->start(processName, arguments);
}

bool ProviderPluginProxyPrivate::findPlugin(Provider *provider,
                                            QString &pluginPath,
                                            QString &pluginFileName)
{
    static const char pluginNamePattern[] = "%1plugin";
    bool pluginTagExists = true;

    QDomElement root(provider->domDocument().documentElement());
    QString pluginName(root.
                       firstChildElement(QString::fromLatin1("plugin")).
                       text());
    if (pluginName.isEmpty()) {
        pluginName = provider->name();
        pluginTagExists = false;
    }

    QStringList pluginFileNames;
    pluginFileNames << QString::fromLatin1(pluginNamePattern).arg(pluginName);

    /* If a plugin for the specified name cannot be found and
     * the plugin is not specified in the provider file, fallback to
     * "genericplugin"
     */
    if (!pluginTagExists) {
        pluginFileNames << QString::fromLatin1(pluginNamePattern).
            arg(QLatin1String("generic"));
    }

    foreach (QString name, pluginFileNames) {
        foreach (QString pluginDir, pluginDirs) {
            QFileInfo pluginFileInfo(pluginDir, name);

            if (pluginFileInfo.exists()) {
                pluginPath = pluginFileInfo.canonicalFilePath();
                pluginFileName = name;
                return true;
            }
        }
    }

    return false;
}

void ProviderPluginProxyPrivate::setCommunicationChannel()
{
    QLocalServer *server = new QLocalServer();
    QLocalServer::removeServer(socketName);
    if (!server->listen(socketName))
        qWarning() << "Server not up";
    else
        connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void ProviderPluginProxyPrivate::onNewConnection()
{
    QLocalServer *server = qobject_cast<QLocalServer*>(sender());
    QLocalSocket *socket = server->nextPendingConnection();
    if (!socket->waitForConnected()) {
        qWarning() << "Server Connection not established";
        return;
    }
    if (!socket->waitForReadyRead()) {
        qWarning() << "Server data not available for reading";
        return;
    }
    pluginOutput = socket->readAll();
    socket->close();
}


void ProviderPluginProxyPrivate::onReadStandardError()
{
    qDebug() << QString::fromLatin1(process->readAllStandardError());
}

void ProviderPluginProxyPrivate::onError(QProcess::ProcessError err)
{
    Q_Q(ProviderPluginProxy);

    if (err == QProcess::FailedToStart) {
        pluginName.clear();
        error = ProviderPluginProxy::PluginCrashed;

        emit q->finished();
        if (process) {
            process->deleteLater();
            process = NULL;
        }
    }

    qDebug() << "Error: " << err;
}

void ProviderPluginProxyPrivate::onFinished(int exitCode,
                                            QProcess::ExitStatus exitStatus)
{
    Q_Q(ProviderPluginProxy);
    Q_UNUSED(exitCode);

    pluginName.clear();

    if (exitStatus == QProcess::CrashExit) {
        error = ProviderPluginProxy::PluginCrashed;
        emit q->finished();
        process->deleteLater();
        process = NULL;
        return;
    }

    QString value;
    if (!pluginOutput.isEmpty()) {
        QDataStream stream(pluginOutput);
        stream >> createdAccountId;
    }

    if (process) {
        process->deleteLater();
        process = NULL;
    }

    emit q->finished();
}

ProviderPluginProxy::ProviderPluginProxy(QObject *parent):
    QObject(parent),
    d_ptr(new ProviderPluginProxyPrivate(this))
{
}

ProviderPluginProxy::~ProviderPluginProxy()
{
    Q_D(ProviderPluginProxy);
    delete d;
}

void ProviderPluginProxy::createAccount(Accounts::Provider *provider,
                                        const QString &serviceType)
{
    Q_D(ProviderPluginProxy);

    if (!provider) {
        qCritical() << " NULL pointer to provider";
        d->error = ProviderPluginProxy::PluginNotFound;
        emit finished();
        return;
    }

    d->startProcess(provider, 0, serviceType);
}

void ProviderPluginProxy::editAccount(Accounts::Account *account,
                                      const QString &serviceType)
{
    Q_D(ProviderPluginProxy);

    if (!account) {
        qCritical() << " NULL pointer to account";
        d->error = ProviderPluginProxy::AccountNotFound;
        emit finished();
        return;
    }

    Manager *manager = account->manager();
    Provider *provider = manager->provider(account->providerName());
    d->startProcess(provider, account->id(), serviceType);
}

void ProviderPluginProxy::setParentWidget(QWidget *parent)
{
    Q_D(ProviderPluginProxy);
    d->parentWidget = parent;
}

void ProviderPluginProxy::setPluginDirectories(const QStringList &pluginDirs)
{
    Q_D(ProviderPluginProxy);
    d->pluginDirs = pluginDirs;
}

QStringList ProviderPluginProxy::pluginDirectories() const
{
    Q_D(const ProviderPluginProxy);
    return d->pluginDirs;
}

bool ProviderPluginProxy::accountCreated() const
{
    Q_D(const ProviderPluginProxy);
    return d->createdAccountId != 0;
}

ProviderPluginProxy::Error ProviderPluginProxy::error() const
{
    Q_D(const ProviderPluginProxy);
    return d->error;
}

Accounts::AccountId ProviderPluginProxy::createdAccountId() const
{
    Q_D(const ProviderPluginProxy);
    return d->createdAccountId;
}

bool ProviderPluginProxy::isPluginRunning()
{
    Q_D(ProviderPluginProxy);
    return d->process != 0;
}

SetupType ProviderPluginProxy::setupType() const
{
    Q_D(const ProviderPluginProxy);
    return d->setupType;
}

QString ProviderPluginProxy::pluginName()
{
    if (!isPluginRunning())
        return QString();

    Q_D(ProviderPluginProxy);
    return d->pluginName;
}

QString ProviderPluginProxy::providerName()
{
    if (!isPluginRunning())
        return QString();
    Q_D(ProviderPluginProxy);
    return d->providerName;
}

void ProviderPluginProxy::setAdditionalParameters(const QStringList &parameters)
{
    Q_D(ProviderPluginProxy);
    d->additionalParameters = parameters;
}

QStringList ProviderPluginProxy::additionalParameters() const
{
    Q_D(const ProviderPluginProxy);
    return d->additionalParameters;
}

bool ProviderPluginProxy::killRunningPlugin()
{
    Q_D(ProviderPluginProxy);

    if (d->process == 0)
        return false;

    d->process->disconnect();
    d->process->close();
    delete d->process;
    d->process = 0;

    return true;
}

