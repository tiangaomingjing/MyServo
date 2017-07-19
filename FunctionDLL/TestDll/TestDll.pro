#-------------------------------------------------
#
# Project created by QtCreator 2016-08-06T00:04:16
#
#-------------------------------------------------
include($$PWD/../FuncCommon.pri)

QT       += core gui designer qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
INCLUDEPATH+=../CfgDriverPower\
             ../AbstractFuncWidget\
             ../CfgMotor\
             ../CfgEncoder

TEMPLATE = app

CONFIG(debug, debug|release){
    TARGET = TestDlld
    LIBS += $${FUNC_LIB_ROOT}/CfgDriverPowerd.lib\
            $${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib\
            $${FUNC_LIB_ROOT}/CfgMotord.lib\
            $${FUNC_LIB_ROOT}/CfgEncoderd.lib
} else{
    TARGET = TestDll
    LIBS += $${FUNC_LIB_ROOT}/CfgDriverPower.lib\
            $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib\
            $${FUNC_LIB_ROOT}/CfgMotor.lib\
            $${FUNC_LIB_ROOT}/CfgEncoder.lib
}

SOURCES += main.cpp\
        mainwindow.cpp \
    factory/factory.cpp \
    ../../XmlBuilder/xmlbuilder.cpp \
    tree.cpp \
    mobj.cpp \
    ../../MainGTSD/FpgaDialogSetting/inputvalidator.cpp \
    ../../globaldef.cpp

HEADERS  += mainwindow.h \
    factory/factory.h \
    ../../XmlBuilder/xmlbuilder.h \
    tree.h \
    mobj.h \
    ../../MainGTSD/FpgaDialogSetting/inputvalidator.h \
    ../../globaldef.h

FORMS    += mainwindow.ui

RESOURCES += \
    icontest.qrc

DISTFILES += \
    QmlTest.qml \
    CircularGaugeDarkStyle.qml
