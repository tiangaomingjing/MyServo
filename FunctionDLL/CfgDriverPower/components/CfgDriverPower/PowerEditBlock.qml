import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

Rectangle {
    id:lineEdit;
    color:Qt.lighter(border.color,1.2);
    border.color: "#BBB9B9";
    border.width: 2;
    radius: 10;
    width: m_text1.width+edit1.width+4*m_rowLayout1.spacing+m_unit1.width;
    height: m_rowLayout1.height+m_rowLayout2.height+m_columnLayout.spacing*3;
    Layout.minimumWidth: m_text1.width+edit1.width+4*m_rowLayout1.spacing+m_unit1.width;
    Layout.minimumHeight: 50;
    property alias title: m_text1.text;
    property alias unit: m_unit1.text;
    property alias editText: edit1.text;
    property alias editTextColor: edit1.textColor;

    ColumnLayout{
        id:m_columnLayout;
        anchors.fill: parent;
        anchors.margins: 10;
        spacing: 10;
        RowLayout{
            id:m_rowLayout1;
            spacing: 10;
            Text {
                id:m_text1;
                text: "abs";
            }
            TextField{
                id:edit1;
                text:"edit1";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 50;
                onTextChanged: {edit1.textColor="red";}
                Component.onCompleted: {
                    edit1.textColor="black";
                }
            }
            Text {
                id:m_unit1;
                text: "xx1";
            }
        }
        RowLayout{
            id:m_rowLayout2;
            spacing: 10;
            Text {
                id:m_text2;
                text: "abs";
            }
            TextField{
                id:edit2;
                text:"edit2";
                style:textStyle;
                Layout.fillWidth: true;
                Layout.minimumWidth: 50;
                onTextChanged: {edit2.textColor="red";}
                Component.onCompleted: {
                    edit2.textColor="black";
                }
            }
            Text {
                id:m_unit2;
                text: "xx2";
            }
        }
    }

    Component{
        id:textStyle;
        TextFieldStyle{
            textColor: "black";
            background: Rectangle{
                radius: 6;
                implicitWidth: 50;
                implicitHeight: 24;
                border.color: "#333";
                border.width: 1;
            }
        }
    }
}

