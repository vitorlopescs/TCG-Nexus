QT += core gui widgets sql testlib
TEMPLATE = app
CONFIG += console warn_on testcase c++14

SOURCES += \
    main.cpp \
    test_storedash.cpp \
    test_login.cpp \
    test_devportal.cpp

HEADERS += \
    ../TCGGUI/storedashwindow.h \
    ../TCGGUI/devportalwindow.h \
    ../TCGGUI/logindialog.h \
    ../TCGGUI/recoverdialog.h \
    ../TCGGUI/registerdialog.h

INCLUDEPATH += ../TCGGUI ../TCGCore
LIBS += -L../TCGCore -lTCGCore