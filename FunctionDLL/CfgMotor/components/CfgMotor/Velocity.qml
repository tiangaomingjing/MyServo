import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

Rectangle
{
    id:velocity;
    color:Qt.lighter(border.color,1.2);
    border.color: "#BBB9B9";
    border.width: 2;
    radius: 10;
    width: 250;
    height: 100;
    property alias sct_1: sct_1;
    property alias srat_1: srat_1;
    property alias nos_1: nos_1;
    property var dataTree: null;
    ColumnLayout{
        anchors.fill: parent;
        anchors.margins: 20;
        spacing: 10;
        Text{
            text:"2 速 度";
            font.bold: true;
        }
        RowLayout{
            spacing: 10;
            Text{
                text:"额定转速(rpm)"
            }
            TextField{
                id:sct_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {sct_1.textColor="red";dataTree.setTopLevelText(2,1,sct_1.text);}
                function onTheTextChanged(){sct_1.textColor="red";dataTree.setTopLevelText(2,1,sct_1.text);}
                Component.onCompleted: {
                    sct_1.textChanged.connect(onTheTextChanged);
                }
            }

            Text{
                text:"过速百分比(%)"
            }
            TextField{
                id:srat_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {srat_1.textColor="red";dataTree.setTopLevelText(3,1,srat_1.text);}
                function onTheTextChanged(){srat_1.textColor="red";dataTree.setTopLevelText(3,1,srat_1.text);}
                Component.onCompleted: {
                    srat_1.textChanged.connect(onTheTextChanged);
                }
            }

            Text{
                text:"最大转速(rpm)"
            }
            TextField{
                id:nos_1;
                text:"0";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 20;
//                onTextChanged: {nos_1.textColor="red";dataTree.setTopLevelText(4,1,nos_1.text);}
                function onTheTextChanged(){nos_1.textColor="red";dataTree.setTopLevelText(4,1,nos_1.text);}
                Component.onCompleted: {
                    nos_1.textChanged.connect(onTheTextChanged);
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
        sct_1.textColor="black";
        srat_1.textColor="black";
        nos_1.textColor="black";
    }
}


