#-------------------------------------------------
#
# Project created by QtCreator 2017-03-28T11:18:16
#
#-------------------------------------------------

#QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#TARGET = ModuleIO
#TEMPLATE = app


#SOURCES += main.cpp\
#        moduleio.cpp

#HEADERS  += moduleio.h

#FORMS    += moduleio.ui


#-------------------------------------------------
#
# Project created by QtCreator 2017-03-28T11:18:16
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
INCLUDEPATH +=../AbstractFuncWidget
#------------------------------------------------
CONFIG(debug, debug|release){
    TARGET = ModuleIOd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = ModuleIO
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------
DEFINES += MODULEIO_LIBRARY

SOURCES +=\
        moduleio.cpp \
    iowidget.cpp \
    ../../globaldef.cpp

HEADERS  += moduleio.h \
    moduleio_golbal.h \
    iowidget.h \
    ../../globaldef.h

FORMS    += moduleio.ui \
    iowidget.ui

RESOURCES += \
    images.qrc
