#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T14:48:04
#
#-------------------------------------------------

QT       += widgets xml qml quick quickwidgets

QT       -= gui

TEMPLATE = lib
#------------------------------------------------
include($$PWD/../FuncCommon.pri)
INCLUDEPATH +=../AbstractFuncWidget\
              ../AbstractFuncWidget/QmlFactory/DeviceStatus

CONFIG(debug, debug|release){
    TARGET = DeviceStatusd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = DeviceStatus
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------

DEFINES += DEVICESTATUS_LIBRARY

SOURCES += devicestatus.cpp \
    ../../ServoControl/servocontrol.cpp \
    ../AbstractFuncWidget/QmlFactory/DeviceStatus/treedatadevicestatus.cpp \
    ../../globaldef.cpp

HEADERS += devicestatus.h\
        devicestatus_global.h \
    ../../ServoControl/servocontrol.h \
    ../AbstractFuncWidget/QmlFactory/DeviceStatus/treedatadevicestatus.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    DeviceStatus.ui

DISTFILES += \
    DeviceStatus.qml \
    LedIndicator.qml
#DEFINES+=   QT_NO_DEBUG\
#            QT_NO_DEBUG_OUTPUT
