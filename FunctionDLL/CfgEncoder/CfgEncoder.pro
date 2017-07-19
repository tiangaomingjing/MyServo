#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T11:33:00
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)
QT       += widgets xml qml quick quickwidgets

QT       -= gui


TEMPLATE = lib
#------------------------------------------------
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = CfgEncoderd
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = CfgEncoder
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------

DEFINES += CFGENCODER_LIBRARY

SOURCES += cfgencoder.cpp \
    ../../globaldef.cpp

HEADERS += cfgencoder.h\
        cfgencoder_global.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    CfgEncoder.ui

DISTFILES += \
    V123/CfgEncoder.qml \
    V123/components/CfgEncoder/CircularGaugeDarkStyle.qml \
    V122/CfgEncoder.qml \
    V122/components/CfgEncoder/CircularGaugeDarkStyle.qml \
    V124/CfgEncoder.qml \
    V124/components/CfgEncoder/gaugebackground.png \
    V124/components/CfgEncoder/gaugecenter.png \
    V124/components/CfgEncoder/gaugeneedle.png \
    V124/components/CfgEncoder/CircularGaugeDarkStyle.qml \
    V124/components/CfgEncoder/RollWheel.qml \
    V124/components/CfgEncoder/ToolBox/AbsoluteEncoder.qml \
    V124/components/CfgEncoder/ToolBox/IncrementalEncoder.qml \
    V124/components/CfgEncoder/ToolBox/NoneEncoder.qml \
    V124/components/CfgEncoder/ToolBox/ToolBox.qml \
    V124/components/CfgEncoder/ToolBox/ToolItem.qml \
    V124/components/CfgEncoder/EncoderItem/AbstractEncoderItem.qml \
    V124/components/CfgEncoder/EncoderItem/DuoMoEncoderItem.qml \
    V124/components/CfgEncoder/EncoderItem/HaiDeHanEncoderItem.qml \
    V124/components/CfgEncoder/EncoderItem/NiKangEncoderItem.qml \
    V124/components/CfgEncoder/EncoderItem/SanXieEncoderItem.qml \
    V124/components/CfgEncoder/EncoderItem/SongXiaEncoderItem.qml
