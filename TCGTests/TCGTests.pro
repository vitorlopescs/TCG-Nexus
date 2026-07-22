QT += core gui widgets sql testlib
TEMPLATE = app
CONFIG += console warn_on testcase c++14

SOURCES += \
    test_storedash.cpp \
    test_login.cpp \
    test_devportal.cpp

# IMPORTANTE: storedashwindow.h precisa estar em HEADERS (não só em
# INCLUDEPATH) para que o moc gere o meta-objeto de StoreDashWindow
# (Q_OBJECT) também neste projeto. Sem isso o link falha com
# "undefined reference to vtable for StoreDashWindow".
HEADERS += \
    ../TCGGUI/storedashwindow.h \
    ../TCGGUI/devportalwindow.h \
    ../TCGGUI/logindialog.h \
    ../TCGGUI/recoverdialog.h

INCLUDEPATH += ../TCGGUI ../TCGCore
LIBS += -L../TCGCore -lTCGCore
