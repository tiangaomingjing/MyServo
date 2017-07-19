#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T11:58:22
#
#-------------------------------------------------

QT       += widgets xml printsupport core designer qml

QT       -= gui

TEMPLATE = lib
QscintillaPath=$${PWD}/DialogPickCurve/Qscintilla

INCLUDEPATH+=$${PWD}\
             $${PWD}/DFTPlot\
             $${PWD}/../ServoGeneralCmd\
             $${PWD}/MuParser/include\
             $${PWD}/DialogPickCurve\
             $${QscintillaPath}/includes\
             $${PWD}/wheelwidget

#------------------------------------------------
include($$PWD/../FuncCommon.pri)

CONFIG(debug, debug|release){
    TARGET = PlotWaved
    LIBS += $${FUNC_LIB_ROOT}/ServoDriverAlgorithmDlld.lib\
          $${FUNC_LIB_ROOT}/ServoGeneralCmdd.lib\
          $${QscintillaPath}/lib/vc12/qscintilla2_qt5d.lib
} else{
    TARGET = PlotWave
    LIBS += $${FUNC_LIB_ROOT}/ServoDriverAlgorithmDll.lib\
            $${FUNC_LIB_ROOT}/ServoGeneralCmd.lib\
            $${QscintillaPath}/lib/vc12/qscintilla2_qt5.lib
}
#---------------------------------------------------
# _UNICODE 给muparser使用
DEFINES += PLOTWAVE_LIBRARY\
           _UNICODE

SOURCES += plotwave.cpp \
    DlgSetXYRange/DlgSetXYRange.cpp \
    boxzoom.cpp \
    meacursorx.cpp \
    meacursor.cpp \
    meacursory.cpp \
    measuretag.cpp \
    DlgSelectCurve/dlgselectcurve.cpp \
    ../../globaldef.cpp \
    ../../XmlBuilder/xmlbuilder.cpp \
    threadsample.cpp \
    ../../ServoControl/servocontrol.cpp \
    qcustomplot.cpp \
    DFTPlot/dftwidget.cpp \
    DFTPlot/viewform.cpp \
    MotionStatus/motionstatus.cpp \
    QtTreeManager/qttreemanager.cpp \
    DialogPickCurve/dialogpickcurve.cpp \
    MuParser/src/muParser.cpp \
    MuParser/src/muParserBase.cpp \
    MuParser/src/muParserBytecode.cpp \
    MuParser/src/muParserCallback.cpp \
    MuParser/src/muParserDLL.cpp \
    MuParser/src/muParserError.cpp \
    MuParser/src/muParserInt.cpp \
    MuParser/src/muParserTest.cpp \
    MuParser/src/muParserTokenReader.cpp \
    UserActionWidget/useractionwidget.cpp \
    DialogPickCurve/usrcurvetreemanager.cpp \
    DialogPickCurve/customcurvepage.cpp \
    DialogPickCurve/utreewidget.cpp \
    wheelwidget/wheelwidget.cpp \
    wheelwidget/rollboxwidget.cpp

HEADERS += plotwave.h\
        plotwave_global.h \
    DlgSetXYRange/DlgSetXYRange.h \
    boxzoom.h \
    meacursorx.h \
    meacursor.h \
    meacursory.h \
    measuretag.h \
    DlgSelectCurve/dlgselectcurve.h \
    ../../globaldef.h \
    ../../XmlBuilder/xmlbuilder.h \
    threadsample.h \
    plotwave_globaldef.h \
    ../../ServoControl/servocontrol.h \
    qcustomplot.h \
    DFTPlot/dftwidget.h \
    DFTPlot/viewform.h \
    MotionStatus/motionstatus.h \
    QtTreeManager/qttreemanager.h \
    DialogPickCurve/dialogpickcurve.h \
    MuParser/include/muParser.h \
    MuParser/include/muParserBase.h \
    MuParser/include/muParserBytecode.h \
    MuParser/include/muParserCallback.h \
    MuParser/include/muParserDef.h \
    MuParser/include/muParserDLL.h \
    MuParser/include/muParserError.h \
    MuParser/include/muParserFixes.h \
    MuParser/include/muParserInt.h \
    MuParser/include/muParserStack.h \
    MuParser/include/muParserTemplateMagic.h \
    MuParser/include/muParserTest.h \
    MuParser/include/muParserToken.h \
    MuParser/include/muParserTokenReader.h \
    UserActionWidget/useractionwidget.h \
    DialogPickCurve/usrcurvetreemanager.h \
    DialogPickCurve/customcurvepage.h \
    DialogPickCurve/utreewidget.h \
    wheelwidget/wheelwidget.h \
    wheelwidget/rollboxwidget.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    PlotWave.ui \
    DlgSetXYRange/DlgSetXYRange.ui \
    DlgSelectCurve/DlgSelectCurve.ui \
    DFTPlot/dftwidget.ui \
    DFTPlot/viewform.ui \
    DialogPickCurve/dialogpickcurve.ui \
    UserActionWidget/useractionwidget.ui \
    DialogPickCurve/customcurvepage.ui

RESOURCES += \
    icon.qrc \
    DialogPickCurve/image.qrc
TRANSLATIONS +=app_plotwave_simplchinese.ts
