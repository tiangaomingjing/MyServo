#-------------------------------------------------
#
# Project created by QtCreator 2016-08-05T15:58:29
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)

QT       +=core gui widgets xml qml quick quickwidgets


TEMPLATE = lib

#------------------------------------------------
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = CfgDriverPowerd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = CfgDriverPower
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------

DEFINES += CFGDRIVERPOWER_LIBRARY

SOURCES += cfgdriverpower.cpp \
    ../../globaldef.cpp

HEADERS += cfgdriverpower.h\
        cfgdriverpower_global.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    CfgDriverPower.ui

DISTFILES += \
    CfgDriverPower.qml \
    components/CfgDriverPower/PowerEditBlock.qml
DEFINES+=   QT_NO_DEBUG\
            QT_NO_DEBUG_OUTPUT
