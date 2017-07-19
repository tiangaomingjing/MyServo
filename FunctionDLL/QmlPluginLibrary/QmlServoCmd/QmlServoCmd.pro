TEMPLATE = lib
TARGET = QmlServoCmd
QT += qml quick core widgets designer
CONFIG += qt plugin c++11

INCLUDEPATH+= $${PWD}/../../../NetCmd/ServoDriverComDll\
              $${PWD}/../../../NetCmd/ServoDriverComDll/NetCom/include\
              $${PWD}/../../../MainGTSD\
              $${PWD}/../../../XmlBuilder\
              $${PWD}/../../../

TARGET = $$qtLibraryTarget($$TARGET)
uri = com.googoltech.qmlcomponents

CONFIG(debug, debug|release){
    LIBS +=$${PWD}/../../../../debug/Bin/ServoDriverComDlld.lib
    OUTPATH=$${PWD}/../../../../debug/Bin/com/googoltech/qmlcomponents
} else{
    LIBS +=$${PWD}/../../../../release/Bin/ServoDriverComDll.lib
    OUTPATH=$${PWD}/../../../../release/Bin/com/googoltech/qmlcomponents
}

DESTDIR =$${OUTPATH}

# Input
SOURCES += \
    qmlservocmd_plugin.cpp \
    servocmd.cpp \
    ../TreeModelLibrary/treeitem.cpp \
    ../TreeModelLibrary/treemodel.cpp

HEADERS += \
    qmlservocmd_plugin.h \
    servocmd.h \
    ../TreeModelLibrary/treeitem.h \
    ../TreeModelLibrary/treemodel.h

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

