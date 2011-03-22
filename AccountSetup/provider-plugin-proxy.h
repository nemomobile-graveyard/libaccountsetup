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

#ifndef ACCOUNTSETUP_PROVIDER_PLUGIN_PROXY_H
#define ACCOUNTSETUP_PROVIDER_PLUGIN_PROXY_H

// libAccountSetup
#include <AccountSetup/common.h>
#include <AccountSetup/types.h>

// Accounts
#include <Accounts/Account>
#include <Accounts/Provider>

// Qt
#include <QObject>
#include <QStringList>

class QWidget;

namespace AccountSetup {

class ProviderPluginProxyPrivate;

/*!
 * @class ProviderPluginProxy
 * @headerfile AccountSetup/provider-plugin-proxy.h \
 * AccountSetup/ProviderPluginProxy
 * @brief Client class for accounts UI plugins.
 *
 * @details The ProviderPluginProxy class can be used to run the account
 * plugins. Plugins can be started with the createAccount() and editAccount()
 * methods, respectively to enter the account creation and editing modes.
 * Plugin lifetime can be monitored with the created(), edited(), cancelled()
 * signals, or inspected with the isPluginRunning() method.
 */
class ACCOUNTSETUP_EXPORT ProviderPluginProxy: public QObject
{
    Q_OBJECT

public:
    /*!
     * Error codes for plugin execution.
     * @sa error()
     */
    enum Error {
        NoError = 0,
        AccountNotFound,
        PluginNotFound,
        PluginCrashed,
    };

    /*!
     * Constructor
     */
    ProviderPluginProxy(QObject *parent = 0);
    virtual ~ProviderPluginProxy();

    /*!
     * Runs the account plugin to create an account.
     *
     * @param provider The Accounts::Provider for the account to be created.
     * @param serviceType The main service type the user is interested in, or
     * empty string.
     */
    void createAccount(Accounts::Provider *provider,
                       const QString &serviceType);

    /*!
     * Runs the account plugin to edit an account.
     *
     * @param account The Accounts::Account to be edited.
     * @param serviceType The main service type the user is interested in, or
     * empty string.
     */
    void editAccount(Accounts::Account *account, const QString &serviceType);

    /*!
     * Attempt to set the next executed account plugin modal to a given widget.
     * @param parent The widget (window) the account plugin should be modal
     * to.
     */
    void setParentWidget(QWidget *parent);

    /*!
     * Set the list of directories which will be searched for provider
     * plugins.
     * @param pluginDirs List of directory names.
     */
    void setPluginDirectories(const QStringList &pluginDirs);

    /*!
     * Get the list of directories which will be searched for provider
     * plugins.
     */
    QStringList pluginDirectories() const;

    /*!
     * Checks whether an account was created by the plugin executed last.
     * @note This method should be called only after the finished() signal has
     * been emitted, and before the next execution of an account plugin.
     *
     * @return Whether an account has been successfully created.
     */
    bool accountCreated() const;

    /*!
     * Gets the error code of the last plugin execution.
     * @note This method should be called only after the finished() signal has
     * been emitted, and before the next execution of an account plugin.
     */
    Error error() const;

    /*!
     * Gets the ID of the newly created account.
     * Call accountCreated() first, to know whether an account was
     * actually created.
     * @return The account ID of the created account.
     */
    Accounts::AccountId createdAccountId() const;

    /*!
     * Checks whether a plugin is running.
     *
     * @return Returns true if there is plugin process running.
     */
    bool isPluginRunning();

    /*!
     * Returns the operation being performed by the plugin.
     *
     * @return CreateNew is the plugin was invoked to create a new account,
     * EditExisting if it's edit an existing account.
     */
    SetupType setupType() const;

    /*!
     * @return the name of the currently running plugin, or empty string if no
     * plugin is running.
     */
    QString pluginName();

    /*!
     * @return the name of the povider of the plugin
     */
    QString providerName();

Q_SIGNALS:
    /*!
     * Emitted when the plugin execution has been completed.
     * @sa error(), accountCreated()
     */
    void finished();

protected:
    /*!
     * Sets additional parameters to be passed to the plugin process on the
     * next invocation of createAccount() or editAccount().
     *
     * @param parameters The additional parameters to be passed to the plugin.
     * In the current implementation these parameters are passed as process
     * arguments.
     */
    void setAdditionalParameters(const QStringList &parameters);

    /*!
     * Gets the list of additional parameters passed to the plugin process.
     */
    QStringList additionalParameters() const;

    /*!
     * Kills the plugin being executed. This will probably result in data loss
     * and other resource waste, so it's strongly recommended not to ever call
     * this method.
     * @return Returns true is the process was terminated, false otherwise.
     */
    bool killRunningPlugin();

private:
    ProviderPluginProxyPrivate *d_ptr;
    Q_DECLARE_PRIVATE(ProviderPluginProxy)
};

} // namespace

#endif // ACCOUNTSETUP_PROVIDER_PLUGIN_PROXY_H

