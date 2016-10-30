#-------------------------------------------------
#
# Project created by QtCreator 2016-10-30T18:10:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CryptoppUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

CONFIG(debug, debug|release) {
    LIBS+= -L../CryptoppUI/cryptopp_builded/dbg -lcryptopp
}

CONFIG(release, debug|release) {
    LIBS+= -L../CryptoppUI/cryptopp_builded/rls -lcryptopp
}
