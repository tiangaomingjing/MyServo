import QtQuick 2.5
import QtQuick.Layouts 1.1
import "./components/FlowCommon"
import "./components/CfgPositionLoop"
import QtClass 1.0

Rectangle {
    id:root;
    width: 1000;
    height: 600;
    color: "#F0F0F0";
    function replot(){
        m_positionLimit.replot();
        m_saturation.replot();
        m_node1.replot();
        m_node2.replot();
        m_node3.replot();
        arrow1.replot();
        arrow2.replot();
        arrow3.replot();
        arrow4.replot();
        arrow5.replot();
        arrow6.replot();
        arrow7.replot();
        arrow8.replot();
        arrow9.replot();
        arrow10.replot();
        arrow11.replot();
        arrow12.replot();
        arrow13.replot();
        arrow14.replot();
        arrow15.replot();
        arrow16.replot();
    }

    //for test button
//    Rectangle{
//        color:"green";
//        width: 100;
//        height: 100;
//        anchors.bottom: parent.bottom;
//        anchors.right: parent.right;
//        MouseArea{
//            anchors.fill: parent;
//            onClicked: {
//                console.log("length:");
//            }
//        }
//    }
    QmlFactory{
        id:factory;
        property QTreeWidgetProxy dataTree: null;
        onInitialFactory:{
            dataTree=factory.createQTreeWidgetProxy(treeSource,drivePositionLoop);
        }
    }

    PidControler{
        id:m_pid;
        width: 220;
        height: 80;
        iBlockVisible: false;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.horizontalCenterOffset: -100;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.verticalCenterOffset: -20;
    }
    SegmentArrow{
        id:arrow4;
        points: [Qt.point(0,10),Qt.point(25,10)];
        anchors.left: m_pid.right;
        anchors.verticalCenter: m_pid.verticalCenter;
        aText: "";
    }
    Saturation{
        id:m_saturation;
        width: 60;
        height: 80;
        anchors.left: arrow4.right;
        anchors.verticalCenter: m_pid.verticalCenter;
        anchors.verticalCenterOffset: 15;
        retPenColor: "gray";
        retBorder.color: "#BBB9B9";
        onClicked: {
            m_positionLimit.visible=!m_positionLimit.visible;
        }
    }
    SegmentArrow{
        id:arrow5;
        points: [Qt.point(0,10),Qt.point(25,10)];
        anchors.left: m_saturation.right;
        anchors.verticalCenter: m_pid.verticalCenter;
        aText: "";
    }
    FlowNode{
        id:m_node2;
        width: 40;
        height: 40;
        anchors.left: arrow5.right;
        anchors.verticalCenter: m_pid.verticalCenter;
        penColor: "#BBB9B9";
        brushColor: Qt.lighter(penColor,1.2);
    }
    SegmentArrow{
        id:arrow6;
        points: [Qt.point(0,10),Qt.point(25,10)];
        anchors.left: m_node2.right;
        anchors.verticalCenter: m_pid.verticalCenter;
        aText: "";
    }
    FilterBlock{
        id:m_speedLoop;
        width: 60;
        height: 60;
        name.text: "速度控制器";
        anchors.left: arrow6.right;
        anchors.verticalCenter: m_pid.verticalCenter;
    }
    SegmentArrow{
        id:arrow7;
        points: [Qt.point(0,10),Qt.point(25,10)];
        anchors.left: m_speedLoop.right;
        anchors.verticalCenter: m_pid.verticalCenter;
        aText: "";
    }
    FlowNode{
        id:m_node3;
        width: 40;
        height: 40;
        anchors.left: arrow7.right;
        anchors.verticalCenter: m_pid.verticalCenter;
        penColor: "#BBB9B9";
        brushColor: Qt.lighter(penColor,1.2);
    }
    SegmentArrow{
        id:arrow8;
        points: [Qt.point(0,10),Qt.point(25,10)];
        anchors.left: m_node3.right;
        anchors.verticalCenter: m_pid.verticalCenter;
        aText: "";
    }
    FilterBlock{
        id:m_currentLoop;
        width: 110;
        height: 60;
        name.text: "电流/力矩控制器";
        anchors.left: arrow8.right;
        anchors.verticalCenter: m_pid.verticalCenter;
    }
    SegmentArrow{
        id:arrow9;
        points: [Qt.point(0,10),Qt.point(50,10)];
        anchors.left: m_currentLoop.right;
        anchors.verticalCenter: m_pid.verticalCenter;
        aText: "";
    }
    SegmentArrow{
        id:arrow3;
        points: [Qt.point(0,10),Qt.point(25,10)];
        anchors.right: m_pid.left;
        anchors.verticalCenter: m_pid.verticalCenter;
        aText: "";
    }
    FlowNode{
        id:m_node1;
        width: 40;
        height: 40;
        anchors.right: arrow3.left;
        anchors.verticalCenter: m_pid.verticalCenter;
        penColor: "#BBB9B9";
        brushColor: Qt.lighter(penColor,1.2);
    }
    SegmentArrow{
        id:arrow2;
        points: [Qt.point(0,10),Qt.point(50,10)];
        anchors.right: m_node1.left;
        anchors.verticalCenter: m_pid.verticalCenter;
        aText: "";
    }
    FilterBlock{
        id:m_inputFilter
        width:60;
        height: 60;
        name.text: "输入滤波"
        anchors.right: arrow2.left;
        anchors.verticalCenter: m_pid.verticalCenter;
    }
    SegmentArrow{
        id:arrow1;
        points: [Qt.point(0,10),Qt.point(50,10)];
        anchors.right: m_inputFilter.left;
        anchors.verticalCenter: m_pid.verticalCenter;
        aText: "";
    }
    SegmentArrow{
        id:arrow12;
        points: [Qt.point(10,0),Qt.point(10,155)];
        anchors.bottom: arrow2.top;
        anchors.bottomMargin: -15;
        anchors.horizontalCenter: arrow2.horizontalCenter;
        anchors.horizontalCenterOffset: -10;
        aText: "";
        hasArrow: false;
    }
    SegmentArrow{
        id:arrow13;
        points: [Qt.point(2,10),Qt.point(140,10)];
        anchors.left: arrow12.horizontalCenter;
        anchors.verticalCenter: arrow12.verticalCenter;
        anchors.verticalCenterOffset: -10;
        aText: "";
    }
    LineEditBlock{
        id:m_kgv;
        anchors.left: arrow13.right;
        anchors.verticalCenter: arrow13.verticalCenter;
        title: "速度前馈增益:";
        unit:"%"
    }
    SegmentArrow{
        id:arrow15;
        points: [Qt.point(0,69),Qt.point(137,69),Qt.point(137,138)];
        anchors.left: m_kgv.right;
        anchors.verticalCenter: m_kgv.verticalCenter;
        aText: "+";
        textPosition:Qt.point(points[2].x+10,points[2].y-10);
        hasBounding: false;
        widthExtend: 50;
    }
    SegmentArrow{
        id:arrow14;
        points: [Qt.point(2,10),Qt.point(140,10)];
        anchors.left: arrow12.horizontalCenter;
        anchors.verticalCenter:  arrow12.top;
        aText: "";
    }
    LineEditBlock{
        id:m_kga;
        anchors.left: arrow14.right;
        anchors.verticalCenter: arrow14.verticalCenter;
        title: "加速度前馈增益:";
        unit:"%";
    }
    SegmentArrow{
        id:arrow16;
        points: [Qt.point(0,138),Qt.point(285,138),Qt.point(285,276)];
        anchors.left: m_kga.right;
        anchors.verticalCenter: m_kga.verticalCenter;
        aText: "+";
        textPosition:Qt.point(points[2].x+10,points[2].y-10);
        hasBounding: false;
        widthExtend: 50;
    }
    FilterBlock{
        id:m_realPostion
        width:60;
        height: 60;
        name.text: "位置信号"
        anchors.top: m_pid.bottom;
        anchors.topMargin: 20;
        anchors.horizontalCenter: m_pid.horizontalCenter;
    }
    SegmentArrow{
        id:arrow11;
        points: [Qt.point(135,74),Qt.point(10,74),Qt.point(10,0)];
        anchors.right: m_realPostion.left;
        anchors.bottom: m_realPostion.verticalCenter;
        anchors.bottomMargin: -10;
        aText: "-";
        textPosition:Qt.point(20,20);
        hasBounding: false;
    }
    SegmentArrow{
        id:arrow10;
        points: [Qt.point(250,10),Qt.point(0,10)];
        anchors.left: m_realPostion.right;
        anchors.verticalCenter: m_realPostion.verticalCenter;
        aText: "";
    }
    PositionLimit{
        id:m_positionLimit;
        anchors.top: m_saturation.bottom;
        anchors.topMargin: -10;
        anchors.horizontalCenter: m_saturation.horizontalCenter;
        anchors.horizontalCenterOffset: 0;
        visible: false;
    }
    Text{
        text:"位置指令";
        anchors.bottom: arrow1.top;
        anchors.horizontalCenter: arrow1.horizontalCenter;
    }

    //信号与槽
    Connections{
        target: drivePositionLoop;
        onActiveNow:{
            replot();
        }
        onItemValueChanged:{
            console.log("value change....");
            updateEditValueFromTree();
        }
        function updateEditValueFromTree(){
            m_pid.pEditText=factory.dataTree.textTopLevel(0,1);
            m_positionLimit.absEditText=factory.dataTree.textTopLevel(1,1);
            m_positionLimit.posEditText=factory.dataTree.textTopLevel(2,1);
            m_positionLimit.negEditText=factory.dataTree.textTopLevel(3,1);
            m_kgv.editText=factory.dataTree.textTopLevel(4,1);
            m_kga.editText=factory.dataTree.textTopLevel(5,1);
            m_pid.pEditTextColor="black";
            m_positionLimit.absEditTextColor="black";
            m_positionLimit.posEditTextColor="black";
            m_positionLimit.negEditTextColor="black";
            m_kgv.editTextColor="black";
            m_kga.editTextColor="black";
        }
    }

    Connections{
        target: m_pid;
        onPEditTextChanged: {
            factory.dataTree.setTopLevelText(0,1,m_pid.pEditText);
        }
    }
    Connections{
        target: m_kgv;
        onEditTextChanged: {
            factory.dataTree.setTopLevelText(4,1,m_kgv.editText);
        }
    }
    Connections{
        target: m_kga;
        onEditTextChanged: {
            factory.dataTree.setTopLevelText(5,1,m_kga.editText);
        }
    }
    Connections{
        target: m_positionLimit;
        onAbsEditTextChanged: {
            factory.dataTree.setTopLevelText(1,1,m_positionLimit.absEditText);
        }
        onPosEditTextChanged: {
            factory.dataTree.setTopLevelText(2,1,m_positionLimit.posEditText);
        }
        onNegEditTextChanged: {
            factory.dataTree.setTopLevelText(3,1,m_positionLimit.negEditText);
        }
    }

}

