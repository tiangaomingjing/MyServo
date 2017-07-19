import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtClass 1.0
import "./components/CfgSpeedLoop"
import "./components/FlowCommon"

Rectangle {
    id:root;
    width: 1000;
    height: 600;
    color: "#F0F0F0"
    property color arrowColor: "blue"
    property color arrowBrushColor: "yellow"
    //测试按钮
//    Rectangle{
//        color:"green";
//        width: 100;
//        height: 100;
//        anchors.bottom: parent.bottom;
//        anchors.right: parent.right;
//        MouseArea{
//            anchors.fill: parent;
//            onClicked: {
//                console.log("length:"+arrow1.points.length);
//                arrow7.hasBounding=!arrow7.hasBounding;
//                arrow7.hasArrow=!arrow7.hasArrow;
//                console.log(arrow7.isBoundingShow)
//                root.replot();
//            }
//        }
//    }
    function replot(){
        speedLimit.replot();
        arrow1.replot();
        arrow2.replot();
        arrow3.replot();
        arrow4.replot();
        arrow5.replot();
        arrow6.replot();
        arrow7.replot();
        arrow8.replot();
        node1.replot();
        saturation1.replot();
    }
    QmlFactory{
        id:factory;
        property QTreeWidgetProxy dataTree: null;
        onInitialFactory:{
            dataTree=factory.createQTreeWidgetProxy(treeSource,driveSpeedLoop);
        }
    }
    SpeedLimit{
        id:speedLimit;
        anchors.horizontalCenter: saturation1.horizontalCenter;
        anchors.horizontalCenterOffset: -110;
        anchors.bottom: saturation1.top;
        anchors.bottomMargin: -15;
        visible: false;
        onAbsEditTextChanged: {
            console.log("abs edit changed ....");
            factory.dataTree.setTopLevelText(2,1,absEditText);
        }
        onPosEditTextChanged: {
            console.log("pos edit changed ....");
            factory.dataTree.setTopLevelText(3,1,posEditText);
        }
        onNegEditTextChanged: {
            console.log("neg edit changed ....");
            factory.dataTree.setTopLevelText(4,1,negEditText);
        }
    }
    PidControler{
        id:pid;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.horizontalCenterOffset: -30;
        anchors.verticalCenter: parent.verticalCenter;
        onPEditTextChanged: {
            console.log("PPPPP edit changed ....");
            factory.dataTree.setTopLevelText(0,1,pEditText);
        }
        onIEditTextChanged: {
            console.log("IIII edit changed ....");
            factory.dataTree.setTopLevelText(1,1,iEditText);
        }
    }
    SegmentArrow{
        id:arrow4;
        anchors.left: pid.right;
        anchors.verticalCenter: pid.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor;
        points: [Qt.point(0,10),Qt.point(40,10)];
        aText: " ";
//        textPosition: Qt.point(40,10);
    }
    Saturation{
        id:saturation1;
        width: 100;
        height: 100;
        anchors.left: arrow4.right;
        anchors.verticalCenter: pid.verticalCenter;
        anchors.verticalCenterOffset: 12;
        retPenColor: "gray";
        retBorder.color: "#BBB9B9";
        property bool checked: false;
        onClicked: {
            console.log("saturation clicked");
            checked=!checked;
            if(checked)
                speedLimit.visible=true;
            else speedLimit.visible=false;
        }
    }
    SegmentArrow{
        id:arrow5;
        anchors.left: saturation1.right;
        anchors.verticalCenter: pid.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(0,10),Qt.point(40,10)];
        aText: "";
    }
    FilterBlock{
        id:outFilter;
        anchors.left: arrow5.right;
        anchors.verticalCenter: pid.verticalCenter;
        width: 100;
        height: 100;
        name.text:qsTr("输出滤波器")
    }
    SegmentArrow{
        id:arrow6;
        anchors.left: outFilter.right;
        anchors.verticalCenter: pid.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(0,10),Qt.point(90,10)];
        aText: "";
    }
    Text{
        anchors.bottom: arrow6.top;
        anchors.horizontalCenter:  arrow6.horizontalCenter;
        text:qsTr("速度控制器输出");
    }

    SegmentArrow{
        id:arrow3;
        anchors.right: pid.left;
        anchors.verticalCenter: pid.verticalCenter;
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
        anchors.verticalCenter: pid.verticalCenter;
        penColor: "#BBB9B9";
        brushColor: Qt.lighter(penColor,1.2);
    }
    SegmentArrow{
        id:arrow7;
        anchors.top: node1.bottom;
        anchors.left: node1.horizontalCenter;
        anchors.leftMargin: -10;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(320,100),Qt.point(10,100),Qt.point(10,0)];
        aText: "-";
        widthExtend: 30;
        textPosition: Qt.point(30,20);
    }
    FilterBlock{
        id:realSpeedInput;
        anchors.verticalCenter:  arrow7.bottom;
        anchors.verticalCenterOffset: -5;
        anchors.horizontalCenter: saturation1.horizontalCenter;
        width: 100;
        height: 40;
        name.text:qsTr("速度反馈");
    }
    SegmentArrow{
        id:arrow2;
        anchors.right: node1.left;
        anchors.verticalCenter: pid.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(0,10),Qt.point(40,10)];
        aText: "";
    }

    FilterBlock{
        id:inputFilter;
        anchors.right: arrow2.left;
        anchors.verticalCenter: pid.verticalCenter;
        width: 100;
        height: 100;
        name.text:qsTr("输入滤波器")
    }
    SegmentArrow{
        id:arrow1;
        anchors.right: inputFilter.left;
        anchors.verticalCenter: pid.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(0,10),Qt.point(70,10)];
        aText: "";
    }
    Text{
        anchors.bottom: arrow1.top;
        anchors.horizontalCenter:  arrow1.horizontalCenter;
        text:qsTr("速度指令");
    }
    SegmentArrow{
        id:arrow8;
        anchors.left:  realSpeedInput.right;
        anchors.verticalCenter: realSpeedInput.verticalCenter;
        penColor: arrowColor;
        brushColor: arrowBrushColor
        points: [Qt.point(70,10),Qt.point(0,10)];
        aText: "";
    }

    //信号与槽
    Connections{
        target: driveSpeedLoop;
        onActiveNow:{
            replot();
        }
        onItemValueChanged:{
            console.log("value change....");
            updateEditValueFromTree();
        }
    }
    function updateEditValueFromTree(){
        pid.pEditText=factory.dataTree.textTopLevel(0,1);
        pid.iEditText=factory.dataTree.textTopLevel(1,1);
        speedLimit.absEditText=factory.dataTree.textTopLevel(2,1);
        speedLimit.posEditText=factory.dataTree.textTopLevel(3,1);
        speedLimit.negEditText=factory.dataTree.textTopLevel(4,1);
        pid.pEditTextColor="black";
        pid.iEditTextColor="black";
        speedLimit.absEditTextColor="black";
        speedLimit.posEditTextColor="black";
        speedLimit.negEditTextColor="black";
    }

}

