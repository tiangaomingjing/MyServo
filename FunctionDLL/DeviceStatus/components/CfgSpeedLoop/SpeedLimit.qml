import QtQuick 2.5
import "../FlowCommon"

Item {
    id:speedLimit;
    width: 600;
    height: 250;
    property alias absEditText: abs.editText;
    property alias absEditTextColor: abs.editTextColor;
    property alias posEditText: pos_rat.editText;
    property alias posEditTextColor: pos_rat.editTextColor;
    property alias negEditText: neg_rat.editText;
    property alias negEditTextColor: neg_rat.editTextColor;
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
        anchors.margins: 20;
        hPercent: 0.2;
        wPercent: 0.7;
        dPercent:0.1;
        radius: 20;
//        brushColor: "green"
//        penColor: "red"
    }
    LineEditBlock{
        id:abs;
        title: "最大扭矩:"
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
        id:pos_rat;
        title:"正向扭矩缩放"
        unit: "%";
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: parent.width*0.07;
        anchors.top: parent.top;
        anchors.topMargin: parent.height*0.13;
//        onValueChanged: {
//            speedLimit.posValueChanged(value);
//        }
    }
    LineEditBlock{
        id:neg_rat;
        title:"反向扭矩缩放";
        unit: "%";
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: parent.width*0.07;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin:  parent.height*(0.1+background.hPercent);
//        onValueChanged: {
//            speedLimit.negValueChanged(value);
//        }
    }
    SegmentArrow{
        id:arrow1;
        aText: " "
        anchors.left: abs.right;
        anchors.bottom: abs.verticalCenter;
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
        anchors.verticalCenter: pos_rat.verticalCenter;
        anchors.left: pos_rat.right;
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
        anchors.verticalCenter: neg_rat.verticalCenter;
        anchors.left: neg_rat.right;
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
        var height=Math.abs(abs.y+abs.height/2-(pos_rat.y+pos_rat.height/2));
        var width=Math.abs(abs.x+abs.width-pos_rat.x);
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

