TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH +=$${PWD}/V124/ToolBox\
                  $${PWD}/V124\
                  $${PWD}/V124/EncoderItem

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    V124/CfgEncoder.qml \
    V124/components/CfgEncoder/gaugebackground.png \
    V124/components/CfgEncoder/gaugecenter.png \
    V124/components/CfgEncoder/gaugeneedle.png \
    V124/components/CfgEncoder/CircularGaugeDarkStyle.qml \
    V124/ToolBox/ToolItem.qml \
    V124/ToolBox/AbsoluteEncoder.qml \
    V124/ToolBox/ToolBox.qml \
    V124/ToolBox/IncrementalEncoder.qml \
    V124/ToolBox/NoneEncoder.qml \
    V124/EncoderItem/AbstractEncoderItem.qml \
    V124/EncoderItem/DuoMoEncoderItem.qml \
    V124/EncoderItem/NiKangEncoderItem.qml \
    V124/EncoderItem/HaiDeHanEncoderItem.qml \
    V124/EncoderItem/SanXieEncoderItem.qml \
    V124/EncoderItem/SongXiaEncoderItem.qml \
    V124/EncoderItem/testEncoder.qml \
    V124/RollWheel.qml

