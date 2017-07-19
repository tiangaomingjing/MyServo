TEMPLATE = lib
TARGET = QmlUserConfigProxy
QT += core widgets qml quick
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.googoltech.qmlcomponents.QmlUserConfigProxy

INCLUDEPATH+=../../../

CONFIG(debug, debug|release){
    OUTPATH=$${PWD}/../../../../debug/Bin/com/googoltech/qmlcomponents/QmlUserConfigProxy
} else{
    OUTPATH=$${PWD}/../../../../release/Bin/com/googoltech/qmlcomponents/QmlUserConfigProxy
}
DESTDIR=$${OUTPATH}

# Input
SOURCES += \
    qmluserconfigproxy_plugin.cpp \
    qmluserconfigproxy.cpp

HEADERS += \
    qmluserconfigproxy_plugin.h \
    qmluserconfigproxy.h

DISTFILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

