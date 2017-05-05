#-------------------------------------------------
#
# Project created by QtCreator 2017-05-05T18:00:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Demo
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

include(../de_skycoder42_qrng.pri)

SOURCES += main.cpp\
		widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui
