include(common-vars.pri)

TEMPLATE = subdirs

CONFIG += ordered
SUBDIRS += AccountSetup tests

include(doc/doc.pri)

include(common-installs-config.pri)

