#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T13:59:51
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)
QT       += widgets xml qml quick quickwidgets

QT       -= gui

TEMPLATE = lib
#------------------------------------------------
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = CfgSpeedLoopd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = CfgSpeedLoop
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------
DEFINES += CFGSPEEDLOOP_LIBRARY

SOURCES += cfgspeedloop.cpp \
    ../../globaldef.cpp

HEADERS += cfgspeedloop.h\
        cfgspeedloop_global.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    CfgSpeedLoop.ui

DISTFILES += \
    CfgSpeedLoop.qml \
    components/CfgSpeedLoop/SpeedLimit.qml \
    components/FlowCommon/ChatDialog.qml \
    components/FlowCommon/FilterBlock.qml \
    components/FlowCommon/FlowNode.qml \
    components/FlowCommon/HighlightComponent.qml \
    components/FlowCommon/LineEditBlock.qml \
    components/FlowCommon/PidControler.qml \
    components/FlowCommon/Saturation.qml \
    components/FlowCommon/SegmentArrow.qml
