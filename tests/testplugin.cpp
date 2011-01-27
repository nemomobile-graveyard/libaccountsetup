/*
 * This file is part of libAccountSetup
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

#include <AccountSetup/ProviderPluginProcess>
#include <Accounts/Account>
#include <Accounts/Manager>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>

using namespace Accounts;
using namespace AccountSetup;

int main(int argc, char **argv)
{
    QCoreApplication *app = new QCoreApplication(argc, argv);

    ProviderPluginProcess *plugin = new ProviderPluginProcess();

    /* test basic functionality */
    if (plugin != ProviderPluginProcess::instance()) {
        qFatal("ProviderPluginProcess::instance returned wrong instance");
    }

    /* open a QSettings file as specified by the parent process */
    QStringList args = QCoreApplication::arguments();
    int argIndex = args.indexOf("--config-file");
    if (argIndex > 0 && argIndex + 1 < args.length()) {
        QSettings status(args[argIndex + 1]);

        /* Dump the current status into the QSettings file */
        status.setValue("ping", QString("pong"));
        status.setValue("AccountId", plugin->account()->id());
        status.setValue("SetupType", plugin->setupType());
        status.setValue("ServiceType", plugin->serviceType());
        status.setValue("ParentWindowId",
                        QVariant::fromValue<uint>(plugin->parentWindowId()));
    }

    plugin->quit();
    delete plugin;
}

