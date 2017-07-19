#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T14:42:17
#
#-------------------------------------------------

QT       += widgets xml

QT       -= gui

TEMPLATE = lib
#------------------------------------------------
include($$PWD/../FuncCommon.pri)
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = CfgIOd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = CfgIO
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------
DEFINES += CFGIO_LIBRARY

SOURCES += cfgio.cpp \
    ../../globaldef.cpp

HEADERS += cfgio.h\
        cfgio_global.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    CfgIO.ui
