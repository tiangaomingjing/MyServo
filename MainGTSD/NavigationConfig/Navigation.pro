TEMPLATE = app

QT += qml quick widgets
CONFIG += c++11

CONFIG(debug, debug|release){
    OUTPATH=$${PWD}/../build/debug
} else{
    OUTPATH=$${PWD}/../build/release
}

DESTDIR =$${OUTPATH}

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    ConfigPages/ServoConfig.qml \
    ConfigPages/ConnectionsConfig.qml \
    ConfigPages/MotorConfig.qml \
    ConfigPages/PowerConfig.qml \
    ConfigPages/LimitConfig.qml \
    ConfigPages/EncoderConfig.qml \
    ConfigPages/MotionConfig.qml \
    ConfigPages/test.qml \
    ConfigPages/MyTreeViewStyle.qml \
    ConfigPages/MotorTreeViewStyle.qml \
    ConfigPages/PowerTreeViewStyle.qml \
    ConfigPages/FlowCommon/ChatDialog.qml \
    ConfigPages/FlowCommon/FilterBlock.qml \
    ConfigPages/FlowCommon/FlowNode.qml \
    ConfigPages/FlowCommon/HighlightComponent.qml \
    ConfigPages/FlowCommon/LineEditBlock.qml \
    ConfigPages/FlowCommon/PidControler.qml \
    ConfigPages/FlowCommon/Saturation.qml \
    ConfigPages/FlowCommon/SegmentArrow.qml \
    ConfigPages/CfgSpeedLoop/SpeedLimit.qml \
    ConfigPages/CfgPositionLoop/PositionLimit.qml \
    ConfigPages/Encoder/CircularGaugeDarkStyle.qml

