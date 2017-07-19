#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T14:33:51
#
#-------------------------------------------------

QT       += widgets xml

QT       -= gui

TEMPLATE = lib
#------------------------------------------------
include($$PWD/../FuncCommon.pri)
INCLUDEPATH +=../AbstractFuncWidget
CONFIG(debug, debug|release){
    TARGET = CfgMoveProfiled
    LIBS +=$${FUNC_LIB_ROOT}/AbstractFuncWidgetd.lib
} else{
    TARGET = CfgMoveProfile
    LIBS += $${FUNC_LIB_ROOT}/AbstractFuncWidget.lib
}
#---------------------------------------------------

DEFINES += CFGMOVEPROFILE_LIBRARY

SOURCES += cfgmoveprofile.cpp \
    ../../globaldef.cpp

HEADERS += cfgmoveprofile.h\
        cfgmoveprofile_global.h \
    ../../globaldef.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    CfgMoveProfile.ui
