#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T14:27:23
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)
QT       += widgets xml

QT       -= gui

TEMPLATE = lib
#------------------------------------------------
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = CfgLimitd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = CfgLimit
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------

DEFINES += CFGLIMIT_LIBRARY

SOURCES += cfglimit.cpp \
    ../../globaldef.cpp

HEADERS += cfglimit.h\
        cfglimit_global.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    CfgLimit.ui
