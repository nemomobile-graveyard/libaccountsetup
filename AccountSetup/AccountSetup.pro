include(../common-project-config.pri)
include(../common-vars.pri)

# -----------------------------------------------------------------------------
# target setup
# -----------------------------------------------------------------------------
TEMPLATE = lib
TARGET = AccountSetup
VERSION = 1.0.0
CONFIG += \
    qt \
    link_pkgconfig
QT += \
    xml \
    network

# -----------------------------------------------------------------------------
# dependencies
# -----------------------------------------------------------------------------
PKGCONFIG += \
    accounts-qt

# -----------------------------------------------------------------------------
# input
# -----------------------------------------------------------------------------
HEADERS += \
    provider-plugin-process.h \
    provider-plugin-process-priv.h \
    provider-plugin-proxy.h \
    provider-plugin-proxy-priv.h

SOURCES += \
    provider-plugin-process.cpp \
    provider-plugin-proxy.cpp

# headers are the files which will be installed with "make install"
headers.files += \
    common.h \
    ProviderPluginProcess \
    provider-plugin-process.h \
    ProviderPluginProxy \
    provider-plugin-proxy.h \
    types.h

# -----------------------------------------------------------------------------
# common installation setup
# NOTE: remember to set headers.files before this include to have the headers
# properly setup.
# -----------------------------------------------------------------------------
include(../common-installs-config.pri)

# -----------------------------------------------------------------------------
# Installation target for application resources
# -----------------------------------------------------------------------------
pkgconfig.files = AccountSetup.pc
pkgconfig.path = $${INSTALL_PREFIX}/lib/pkgconfig
INSTALLS += \
    pkgconfig
