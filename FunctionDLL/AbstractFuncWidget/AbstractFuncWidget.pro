#-------------------------------------------------
#
# Project created by QtCreator 2016-08-05T15:13:34
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)

QT       += core widgets xml designer qml

TEMPLATE = lib
#------------------------------------------------

CONFIG(debug, debug|release){
    TARGET = AbstractFuncWidgetd
    LIBS +=
} else{
    TARGET = AbstractFuncWidget
    LIBS +=
}

#---------------------------------------------------
DEFINES += ABSTRACTFUNCWIDGET_LIBRARY

SOURCES += abstractfuncwidget.cpp \
    ../../globaldef.cpp \
    ../../XmlBuilder/xmlbuilder.cpp \
    ../../ServoControl/servocontrol.cpp \
    QmlFactory/iconlabel.cpp \
    QmlFactory/led.cpp \
    QmlFactory/person.cpp \
    QmlFactory/qhboxlayoutproxy.cpp \
    QmlFactory/qmlfactory.cpp \
    QmlFactory/qvboxlayoutproxy.cpp \
    QmlFactory/qmlregisterincludes.cpp \
    QmlFactory/qformlayoutproxy.cpp \
    QmlFactory/qspaceritemproxy.cpp \
    QmlFactory/DeviceStatus/treedatadevicestatus.cpp \
    QmlFactory/qtreewidgetproxy.cpp

HEADERS += abstractfuncwidget.h\
        abstractfuncwidget_global.h \
    ../../globaldef.h \
    ../../XmlBuilder/xmlbuilder.h \
    ../../ServoControl/servocontrol.h \
    QmlFactory/iconlabel.h \
    QmlFactory/led.h \
    QmlFactory/person.h \
    QmlFactory/qhboxlayoutproxy.h \
    QmlFactory/qmlfactory.h \
    QmlFactory/qmlregisterincludes.h \
    QmlFactory/qvboxlayoutproxy.h \
    QmlFactory/qformlayoutproxy.h \
    QmlFactory/qspaceritemproxy.h \
    QmlFactory/DeviceStatus/treedatadevicestatus.h \
    QmlFactory/qtreewidgetproxy.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
#DEFINES+=   QT_NO_DEBUG\
#            QT_NO_DEBUG_OUTPUT
