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
/*!
 * @copyright Copyright (C) 2011 Nokia Corporation.
 * @license LGPL
 */

#ifndef ACCOUNTSETUP_PROVIDER_PLUGIN_PROCESS_H
#define ACCOUNTSETUP_PROVIDER_PLUGIN_PROCESS_H

// libAccountSetup
#include <AccountSetup/common.h>
#include <AccountSetup/types.h>

// Accounts
#include <Accounts/Account>

// Qt
#include <QObject>
#include <QWidget>

namespace AccountSetup {
class ProviderPluginProcessPrivate;

/*!
 * @class ProviderPluginProcess
 * @headerfile AccountSetup/provider-plugin-process.h \
 * AccountSetup/ProviderPluginProcess
 * @brief Class to manage the account provider plugins as separate processes.
 *
 * @details The ProviderPluginProcess class takes care of handling the
 * communication protocol between the client application and the account
 * plugin process.
 */
class ACCOUNTSETUP_EXPORT ProviderPluginProcess: public QObject
{
    Q_OBJECT

public:
    /*!
     * Constructs the account provider plugin process.
     */
    ProviderPluginProcess(QObject *object = 0);
    virtual ~ProviderPluginProcess();

    /*!
     * Get the instance of the object.
     */
    static ProviderPluginProcess *instance();

    /*!
     * @return The type of operation this plugin is expected to perform.
     */
    SetupType setupType() const;

    /*!
     * Gets the account being setup by this plugin.
     * @note The returned object might not refer to an account stored on the
     * accounts DB, if the task of this plugin is to create a new account.
     */
    Accounts::Account *account() const;

    /*!
     * @return The service type.
     */
    QString serviceType() const;

    /*!
     * Gets the platform specific identifier of the window to which this
     * plugin UI should be modal.
     */
    WId parentWindowId() const;

    /*!
     * Sets the variable to go to accountsPage, when plugin exits.
     */
    void setReturnToAccountsList(bool value);

    /*!
     * sets the exit data.
     */
    void setExitData(const QVariant &data);

public Q_SLOTS:
    /*!
     * Clean termination of the plugin process.
     */
    void quit();

private:
    ProviderPluginProcessPrivate *d_ptr;
    Q_DECLARE_PRIVATE(ProviderPluginProcess)
};

} // namespace

#endif // ACCOUNTSETUP_PROVIDER_PLUGIN_PROCESS_H
