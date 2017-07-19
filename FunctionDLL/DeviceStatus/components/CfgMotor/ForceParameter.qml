import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle
{
    id:forceParameter;
    color:Qt.lighter(border.color,1.2);
    border.color: "#BBB9B9";
    border.width: 2;
    radius: 10;
    width: 250;
    height: 100;
    property alias tqr_1: tqr_1;
    property alias phim_1: phim_1;
    property alias vmax_1: vmax_1;
    property var dataTree: null;

    ColumnLayout{
        anchors.fill: parent;
        anchors.margins: 20;
        spacing: 10;
        Text {
            text: qsTr("5 力 矩 参 数");
            font.bold: true;
        }
        RowLayout{
            spacing: 10;
            Text{text:"额定转矩(N.m)"}
            TextField{
                id:tqr_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {tqr_1.textColor="red";dataTree.setTopLevelText(5,1,tqr_1.text);}
                function onTheTextChanged(){tqr_1.textColor="red";dataTree.setTopLevelText(5,1,tqr_1.text);}
                Component.onCompleted: {
                    tqr_1.textChanged.connect(onTheTextChanged);
                }
            }
            Text{text:"反电动势系数(mV/rpm)"}
            TextField{
                id:phim_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {phim_1.textColor="red";dataTree.setTopLevelText(8,1,phim_1.text);}
                function onTheTextChanged(){phim_1.textColor="red";dataTree.setTopLevelText(8,1,phim_1.text);}
                Component.onCompleted: {
                    phim_1.textChanged.connect(onTheTextChanged);
                }
            }
            Text{text:"最大电压(V)"}
            TextField{
                id:vmax_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {vmax_1.textColor="red";dataTree.setTopLevelText(7,1,vmax_1.text);}
                function onTheTextChanged(){vmax_1.textColor="red";dataTree.setTopLevelText(7,1,vmax_1.text);}
                Component.onCompleted: {
                    vmax_1.textChanged.connect(onTheTextChanged);
                }
            }
        }
    }

    Component{
        id:textStyle;
        TextFieldStyle{
            textColor: "black";
            background: Rectangle{
                radius: 6;
                implicitWidth: 100;
                implicitHeight: 24;
                border.color: "#333";
                border.width: 1;
            }
        }
    }

    Component.onCompleted: {
        tqr_1.textColor="black";
        phim_1.textColor="black";
        vmax_1.textColor="black";
    }
}

