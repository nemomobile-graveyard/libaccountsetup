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

#include "test.h"

#include <AccountSetup/ProviderPluginProxy>
#include <Accounts/Account>
#include <Accounts/Manager>
#include <QEventLoop>
#include <QSettings>
#include <QSignalSpy>
#include <QTimer>
#include <QtTest/QtTest>

using namespace Accounts;
using namespace AccountSetup;

class ProviderPluginProxyTest: public ProviderPluginProxy
{
public:
    ProviderPluginProxyTest(QObject *parent):
        ProviderPluginProxy(parent)
    {
    }

    void setDumpFile(const QString &dumpFile)
    {
        QStringList parameters;
        parameters << "--config-file" << dumpFile;
        setAdditionalParameters(parameters);
    }
};

void clearDb()
{
    QDir dbroot(QString(getenv("ACCOUNTS")));
    dbroot.remove(QString("accounts.db"));
}

void Test::initTestCase()
{
    //init
    setenv("ACCOUNTS", "/tmp/", TRUE);
    setenv("AG_PROVIDERS", PROVIDERS_DIR, TRUE);
    //clear database
    clearDb();
}

void Test::cleanupTestCase()
{

}

void Test::missingPluginTest()
{
    Manager *manager = new Manager();

    Provider *provider = manager->provider("SomePlugin");
    QVERIFY(provider == NULL);

    ProviderPluginProxy *proxy = new ProviderPluginProxy(manager);
    QSignalSpy spy(proxy, SIGNAL(finished()));

    /* Account creation should fail immediately, because no plugin can be
     * loaded. */
    proxy->createAccount(provider, QString());

    QCOMPARE(spy.count(), 1);
    QCOMPARE(proxy->error(), ProviderPluginProxy::PluginNotFound);

    delete manager;
}

void Test::onFinished()
{
    ProviderPluginProxy *proxy = qobject_cast<ProviderPluginProxy*>(sender());
    qDebug() << "Error:" << proxy->error();
    finishedEmitted = true;
}

void Test::pluginStatusTest()
{
    Manager *manager = new Manager();

    Provider *provider = manager->provider("NutProvider");
    QVERIFY(provider != NULL);

    ProviderPluginProxyTest *proxy = new ProviderPluginProxyTest(manager);
    QObject::connect(proxy, SIGNAL(finished()),
                     this, SLOT(onFinished()));

    QEventLoop loop;
    QObject::connect(proxy, SIGNAL(finished()), &loop, SLOT(quit()));
    finishedEmitted = false;

    const QString dumpFile("/tmp/testplugin.dump");
    proxy->setDumpFile(dumpFile);

    const QString serviceType("AnyServiceType");
    proxy->createAccount(provider, serviceType);
    QVERIFY(!finishedEmitted);
    QVERIFY(proxy->isPluginRunning());

    QTimer::singleShot(10*1000, &loop, SLOT(quit()));
    loop.exec();
    QVERIFY(finishedEmitted);
    QVERIFY(!proxy->isPluginRunning());

    /* check the status dump */
    QSettings status(dumpFile);
    QCOMPARE(status.value("ping").toString(), QString("pong"));
    QCOMPARE(status.value("AccountId").toInt(), 0);
    QCOMPARE(status.value("SetupType").toInt(), (int)CreateNew);
    QCOMPARE(status.value("ServiceType").toString(), serviceType);

    QCOMPARE(proxy->error(), ProviderPluginProxy::NoError);

    delete manager;
}

QTEST_MAIN(Test)

