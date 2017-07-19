import QtQuick 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Rectangle
{
    id:impedance;
    color:Qt.lighter(border.color,1.2);
    border.color: "#BBB9B9";
    border.width: 2;
    radius: 10;
    width: 250;
    height: 100;
    property alias rm_1: rm_1;
    property alias ldm_1: ldm_1;
    property alias lqm_1: lqm_1;
    property var dataTree: null;
    ColumnLayout{
        anchors.fill: parent;
        anchors.margins: 20;
        spacing: 10;
        Text{
            text:"3 阻 抗"
            font.bold: true;
        }
        RowLayout{
            spacing: 10;
            Text{
                text:"绕线电阻(Ohm)"
            }
            TextField{
                id:rm_1;
                text:"0";
                style: textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {rm_1.textColor="red";dataTree.setTopLevelText(11,1,rm_1.text);}
                function onTheTextChanged(){rm_1.textColor="red";dataTree.setTopLevelText(11,1,rm_1.text);}
                Component.onCompleted: {
                    rm_1.textChanged.connect(onTheTextChanged);
                }
            }
            Text{
                text:"d轴电感(mH)"
            }
            TextField{
                id:ldm_1;
                text:"0";
                style: textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {ldm_1.textColor="red";dataTree.setTopLevelText(9,1,ldm_1.text);}
                function onTheTextChanged(){ldm_1.textColor="red";dataTree.setTopLevelText(9,1,ldm_1.text);}
                Component.onCompleted: {
                    ldm_1.textChanged.connect(onTheTextChanged);
                }
            }
            Text{
                text:"q轴电感(mH)"
            }
            TextField{
                id:lqm_1;
                text:"0";
                style: textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {lqm_1.textColor="red";dataTree.setTopLevelText(10,1,lqm_1.text);}
                function onTheTextChanged(){lqm_1.textColor="red";dataTree.setTopLevelText(10,1,lqm_1.text);}
                Component.onCompleted: {
                    lqm_1.textChanged.connect(onTheTextChanged);
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
        rm_1.textColor="black";
        ldm_1.textColor="black";
        lqm_1.textColor="black";
    }
}


