import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
//import QtClass 1.0
Rectangle
{
    id:cfgDriverPower;
    Layout.minimumHeight: 560;
    Layout.minimumWidth: 600;
    color: "#F0F0F0";
    objectName: "driverPower";
//    QmlFactory{
//        id:factory;
//        property QTreeWidgetProxy dataTree: null;
//        onInitialFactory:{
//            dataTree=factory.createQTreeWidgetProxy(treeSource,driverPower);
//        }
//    }
    Component{
        id:textStyle;
        TextFieldStyle{
            textColor: "black";
            background: Rectangle{
                radius: 6;
                implicitHeight: 24;
                implicitWidth: 50;
                border.color: "#333";
                border.width: 1;
            }
        }
    }
    Text{
        id:t1
        text:"母线过压保护点(V)";
        Layout.fillWidth: true;
        horizontalAlignment: Text.AlignLeft;
    }
    TextField{
        id:vdcUp;
        text:"0";
        style: textStyle;
        Layout.fillWidth: true;
        Layout.minimumWidth: 50;
        onTextChanged: {
            vdcUp.textColor="red";
        }
//        Component.onCompleted: {
//            vdcUp.textColor="black";
//        }
    }
    Text{
        id:t2
        text:"母线欠压保护点(V)";
        Layout.fillWidth: true;
        horizontalAlignment: Text.AlignLeft;
    }
    TextField{
        id:vdcDown;
        text:"0";
        style: textStyle;
        Layout.fillWidth: true;
        Layout.minimumWidth: 50;
        onTextChanged: {
            vdcDown.textColor="red";
        }
//        Component.onCompleted: {
//            vdcDown.textColor="black";
//        }
    }
    Text{
        id:t3;
        text:"制动管开通电压(V)";
        Layout.fillWidth: true;
        horizontalAlignment: Text.AlignLeft;
    }
    TextField{
        id:brakeUp;
        text:"0";
        style: textStyle;
        Layout.fillWidth: true;
        Layout.minimumWidth: 50;
        onTextChanged: {
            brakeUp.textColor="red";
        }
//        Component.onCompleted: {
//            brakeUp.textColor="black";
//        }
    }
    Text{
        id:t4;
        text:"制动管关闭电压(V)";
        Layout.fillWidth: true;
        horizontalAlignment: Text.AlignLeft;
    }
    TextField{
        id:brakeDown;
        text:"0";
        style: textStyle;
        Layout.fillWidth: true;
        Layout.minimumWidth: 50;
        onTextChanged: {
            brakeDown.textColor="red";
        }
//        Component.onCompleted: {
//            brakeDown.textColor="black";
//        }
    }
    Text{
        id:t5;
        text:"软启动继电器吸合电压(V)";
        Layout.fillWidth: true;
        horizontalAlignment: Text.AlignLeft;
    }
    TextField{
        id:softUp;
        text:"0";
        style: textStyle;
        Layout.fillWidth: true;
        Layout.minimumWidth: 50;
        onTextChanged: {
            softUp.textColor="red";
        }
//        Component.onCompleted: {
//            softUp.textColor="black";
//        }
    }
//    Text{
//        id:t6;
//        x:10;
//        y:10;
//        text:"软启动继电器断开电压(V)";
//        Layout.fillWidth: true;
//        horizontalAlignment: Text.AlignLeft;
//    }
    TextField{
        x:20;
        y:60;
        id:softDown;
        text:"0";
        style: textStyle;
        Layout.fillWidth: true;
        Layout.minimumWidth: 50;
        onTextChanged: {
            softDown.textColor="red";
        }
//        Component.onCompleted: {
//            softDown.textColor="black";
//        }
    }


}
