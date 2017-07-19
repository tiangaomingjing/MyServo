#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T11:59:25
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)

QT       += xml widgets qml quick quickwidgets

QT       -= gui


TEMPLATE = lib
#------------------------------------------------
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = CfgCurrentLoopd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = CfgCurrentLoop
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------

DEFINES += CFGCURRENTLOOP_LIBRARY

SOURCES += cfgcurrentloop.cpp \
    ../../globaldef.cpp

HEADERS += cfgcurrentloop.h\
        cfgcurrentloop_global.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    CfgCurrentLoop.ui

DISTFILES += \
    CfgCurrentLoop.qml

