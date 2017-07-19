#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T14:58:46
#
#-------------------------------------------------

QT       += widgets xml designer

QT       -= gui

TEMPLATE = lib
#------------------------------------------------
include($$PWD/../FuncCommon.pri)
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = AdvFLASHd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = AdvFLASH
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------
DEFINES += ADVFLASH_LIBRARY

SOURCES += advflash.cpp \
    ../../XmlBuilder/xmlbuilder.cpp \
    ../../ServoControl/servocontrol.cpp \
    ../../globaldef.cpp

HEADERS += advflash.h\
        advflash_global.h \
    ../../XmlBuilder/xmlbuilder.h \
    ../../ServoControl/servocontrol.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    AdvFLASH.ui
