#-------------------------------------------------
#
# Project created by QtCreator 2016-10-08T15:19:22
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include($$PWD/../../FuncCommon.pri)

INCLUDEPATH +=$${PWD}/QCustomPlot2.0

TEMPLATE = lib

CONFIG(debug, debug|release){
    TARGET = DFTPlotd
#    LIBS+=E:/work/Servo/GTSD/FunctionDLL/PlotWave/DFTPlot/QCustomPlot2.0/customplotd.lib
} else{
    TARGET = DFTPlot
}
DEFINES+=DFTWIDGET_LIBRARY

SOURCES +=\
        dftwidget.cpp \
    dftaddform.cpp \
    QCustomPlot2.0/qcustomplot.cpp

HEADERS  += dftwidget.h \
    dftaddform.h \
    dftwidget_global.h \
    QCustomPlot2.0/qcustomplot.h

FORMS    += dftwidget.ui \
    dftaddform.ui
