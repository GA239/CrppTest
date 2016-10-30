#-------------------------------------------------
#
# Project created by QtCreator 2016-10-04T12:03:08
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = CrypToPPTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    support.cpp
LIBS += -lcryptopp

HEADERS += \
    support.h
