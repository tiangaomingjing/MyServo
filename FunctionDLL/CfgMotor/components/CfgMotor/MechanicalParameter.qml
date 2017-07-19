import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

Rectangle
{
    id:mechanicalParameter;
    color:Qt.lighter(border.color,1.2);
    border.color: "#BBB9B9";
    border.width: 2;
    radius: 10;
    width: 250;
    height: 100;
    property alias jm_1: jm_1;
    property alias jrat_1: jrat_1;
    property alias fm_1: fm_1;
    property alias ppn_1: ppn_1;
    property var dataTree: null;
    ColumnLayout{
        anchors.fill: parent;
        anchors.margins: 20;
        spacing: 10;
        Text{
            text:"4 机 械 参 数";
            font.bold: true;
        }
        RowLayout{
            spacing: 10;
            Text{text:"转动惯量(10^-6.kg.m^2)"}
            TextField{
                id:jm_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {jm_1.textColor="red";dataTree.setTopLevelText(12,1,jm_1.text);}
                function onTheTextChanged(){jm_1.textColor="red";dataTree.setTopLevelText(12,1,jm_1.text);}
                Component.onCompleted: {
                    jm_1.textChanged.connect(onTheTextChanged);
                }
            }
            Text{text:"惯量比(%)"}
            TextField{
                id:jrat_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {jrat_1.textColor="red";dataTree.setTopLevelText(13,1,jrat_1.text);}
                function onTheTextChanged(){jrat_1.textColor="red";dataTree.setTopLevelText(13,1,jrat_1.text);}
                Component.onCompleted: {
                    jrat_1.textChanged.connect(onTheTextChanged);
                }
            }
            Text{text:"摩擦系数(0.0001.N.m/(rad/s))"}
            TextField{
                id:fm_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {fm_1.textColor="red";dataTree.setTopLevelText(14,1,fm_1.text);}
                function onTheTextChanged(){fm_1.textColor="red";dataTree.setTopLevelText(14,1,fm_1.text);}
                Component.onCompleted: {
                    fm_1.textChanged.connect(onTheTextChanged);
                }
            }
            Text{text:"极对数"}
            TextField{
                id:ppn_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {ppn_1.textColor="red";dataTree.setTopLevelText(6,1,ppn_1.text);}
                function onTheTextChanged(){ppn_1.textColor="red";dataTree.setTopLevelText(6,1,ppn_1.text);}
                Component.onCompleted: {
                    ppn_1.textChanged.connect(onTheTextChanged);
                }
            }
        }
    }
    //编辑框的样式
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
        jm_1.textColor="black";
        jrat_1.textColor="black";
        fm_1.textColor="black";
        ppn_1.textColor="black";
    }
}


