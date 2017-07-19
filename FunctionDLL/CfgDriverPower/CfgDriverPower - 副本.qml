import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtClass 1.0

Rectangle
{
    id:root;
    Layout.minimumHeight: 560;
    Layout.minimumWidth: 600;
    color: "#F0F0F0";
    objectName: "rootObj";
    signal valueChanged();

    QmlFactory
    {
        id:factory;
        property QTreeWidgetProxy dataTree: null;

        onInitialFactory:
        {
            dataTree=factory.createQTreeWidgetProxy(treeSource,driverPower);
        }
    }


    Image
    {
        id: servo;
        width: 100;
        height: 100;
        anchors.left: row.left
        anchors.bottom: row.top;
        anchors.bottomMargin: 30;
//        anchors.fill: parent;
        source: "file:///"+iconPath+"blue_battery.png"
    }
    Text
    {
        id:power;
        anchors.left: servo.right;
        anchors.leftMargin: 40;
        anchors.bottom: servo.bottom;
        text:"Axis_"+axisIndex+" Power";
        font.bold: true;
    }
    Row
    {
        id:row;
        anchors.centerIn: parent;
        spacing: 40;

        Rectangle
        {
            id:ret1;
            color:Qt.lighter(border.color,1.2);
            border.color: "#BBB9B9";
            border.width: 2;
            radius: 10;
            width: 250;
            height: 100;
            Column
            {
                anchors.centerIn: parent
                spacing: 10;
                Row
                {
                    spacing: 10;
                    Text
                    {
                        id:label;
                        height: 24;
                        verticalAlignment:  Text.AlignVCenter
                        text:"母线过压保护点(V)"
                    }
                    TextField
                    {
                       id: vdc_up;
                       placeholderText: qsTr("Enter description");
                       text:"0";
                       style: TextFieldStyle
                       {
                           id:textStyle;
                           textColor: "black"
                           background: Rectangle
                           {
                               radius: 6
                    //                color:"green";
                               implicitWidth: 100
                               implicitHeight: 24
                               border.color: "#333"
                               border.width: 1;
                           }
                       }
//                       onTextChanged:
//                       {
//                           vdc_up.textColor="red";
////                           root.valueChanged();
//                           factory.dataTree.setTopLevelText(0,1,vdc_up.text);
//                       }
                       function onTheTextChanged(){vdc_up.textColor="red";factory.dataTree.setTopLevelText(0,1,vdc_up.text);}
                       Component.onCompleted: {
                           vdc_up.textChanged.connect(onTheTextChanged);
                       }

                    }
                }
                Row
                {
                    spacing: 10;
                    Text{verticalAlignment:  Text.AlignVCenter;height: 24;text:"母线欠压保护点(V)"}
                    TextField
                    {
                       id: vdc_down;
                       placeholderText: qsTr("Enter description2");
                       text:"0";
                       style: TextFieldStyle
                       {
                           id:vdc_downStyle;
                           textColor: "black"
                           background: Rectangle
                           {
                               radius: 6
                    //                color:"green";
                               implicitWidth: 100
                               implicitHeight: 24
                               border.color: "#333"
                               border.width: 1;
                           }
                       }
//                       onTextChanged:
//                       {
//                           vdc_down.textColor="red";
////                           root.valueChanged();
//                           factory.dataTree.setTopLevelText(1,1,vdc_down.text);
//                       }
                       function onTheTextChanged(){vdc_down.textColor="red";factory.dataTree.setTopLevelText(1,1,vdc_down.text);}
                       Component.onCompleted: {
                           vdc_down.textChanged.connect(onTheTextChanged);
                       }

                    }
                }
            }
        }

        Rectangle
        {
            id:ret2;
            color:Qt.lighter(border.color,1.2);
            border.color: "#BBB9B9";
            border.width: 2;
            radius: 10;
            width: 250;
            height: 100;
            Column
            {
                anchors.centerIn: parent
                spacing: 10;
                Row
                {
                    spacing: 10;
                    Text
                    {
                        id:label2;
                        height: 24;
                        verticalAlignment:  Text.AlignVCenter
                        text:"制动管开通电压(V)"
                    }
                    TextField
                    {
                       id: brake_up;
                       placeholderText: qsTr("Enter description");
                       text:"0";
                       style: TextFieldStyle
                       {
                           id:textStyle11;
                           textColor: "black"
                           background: Rectangle
                           {
                               radius: 6
                    //                color:"green";
                               implicitWidth: 100
                               implicitHeight: 24
                               border.color: "#333"
                               border.width: 1;
                           }
                       }
//                       onTextChanged:
//                       {
//                           brake_up.textColor="red";
////                           root.valueChanged();
//                           factory.dataTree.setTopLevelText(2,1,brake_up.text);
//                       }
                        function onTheTextChanged(){brake_up.textColor="red";factory.dataTree.setTopLevelText(2,1,brake_up.text);}
                        Component.onCompleted: {
                            brake_up.textChanged.connect(onTheTextChanged);
                        }

                    }
                }
                Row
                {
                    spacing: 10;
                    Text{verticalAlignment:  Text.AlignVCenter;height: 24;text:"制动管关闭电压(V)"}
                    TextField
                    {
                       id: brake_down;
                       placeholderText: qsTr("Enter description2");
                       text:"0";
                       style: TextFieldStyle
                       {
                           id:textStyle22;
                           textColor: "black"
                           background: Rectangle
                           {
                               radius: 6
                    //                color:"green";
                               implicitWidth: 100
                               implicitHeight: 24
                               border.color: "#333"
                               border.width: 1;
                           }
                       }
//                       onTextChanged:
//                       {
//                           brake_down.textColor="red";
////                            root.valueChanged();
//                            factory.dataTree.setTopLevelText(3,1,brake_down.text);
//                       }
                        function onTheTextChanged(){brake_down.textColor="red";factory.dataTree.setTopLevelText(3,1,brake_down.text);}
                        Component.onCompleted: {
                            brake_down.textChanged.connect(onTheTextChanged);
                        }

                    }
                }
            }
        }
        Rectangle
        {
            id:ret3;
            color:Qt.lighter(border.color,1.2);
            border.color: "#BBB9B9";
            border.width: 2;
            radius: 10;
            width: 300;
            height: 100;
            Column
            {
                anchors.centerIn: parent
                spacing: 10;
                Row
                {
                    spacing: 10;
                    Text
                    {
                        id:labe3l;
                        height: 24;
                        verticalAlignment:  Text.AlignVCenter
                        text:"软启动继电器吸合电压(V)"
                    }
                    TextField
                    {
                       id: softon_up;
                       placeholderText: qsTr("Enter description");
                       text:"0";
                       style: TextFieldStyle
                       {
                           id:textStyle3;
                           textColor: "black"
                           background: Rectangle
                           {
                               radius: 6
                    //                color:"green";
                               implicitWidth: 100
                               implicitHeight: 24
                               border.color: "#333"
                               border.width: 1;
                           }
                       }
//                       onTextChanged:
//                       {
//                           softon_up.textColor="red";
////                           root.valueChanged();
//                           factory.dataTree.setTopLevelText(4,1,softon_up.text);
//                       }
                        function onTheTextChanged(){softon_up.textColor="red";factory.dataTree.setTopLevelText(4,1,softon_up.text);}
                        Component.onCompleted: {
                            softon_up.textChanged.connect(onTheTextChanged);
                        }

                    }
                }
                Row
                {
                    spacing: 10;
                    Text{verticalAlignment:  Text.AlignVCenter;height: 24;text:"软启动继电器断开电压(V)"}
                    TextField
                    {
                       id: softon_down;
                       placeholderText: qsTr("Enter description2");
                       text:"0";
                       style: TextFieldStyle
                       {
                           id:textStyle32;
                           textColor: "black"
                           background: Rectangle
                           {
                               radius: 6
                    //                color:"green";
                               implicitWidth: 100
                               implicitHeight: 24
                               border.color: "#333"
                               border.width: 1;
                           }
                       }
//                       onTextChanged:
//                       {
//                           softon_down.textColor="red";
////                           root.valueChanged();
//                           factory.dataTree.setTopLevelText(5,1,softon_down.text);
//                       }
                        function onTheTextChanged(){softon_down.textColor="red";factory.dataTree.setTopLevelText(5,1,softon_down.text);}
                        Component.onCompleted: {
                            softon_down.textChanged.connect(onTheTextChanged);
                        }

                    }
                }
            }
        }
    }
    Component.onCompleted:
    {
    softon_down.text="0";
    softon_up.text="0";
    vdc_up.text="0";
    vdc_down.text="0";
    brake_up.text="0";
    brake_down.text="0";
    softon_down.textColor="black";
    softon_up.textColor="black";
    vdc_up.textColor="black";
    vdc_down.textColor="black";
    brake_up.textColor="black";
    brake_down.textColor="black";
    }
    Connections
    {
        target: driverPower;
        onItemValueChanged:
        {
        vdc_up.text=factory.dataTree.textTopLevel(0,1);
        vdc_down.text=factory.dataTree.textTopLevel(1,1);
        brake_up.text=factory.dataTree.textTopLevel(2,1);
        brake_down.text=factory.dataTree.textTopLevel(3,1);
        softon_up.text=factory.dataTree.textTopLevel(4,1);
        softon_down.text=factory.dataTree.textTopLevel(5,1);

        softon_down.textColor="black";
        softon_up.textColor="black";
        vdc_up.textColor="black";
        vdc_down.textColor="black";
        brake_up.textColor="black";
        brake_down.textColor="black";
        }
    }
    onValueChanged:
    {
        console.log("qml log value changed ...");
    }
    Component.onDestruction: {
        console.log("CfgDriverPower QML release ->");
    }
}
