#-------------------------------------------------
#
# Project created by QtCreator 2016-08-08T15:30:55
#
#-------------------------------------------------

QT       += core gui designer qml quick sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TEMPLATE = app

INCLUDEPATH +=$${PWD}/../ServoControl
INCLUDEPATH +=$${PWD}/../XmlBuilder
INCLUDEPATH +=$${PWD}/../NetCmd/ServoDriverComDll
INCLUDEPATH +=$${PWD}/../NetCmd/ServoDriverComDll/NetCom/include
INCLUDEPATH +=$${PWD}/..\
                $${PWD}/../FunctionDLL/AbstractFuncWidget\
                $${PWD}/../FunctionDLL/AdvFLASH\
                $${PWD}/../FunctionDLL/AdvRAM\
                $${PWD}/../FunctionDLL/CfgCurrentLoop\
                $${PWD}/../FunctionDLL/CfgDriverPower\
                $${PWD}/../FunctionDLL/CfgEncoder\
                $${PWD}/../FunctionDLL/CfgIO\
                $${PWD}/../FunctionDLL/CfgLimit\
                $${PWD}/../FunctionDLL/CfgMotor\
                $${PWD}/../FunctionDLL/CfgMoveProfile\
                $${PWD}/../FunctionDLL/CfgPositionLoop\
                $${PWD}/../FunctionDLL/CfgSpeedLoop\
                $${PWD}/../FunctionDLL/DeviceStatus\
                $${PWD}/../FunctionDLL/PlotWave\
                $${PWD}/../FunctionDLL/ModuleIO


CONFIG(debug, debug|release){
    TARGET = SDT_d
    LIB_PATH=$${PWD}/../../debug/Bin
    OUT_ROOT = $${PWD}/../../debug
    LIBS += $${LIB_PATH}/AbstractFuncWidgetd.lib\
            $${LIB_PATH}/AdvFLASHd.lib\
            $${LIB_PATH}/AdvRAMd.lib\
            $${LIB_PATH}/CfgCurrentLoopd.lib\
            $${LIB_PATH}/CfgDriverPowerd.lib\
            $${LIB_PATH}/CfgEncoderd.lib\
            $${LIB_PATH}/CfgIOd.lib\
            $${LIB_PATH}/CfgLimitd.lib\
            $${LIB_PATH}/CfgMotord.lib\
            $${LIB_PATH}/CfgMoveProfiled.lib\
            $${LIB_PATH}/CfgPositionLoopd.lib\
            $${LIB_PATH}/CfgSpeedLoopd.lib\
            $${LIB_PATH}/DeviceStatusd.lib\
            $${LIB_PATH}/PlotWaved.lib\
            $${LIB_PATH}/ServoDriverComDlld.lib\
            $${LIB_PATH}/ModuleIOd.lib

} else{
    TARGET = SDT
    LIB_PATH=$${PWD}/../../release/Bin
    OUT_ROOT = $${PWD}/../../release
    LIBS += $${LIB_PATH}/AbstractFuncWidget.lib\
            $${LIB_PATH}/AdvFLASH.lib\
            $${LIB_PATH}/AdvRAM.lib\
            $${LIB_PATH}/CfgCurrentLoop.lib\
            $${LIB_PATH}/CfgDriverPower.lib\
            $${LIB_PATH}/CfgEncoder.lib\
            $${LIB_PATH}/CfgIO.lib\
            $${LIB_PATH}/CfgLimit.lib\
            $${LIB_PATH}/CfgMotor.lib\
            $${LIB_PATH}/CfgMoveProfile.lib\
            $${LIB_PATH}/CfgPositionLoop.lib\
            $${LIB_PATH}/CfgSpeedLoop.lib\
            $${LIB_PATH}/DeviceStatus.lib\
            $${LIB_PATH}/PlotWave.lib\
            $${LIB_PATH}/ServoDriverComDll.lib\
            $${LIB_PATH}/ModuleIO.lib
}

DESTDIR =$${OUT_ROOT}/Bin

SOURCES += main.cpp\
        mainwindow.cpp \
    Factory/factory.cpp \
    NewConfigDialog/newconfigdialog.cpp \
    FlashUpdate/flashupdatewidget.cpp \
    ../globaldef.cpp \
    ../XmlBuilder/xmlbuilder.cpp \
    MAction/maction.cpp \
    ../ServoControl/servocontrol.cpp \
    AxisClone/axisclone.cpp \
    AxisClone/axisfileclone.cpp \
    AxisClone/arrow.cpp \
    AxisClone/axisbackground.cpp \
    AxisClone/axisitem.cpp \
    DownloadProgram/downloaddialog.cpp \
    NormalizeTree/normalizetreeform.cpp \
    FpgaDialogSetting/fpgadialogsetting.cpp \
    FpgaDialogSetting/inputvalidator.cpp \
    NavigationConfig/UserConfigProxyClass/qmluserconfigproxy.cpp \
    NavigationConfig/DataBase/ConnectDataBase/connectdatabase.cpp \
    NavigationConfig/DataBase/SqltableModel/sqltablemodel.cpp \
    Uboot/netconfig.cpp \
    Uboot/ubootdialog.cpp \
    NavigationConfig/ControlName/controlname.cpp \
    FpgaDialogSetting/fpgadialogsettingrnnet.cpp


