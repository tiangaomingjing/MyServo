#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T14:54:07
#
#-------------------------------------------------

QT       += widgets xml designer

QT       -= gui

TEMPLATE = lib
#------------------------------------------------
include($$PWD/../FuncCommon.pri)
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = AdvRAMd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = AdvRAM
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------

DEFINES += ADVRAM_LIBRARY

SOURCES += advram.cpp \
    ../../XmlBuilder/xmlbuilder.cpp \
    ../../globaldef.cpp \
    ../../ServoControl/servocontrol.cpp

HEADERS += advram.h\
        advram_global.h \
    ../../XmlBuilder/xmlbuilder.h \
    ../../globaldef.h \
    ../../ServoControl/servocontrol.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    AdvRAM.ui
