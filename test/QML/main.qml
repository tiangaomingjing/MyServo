import QtQuick 2.3
import QtQuick.Window 2.2
import "./V124/"
Item {
    id:rootItem;
    width: 100;
    height: 150;

//    MouseArea {
//        anchors.fill: parent
//        onClicked: {
//            Qt.quit();
//        }
//    }

//    Text {
//        text: qsTr("Hello World")
//        anchors.centerIn: parent
//    }
    CfgEncoder{
        anchors.fill: parent;
    }
}

