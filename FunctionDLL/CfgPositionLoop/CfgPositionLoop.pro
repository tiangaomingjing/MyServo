#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T14:18:58
#
#-------------------------------------------------

QT       += widgets xml qml quick quickwidgets

QT       -= gui


TEMPLATE = lib
#------------------------------------------------
include($$PWD/../FuncCommon.pri)
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = CfgPositionLoopd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = CfgPositionLoop
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------

DEFINES += CFGPOSITIONLOOP_LIBRARY

SOURCES += cfgpositionloop.cpp \
    ../../globaldef.cpp

HEADERS += cfgpositionloop.h\
        cfgpositionloop_global.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    CfgPositionLoop.ui

DISTFILES += \
    CfgPositionLoop.qml \
    components/CfgPositionLoop/PositionLimit.qml \
    components/FlowCommon/ChatDialog.qml \
    components/FlowCommon/FilterBlock.qml \
    components/FlowCommon/FlowNode.qml \
    components/FlowCommon/HighlightComponent.qml \
    components/FlowCommon/LineEditBlock.qml \
    components/FlowCommon/PidControler.qml \
    components/FlowCommon/Saturation.qml \
    components/FlowCommon/SegmentArrow.qml
