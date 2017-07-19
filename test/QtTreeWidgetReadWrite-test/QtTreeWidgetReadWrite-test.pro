#-------------------------------------------------
#
# Project created by QtCreator 2017-05-03T14:35:27
#
#-------------------------------------------------

QT       += core gui designer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QscintillaPath=$${PWD}/Qscintilla

INCLUDEPATH+=$${QscintillaPath}/includes


TEMPLATE = app

CONFIG(debug, debug|release){
    FUNC_OUT_ROOT = $${PWD}/../../../test-build/debug
    FUNC_LIB_ROOT=$${FUNC_OUT_ROOT}
    TARGET = QtTreeWidgetReadWrite-testd
    LIBS+=$${QscintillaPath}/lib/vc12/qscintilla2_qt5d.lib
} else{
    FUNC_OUT_ROOT = $${PWD}/../../../test-build/release
    FUNC_LIB_ROOT=$${FUNC_OUT_ROOT}
#    LIBS += $${FUNC_LIB_ROOT}/Packet.lib\
#            $${FUNC_LIB_ROOT}/wpcap.lib
    LIBS+=$${QscintillaPath}/lib/vc12/qscintilla2_qt5.lib
    TARGET = QtTreeWidgetReadWrite-test
}
DESTDIR =$${FUNC_OUT_ROOT}


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../FunctionDLL/PlotWave/QtTreeManager/qttreemanager.cpp \
    useractionwidget.cpp \
    mycombobox.cpp \
    customcurvepage.cpp \
    mousebase.cpp \
    utreewidget.cpp

HEADERS  += mainwindow.h \
    ../../FunctionDLL/PlotWave/QtTreeManager/qttreemanager.h \
    useractionwidget.h \
    mycombobox.h \
    customcurvepage.h \
    mousebase.h \
    utreewidget.h

FORMS    += mainwindow.ui \
    useractionwidget.ui \
    customcurvepage.ui

RESOURCES += \
    file.qrc
