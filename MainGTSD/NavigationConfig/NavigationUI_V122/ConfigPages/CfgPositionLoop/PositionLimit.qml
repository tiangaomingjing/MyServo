import QtQuick 2.5
import "../FlowCommon"

Item {
    id:speedLimit;
    width: 600;
    height: 250;
    property alias editText1: edit1.editText;
    property alias editText1Color: edit1.editTextColor;
    property alias editText2: edit2.editText;
    property alias editText2Color: edit2.editTextColor;
    property alias editText3: edit3.editText;
    property alias editText3Color: edit3.editTextColor;
    property color arrowColor: "blue";
    property color arrowBrushColor: "yellow";
    function replot(){
        background.replot();
        arrow1.replot();
        arrow2.replot();
        arrow3.replot();
        arrow4.replot();
    }

    ChatDialog{
        id:background;
        anchors.fill: parent;
        anchors.topMargin: 40;
        anchors.bottomMargin: 40;
        anchors.leftMargin: 20;
        anchors.rightMargin: 20;
        hPercent: 0.2;
        wPercent: 0.3;
        dPercent:0.05;
        radius: 20;
        rotation: 0;
//        brushColor: "green"
//        penColor: "red"
    }
    LineEditBlock{
        id:edit1;
        title: "最大速度(%):"
        unit: ""
        anchors.left: parent.left;
        anchors.leftMargin: parent.width*0.08;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.verticalCenterOffset: -background.hPercent*speedLimit.height/2;
//        onValueChanged: {
//            speedLimit.absValueChanged(value);
//        }
    }
    LineEditBlock{
        id:edit2;
        title:"正向速度缩放"
        unit: "%";
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: parent.width*0.1;
        anchors.top: parent.top;
        anchors.topMargin: parent.height*0.20;
//        onValueChanged: {
//            speedLimit.posValueChanged(value);
//        }
    }
    LineEditBlock{
        id:edit3;
        title:"反向速度缩放";
        unit: "%";
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: parent.width*0.1;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin:  parent.height*(0.15+background.hPercent);
//        onValueChanged: {
//            speedLimit.negValueChanged(value);
//        }
    }
    SegmentArrow{
        id:arrow1;
        aText: " "
        anchors.left: edit1.right;
        anchors.bottom: edit1.verticalCenter;
        anchors.bottomMargin: -heightExtend-aLength/2;
//        hasBounding: true;
        heightExtend: 10;
        brushColor: arrowBrushColor
        penColor: arrowColor;

    }
    SegmentArrow{
        id:arrow2;
        aText: " "
        anchors.horizontalCenter: arrow1.horizontalCenter;
        anchors.top: arrow1.bottom;
        anchors.topMargin: -arrow1.heightExtend-arrow1.aLength/2;
//        hasBounding: true;
        heightExtend: 0;
        widthExtend: 0;
        brushColor: arrowBrushColor
        penColor: arrowColor;
    }
    SegmentArrow{
        id:arrow3;
        aText: " "
        anchors.verticalCenter: edit2.verticalCenter;
        anchors.left: edit2.right;
//        hasBounding: true;
        heightExtend: 0;
        widthExtend: 0;
        points: [Qt.point(0,10),Qt.point(50,10)]
        brushColor: arrowBrushColor
        penColor: arrowColor;
    }
    SegmentArrow{
        id:arrow4;
        aText: " "
        anchors.verticalCenter: edit3.verticalCenter;
        anchors.left: edit3.right;
//        hasBounding: true;
        heightExtend: 0;
        widthExtend: 0;
        points: [Qt.point(0,10),Qt.point(50,10)]
        brushColor: arrowBrushColor
        penColor: arrowColor;
    }
    Text{
        text:"正方向最大值"
        anchors.left: arrow3.right;
        anchors.verticalCenter: arrow3.verticalCenter;
    }
    Text{
        text:"反方向最大值"
        anchors.left: arrow4.right;
        anchors.verticalCenter: arrow4.verticalCenter;
    }

    Component.onCompleted: {
        var points=new Array;
        for(var i=0;i<7;i++)
            points.push(Qt.point(0,0));
        var height=Math.abs(edit1.y+edit1.height/2-(edit2.y+edit2.height/2));
        var width=Math.abs(edit1.x+edit1.width-edit2.x);
        points[0].x=0;
        points[0].y=height+arrow1.heightExtend;
        points[1].x=width/2;
        points[1].y=height+arrow1.heightExtend;
        points[2].x=width/2;
        points[2].y=0+arrow1.heightExtend;
        points[3].x=width;
        points[3].y=0+arrow1.heightExtend;
        arrow1.points[0]=points[0];
        arrow1.points[1]=points[1];
        arrow1.points[2]=points[2];
        arrow1.points[3]=points[3];

        points[4].x=width/2;
        points[4].y=0;
        points[5].x=width/2;;
        points[5].y=height;
        points[6].x=width;
        points[6].y=height;
        arrow2.points[0]=points[4];
        arrow2.points[1]=points[5];
        arrow2.points[2]=points[6];

    }

}

