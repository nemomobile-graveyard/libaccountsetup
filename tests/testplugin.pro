include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TARGET = testplugin
TEMPLATE = app

CONFIG += \
    qt
SOURCES += \
    testplugin.cpp

QT += core xml

LIBS += -lAccountSetup
DEPENDPATH += $${INCLUDEPATH}
PKGCONFIG += \
    accounts-qt

target.path = /usr/lib/AccountSetup/
INSTALLS += target

