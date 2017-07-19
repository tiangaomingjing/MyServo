#-------------------------------------------------
#
# Project created by QtCreator 2017-04-12T14:01:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TEMPLATE = app
INCLUDEPATH += $${PWD}/Include


CONFIG(debug, debug|release){
    FUNC_OUT_ROOT = $${PWD}/../../../test-build/debug
    FUNC_LIB_ROOT=$${FUNC_OUT_ROOT}
    LIBS +=$${FUNC_LIB_ROOT}/Packet.lib\
           $${FUNC_LIB_ROOT}/wpcap.lib
    TARGET = winpcap-testd
} else{
    FUNC_OUT_ROOT = $${PWD}/../../../test-build/release
    FUNC_LIB_ROOT=$${FUNC_OUT_ROOT}
    LIBS += $${FUNC_LIB_ROOT}/Packet.lib\
            $${FUNC_LIB_ROOT}/wpcap.lib
    TARGET = winpcap-test
}
DESTDIR =$${FUNC_OUT_ROOT}

SOURCES += main.cpp\
        winpcapmainwindow.cpp

HEADERS  += winpcapmainwindow.h

FORMS    += winpcapmainwindow.ui
