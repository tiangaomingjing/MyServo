#-------------------------------------------------
#
# Project created by QtCreator 2017-03-13T19:53:14
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#------------------------------------------------
CONFIG(debug, debug|release){
    TARGET = ServoGeneralCmdd
} else{
    TARGET = ServoGeneralCmd
}
#---------------------------------------------------
TEMPLATE = lib

DEFINES += SERVOGENERALCMD_LIBRARY

SOURCES += servogeneralcmd.cpp

HEADERS += servogeneralcmd.h\
        servogeneralcmd_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
