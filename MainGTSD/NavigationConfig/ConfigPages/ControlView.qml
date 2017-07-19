import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle {
    id:controlView;
    color:"lightgray"
    width: 400;
    height: 600;
    property Component control;
    property alias controlItem: controlLoader.item;
    property string backButtonText: ""
    property bool isConnected: false;
    signal backBtnClicked();
    signal connectBtnClicked();
    signal disConnectBtnClicked();

    ColumnLayout{
        anchors.fill: parent;
        spacing: 0;
        Loader{
            id:controlLoader;
            sourceComponent: control;
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            property alias view: controlView
        }
        //回退框
        Rectangle{
            id:backRect;
            Layout.fillWidth: true;
            Layout.minimumHeight: 40;
            height: 40;
            color:Qt.darker("gray",1.2);
            Text{
                anchors.centerIn: parent;
                text:backButtonText;
                color: "white"
                font.pixelSize: 16;
            }
            RowLayout{
                anchors.fill: parent;
                spacing: 0;
                //返回按扭
                Rectangle{
                    Layout.fillHeight: true;
                    implicitWidth: height
                    color:backBtn.containsMouse||backBtn.pressed?Qt.darker(backRect.color,2):backRect.color;
                    Image {
                        anchors.centerIn: parent;
                        width: height*0.7;
                        height: parent.height*0.7;
                        source: "./Resource/Icon/common/icon-back.png"
                    }
                    MouseArea{
                        id:backBtn;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: backBtnClicked();
                    }
                }
                //连接按钮
                Rectangle{
                    Layout.fillHeight: true;
                    implicitWidth: (backRect.width-backRect.height)/2
                    color:connectBtn.containsMouse||connectBtn.pressed?Qt.darker(backRect.color,2):backRect.color;
                    Text {
                        anchors.centerIn: parent;
                        text: qsTr("连接")
                        font.pixelSize: 16;
                        font.bold: true;
                        color:isConnected?"white":"black";
                    }
                    MouseArea{
                        id:connectBtn;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            isConnected=true;
                            connectBtnClicked();
                        }
                    }
                }
                //断开连接按键
                Rectangle{
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                    color:disConnectBtn.containsMouse||disConnectBtn.pressed?Qt.darker(backRect.color,2):backRect.color;
                    Text {
                        anchors.centerIn: parent;
                        text: qsTr("断开")
                        font.pixelSize: 16;
                        font.bold: true;
                        color:!isConnected?"white":"black";
                    }
                    MouseArea{
                        id:disConnectBtn;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            isConnected=false;
                            disConnectBtnClicked();
                        }
                    }
                }
            }
        }
    }

}

