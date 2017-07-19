#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T09:51:37
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)

QT       += widgets xml qml quick quickwidgets

QT       -= gui

TEMPLATE = lib
#------------------------------------------------
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = CfgMotord
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = CfgMotor
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------

DEFINES += CFGMOTOR_LIBRARY

SOURCES += cfgmotor.cpp \
    ../../globaldef.cpp

HEADERS += cfgmotor.h\
        cfgmotor_global.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    CfgMotor.ui

DISTFILES += \
    CfgMotor.qml \
    components/CfgMotor/Current.qml \
    components/CfgMotor/ForceParameter.qml \
    components/CfgMotor/Impedance.qml \
    components/CfgMotor/MechanicalParameter.qml \
    components/CfgMotor/Velocity.qml
