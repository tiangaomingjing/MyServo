TEMPLATE = lib
TARGET = TreeModelLibrary
QT += qml quick widgets designer
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.googoltech.qmlcomponents.TreeModel

CONFIG(debug, debug|release){
    OUTPATH=$${PWD}/../../../../debug/Bin/com/googoltech/qmlcomponents/TreeModel
} else{
    OUTPATH=$${PWD}/../../../../release/Bin/com/googoltech/qmlcomponents/TreeModel
}

DESTDIR =$${OUTPATH}

# Input
SOURCES += \
    treemodellibrary_plugin.cpp \
    treemodel.cpp \
    treeitem.cpp

HEADERS += \
    treemodellibrary_plugin.h \
    treemodel.h \
    treeitem.h

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

