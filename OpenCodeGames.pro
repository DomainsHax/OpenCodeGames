#-------------------------------------------------
#
# Project created by QtCreator 2016-10-25T21:54:41
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenCodeGames
TEMPLATE = app

SOURCES += main.cpp\
        opencodegames.cpp\
        filedownloader.cpp

HEADERS  += opencodegames.h\
         filedownloader.h

CONFIG   += c++14

FORMS    += opencodegames.ui

RESOURCES += images.qrc
