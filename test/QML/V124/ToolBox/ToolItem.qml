import QtQuick 2.2
import QtQuick.Layouts 1.1

Rectangle {
    id:m_toolItem;
    property int index: 0;
    property url loaderSource: "AbsoluteEncoder.qml";
    property alias controlItem: m_loader.item;
    property string btnText: "Button";
    readonly property alias btnHeight_RO: m_buttonItem.height;
    property bool actived: false;
    Behavior on height {NumberAnimation{}}

    function show(idx){
        if(index!==idx)
            actived=false;
        else
            actived=true;
        console.log(qsTr("curren=%1 idx=%2,actived=%3").arg(index).arg(idx).arg(actived));
    }
    signal btnClicked(int idx);
    ColumnLayout{
        anchors.fill: parent;
        spacing: 0;
        Rectangle{
            id:m_buttonItem;
            Layout.minimumHeight: 50;
            Layout.fillWidth: true;
            color: buttonItemMouse.containsMouse?"gray":Qt.lighter("gray");
            z:50000;
            MouseArea{
                id:buttonItemMouse;
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked:m_toolItem.btnClicked(m_toolItem.index);
            }
            Text{
                anchors.centerIn: parent
                text:btnText;
                color: buttonItemMouse.pressed?"white":"black";
            }
            Rectangle{
                anchors.bottom: parent.bottom;
                anchors.left: parent.left;
                width: parent.width;
                height: parent.height*0.01;
//                color:actived?"transparent":"#F0F0F0";
                color:actived?"black":"gray";
            }
        }
        Loader{
           id:m_loader;
           Layout.fillWidth: true;
           Layout.fillHeight: true;
           focus: true;
           source: loaderSource;
        }

    }

}

