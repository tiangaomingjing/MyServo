import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtClass 1.0
import "./components/FlowCommon"

Rectangle {
    id:root;
    color: "#F0F0F0";
    Layout.minimumHeight: 600;
    Layout.minimumWidth: 600;

    property color arrowColor: "blue"
    property color arrowBrushColor: "yellow"

    function replot(){
        arrow1.replot();
        arrow2.replot();
        arrow3.replot();
        arrow4.replot();
        arrow5.replot();
        arrow6.replot();
        node1.replot();
    }

    Text{
        anchors.bottom: arrow1.top;
        anchors.horizontalCenter: arrow1.horizontalCenter;
        text:qsTr("电流/力矩 指令")
    }

    SegmentArrow{
        id:arrow1;
        anchors.right: inputFilter.left;
        anchors.verticalCenter: inputFilter.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(0,10),Qt.point(110,10)];
        aText: "";
    }
    FilterBlock{
        id:inputFilter;
        anchors.right: arrow2.left;
        anchors.verticalCenter: arrow2.verticalCenter;
        width: 80;
        height: 80;
        name.text:qsTr("输入滤波器")
    }
    SegmentArrow{
        id:arrow2;
        anchors.right: node1.left;
        anchors.verticalCenter: node1.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(0,10),Qt.point(40,10)];
        aText: "";
    }

    FlowNode{
        id:node1;
        width: 50;
        height: 50;
        anchors.right: arrow3.left;
        anchors.verticalCenter: arrow3.verticalCenter;
        penColor: "#BBB9B9";
        brushColor: Qt.lighter(penColor,1.2);
    }
    SegmentArrow{
        id:arrow3;
        anchors.right: m_currentLoopPID.left;
        anchors.verticalCenter: m_currentLoopPID.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(0,10),Qt.point(40,10)];
        aText: "";
    }
    PidControler{
        id:m_currentLoopPID;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.horizontalCenterOffset: 60;
        anchors.verticalCenter: parent.verticalCenter;
        iUnitStr: qsTr("us");
        onPEditTextChanged: {
            console.log("PPPPP edit changed ....");
            //factory.dataTree.setTopLevelText(0,1,pEditText);
            factory.dataTree.setTopLevelText(3,1,pEditText);
            factory.dataTree.setTopLevelText(2,1,pEditText);
        }
        onIEditTextChanged: {
            console.log("IIII edit changed ....");
            //factory.dataTree.setTopLevelText(1,1,iEditText);
            factory.dataTree.setTopLevelText(1,1,iEditText);
            factory.dataTree.setTopLevelText(0,1,iEditText);
        }
    }
    SegmentArrow{
        id:arrow4;
        anchors.left: m_currentLoopPID.right;
        anchors.verticalCenter: m_currentLoopPID.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(0,10),Qt.point(160,10)];
        aText: "";
    }
    FilterBlock{
        id:realCurrentInput;
        anchors.top: m_currentLoopPID.bottom;
        anchors.topMargin:  30;
        anchors.horizontalCenter: m_currentLoopPID.horizontalCenter;
        width: 100;
        height: 40;
        name.text:qsTr("电流反馈");
    }
    SegmentArrow{
        id:arrow5;
        anchors.left: realCurrentInput.right;
        anchors.verticalCenter: realCurrentInput.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(80,10),Qt.point(0,10)];
        aText: "";
    }
    SegmentArrow{
        id:arrow6;
        anchors.right: realCurrentInput.left;
        anchors.bottom:realCurrentInput.verticalCenter;
        anchors.bottomMargin: -10;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(135,100),Qt.point(10,100),Qt.point(10,10)];
        aText: "-";
        textPosition: Qt.point(30,30);
    }
    Text{
        anchors.bottom: arrow4.top;
        anchors.horizontalCenter: arrow4.horizontalCenter;
        text:qsTr("电流/力矩控制器 输出")
    }

    QmlFactory{
        id:factory;
        property QTreeWidgetProxy dataTree: null;

        onInitialFactory:{
            dataTree=factory.createQTreeWidgetProxy(treeSource,driveCurrentLoop);
        }
    }

    /*Rectangle{
        id:current;
        color:Qt.lighter(border.color,1.2);
        border.color: "#BBB9B9";
        border.width: 2;
//        color:"gray";
//        opacity: 0.4;//子类也透明了
        radius: 10;
        width: 400;
        height: 200;
        ColumnLayout{
            anchors.fill: parent;
            anchors.margins: 30;
            spacing: 20;
            Text{
                text:qsTr("电流环  Axis_"+axisIndex);
                font.bold: true;
            }

            RowLayout{
                spacing: 10;
                Text{
                    text:"q 轴比例控制参数P:";
                }
                TextField{
                    id:fnq_4;
                    text:"0";
                    style:textStyle;
                    focus: true;
                    z:1.5;
                    Layout.fillWidth: true;
                    Layout.minimumWidth: 50;
                    function onTheTextChanged(){fnq_4.textColor="red";factory.dataTree.setTopLevelText(3,1,fnq_4.text);}
                    Component.onCompleted: {
                        fnq_4.textChanged.connect(onTheTextChanged);
                    }
                }
            }

            RowLayout{
                spacing: 10;
                Text{
                    text:"q 轴积分控制参数I:";
                }
                TextField{
                    id:tiq_4;
                    text:"0";
                    style:textStyle;
                    focus: true;
                    z:2;
                    Layout.fillWidth: true;
                    Layout.minimumWidth: 50;
                    function onTheTextChanged(){tiq_4.textColor="red";factory.dataTree.setTopLevelText(1,1,tiq_4.text);}
                    Component.onCompleted: {
                        tiq_4.textChanged.connect(onTheTextChanged);
                    }
                }
            }

        }
    }*/

//    Component{
//        id:textStyle;
//        TextFieldStyle{
//            textColor: "black";
//            background: Rectangle{
//                radius: 6;
//                implicitWidth: 100;
//                implicitHeight: 24;
//                border.color: "#333";
//                border.width: 1;
//            }
//        }
//    }

//    Component.onCompleted: {
//        fnq_4.textColor="black";
//        tiq_4.textColor="black";
//    }

    //信号与槽
    Connections{
        target: driveCurrentLoop;
        onItemValueChanged:{
//            fnq_4.text=factory.dataTree.textTopLevel(3,1)
//            tiq_4.text=factory.dataTree.textTopLevel(1,1);
//            fnq_4.textColor="black";
//            tiq_4.textColor="black";

            m_currentLoopPID.pEditText=factory.dataTree.textTopLevel(3,1);
            m_currentLoopPID.iEditText=factory.dataTree.textTopLevel(1,1);
            m_currentLoopPID.pEditTextColor="black";
            m_currentLoopPID.iEditTextColor="black";
        }
        onActiveNow:{
            console.log("cfgcurrentLoop activenow:"+actived);
            replot();
        }
    }

}

