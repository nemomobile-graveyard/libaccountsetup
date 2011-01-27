include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

QMAKE_EXTRA_TARGETS += check
check.depends = libaccountsetup-test
check.commands = ./libaccountsetup-test

TARGET = libaccountsetup-test

CONFIG += \
    qtestlib \
    qt
SOURCES += \
    test.cpp
HEADERS += \
    test.h

QT += core xml

LIBS += -lAccountSetup
DEPENDPATH += $${INCLUDEPATH}
PKGCONFIG += \
    accounts-qt

include($${TOP_SRC_DIR}/common-installs-config.pri)

DATA_PATH = $${INSTALL_PREFIX}/share/libaccountsetup-tests/

DEFINES += \
    PROVIDERS_DIR=\\\"$$DATA_PATH\\\"

provider.path = $$DATA_PATH
provider.files += \
    MissingPlugin.provider \
    NutProvider.provider
INSTALLS += provider

testsuite.path = $$DATA_PATH
testsuite.files = tests.xml
INSTALLS += testsuite
