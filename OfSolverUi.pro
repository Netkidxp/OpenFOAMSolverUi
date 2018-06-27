#-------------------------------------------------
#
# Project created by QtCreator 2018-06-14T07:42:39
#
#-------------------------------------------------

QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OfSolverUi
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
target.path = $$[QT_INSTALL_EXAMPLES]/charts/chartthemes

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    runner.cpp \
    foamrunner.cpp \
    decomposerunner.cpp \
    reconstrucerunner.cpp \
    foamdatapoint.cpp \
    foamlogdecoder.cpp \
    casecleaner.cpp

HEADERS += \
        mainwindow.h \
    runner.h \
    foamrunner.h \
    decomposerunner.h \
    reconstrucerunner.h \
    foamdatapoint.h \
    foamlogdecoder.h \
    casecleaner.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    icons.qrc