HEADERS  += mainwindow.h \
    Factory/factory.h \
    functioninc.h \
    NewConfigDialog/newconfigdialog.h \
    FlashUpdate/flashupdatewidget.h \
    ../globaldef.h \
    ../XmlBuilder/xmlbuilder.h \
    MAction/maction.h \
    ../ServoControl/servocontrol.h \
    AxisClone/axisclone.h \
    AxisClone/axisfileclone.h \
    AxisClone/arrow.h \
    AxisClone/axisbackground.h \
    AxisClone/axisitem.h \
    DownloadProgram/downloaddialog.h \
    NormalizeTree/normalizetreeform.h \
    FpgaDialogSetting/fpgadialogsetting.h \
    FpgaDialogSetting/inputvalidator.h \
    NavigationConfig/UserConfigProxyClass/qmluserconfigproxy.h \
    NavigationConfig/DataBase/ConnectDataBase/connectdatabase.h \
    NavigationConfig/DataBase/SqltableModel/sqltablemodel.h \
    Uboot/netconfig.h \
    Uboot/ubootdialog.h \
    NavigationConfig/ControlName/controlname.h \
    FpgaDialogSetting/fpgadialogsettingrnnet.h \
    FpgaDialogSetting/highlighttext.h

FORMS    += mainwindow.ui \
    NewConfigDialog/NewConfigDialog.ui \
    FlashUpdate/flashupdatewidget.ui \
    status.ui \
    AxisClone/axisclone.ui \
    AxisClone/AxisFileClone.ui \
    DownloadProgram/downloaddialog.ui \
    NormalizeTree/normalizetreeform.ui \
    FpgaDialogSetting/fpgadialogsetting.ui \
    Uboot/ubootdialog.ui \
    FpgaDialogSetting/fpgadialogsettingrnnet.ui

RESOURCES += \
    image.qrc
RC_ICONS +=AppIco.ico

TRANSLATIONS +=app_main_simplchinese.ts

#DEFINES+=   QT_NO_DEBUG\
#            QT_NO_DEBUG_OUTPUT

DISTFILES += \
    NavigationConfig/main.qml \
    NavigationConfig/ConfigPages/ConnectionsConfig.qml \
    NavigationConfig/ConfigPages/EncoderConfig.qml \
    NavigationConfig/ConfigPages/LimitConfig.qml \
    NavigationConfig/ConfigPages/MotionConfig.qml \
    NavigationConfig/ConfigPages/MotorConfig.qml \
    NavigationConfig/ConfigPages/MotorTreeViewStyle.qml \
    NavigationConfig/ConfigPages/MyTreeViewStyle.qml \
    NavigationConfig/ConfigPages/PowerConfig.qml \
    NavigationConfig/ConfigPages/PowerTreeViewStyle.qml \
    NavigationConfig/ConfigPages/ServoConfig.qml \
    NavigationConfig/ConfigPages/FlowCommon/ChatDialog.qml \
    NavigationConfig/ConfigPages/FlowCommon/FilterBlock.qml \
    NavigationConfig/ConfigPages/FlowCommon/FlowNode.qml \
    NavigationConfig/ConfigPages/FlowCommon/HighlightComponent.qml \
    NavigationConfig/ConfigPages/FlowCommon/LineEditBlock.qml \
    NavigationConfig/ConfigPages/FlowCommon/PidControler.qml \
    NavigationConfig/ConfigPages/FlowCommon/Saturation.qml \
    NavigationConfig/ConfigPages/FlowCommon/SegmentArrow.qml \
    NavigationConfig/ConfigPages/Encoder/CircularGaugeDarkStyle.qml \
    NavigationConfig/ConfigPages/CfgPositionLoop/PositionLimit.qml \
    NavigationConfig/ConfigPages/CfgSpeedLoop/SpeedLimit.qml \
    NavigationConfig/ConfigPages/servoConfigJs.js \
    NavigationConfig/mainJs.js \
    NavigationConfig/ConfigPages/test.qml \
    NavigationConfig/ConfigPages/ControlView.qml

#require administator
QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

