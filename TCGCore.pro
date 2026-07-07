QT += core sql
TEMPLATE = lib
CONFIG += shared c++14
DEFINES += TCGCORE_LIBRARY

SOURCES += \
    nexusdbmanager.cpp

HEADERS += \
    nexusdbmanager.h \
    tcgcore_global.h
