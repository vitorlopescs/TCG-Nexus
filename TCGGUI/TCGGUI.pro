QT += core gui widgets sql
TEMPLATE = app
CONFIG += c++14

SOURCES += \
    main.cpp

HEADERS += \
    logindialog.h \
    devportalwindow.h \
    storedashwindow.h

INCLUDEPATH += ../TCGCore
LIBS += -L../TCGCore -lTCGCore
