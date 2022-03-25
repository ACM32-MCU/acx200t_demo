#-------------------------------------------------
#
# Project created by QtCreator 2022-01-31T14:31:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = acx200t-qt-demo

TEMPLATE = app

SOURCES += main.cpp\
    widget.cpp \
    sewidget.cpp \
    imagepressbutton.cpp \
    dashboard.cpp \
    imagebutton.cpp

HEADERS  += \
    widget.h \
    imagepressbutton.h \
    dashboard.h \
    imagebutton.h \
    sewidget.h

FORMS    += \
    imagebutton.ui \
    fromanimationbutton.ui

RESOURCES += \
    rc.qrc

LIBS += -L$$PWD/./ -lacx200t -Wl,-rpath,./

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
