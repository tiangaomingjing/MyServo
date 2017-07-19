import QtQuick 2.5

Item {
    id:chatDialog;
    width: 500;
    height: 500;

    property color penColor: "#BBB9B9";
    property color brushColor: Qt.lighter("#BBB9B9",1.2);
    property int penWidth: 3;
    property int radius: 30;
    property double hPercent: 0.3;
    property double wPercent: 0.7;
    property double dPercent: 0.1;
    property var controlPoints: [Qt.point(width*(wPercent-0.5*dPercent),(1-hPercent)*height),
                                Qt.point(width*wPercent,height),
                                Qt.point(width*(wPercent+0.5*dPercent),(1-hPercent)*height)]
    function replot(){
        m_canvas.requestPaint();
    }

    Canvas{
        id:m_canvas;
        anchors.fill: parent;
        onPaint: {
//            console.log(controlPoints);
            var ctx=getContext("2d");
            ctx.clearRect(0,0,parent.width,parent.height);

            //设置笔宽
            ctx.lineWidth = parent.penWidth;
            //设置画笔
            ctx.strokeStyle = parent.penColor;
            // 设置填充
            ctx.fillStyle = parent.brushColor;
            // 开始绘制路径
            ctx.beginPath();
            var w=parent.width;
            var h=(1-parent.hPercent)*parent.height;
            var r=parent.radius;
            var a=Math.PI/180;
//            console.log("h"+h+"r"+r);
//            ctx.moveTo(0,r);
//            ctx.lineTo(0,h-r);
//            ctx.arc(r,h-r,r,180*a,90*a,true);
//            ctx.lineTo(controlPoints[0].x,controlPoints[0].y);
//            ctx.lineTo(controlPoints[1].x,controlPoints[1].y);
//            ctx.lineTo(controlPoints[2].x,controlPoints[2].y);

//            ctx.lineTo(w-r,h);
//            ctx.arc(w-r,h-r,r,90*a,0*a,true);
//            ctx.lineTo(w,r);
//            ctx.arc(w-r,r,r,360*a,270*a,true);
//            ctx.lineTo(r,0);
//            ctx.arc(r,r,r,270*a,180*a,true);
            ctx.moveTo(penWidth,r+penWidth);
            ctx.lineTo(penWidth,h-r);
            ctx.arc(r+penWidth,h-r,r,180*a,90*a,true);
            ctx.lineTo(controlPoints[0].x,controlPoints[0].y);
            ctx.lineTo(controlPoints[1].x,controlPoints[1].y);
            ctx.lineTo(controlPoints[2].x,controlPoints[2].y);

            ctx.lineTo(w-r-penWidth,h);
            ctx.arc(w-r-penWidth,h-r,r,90*a,0*a,true);
            ctx.lineTo(w-penWidth,r+penWidth);
            ctx.arc(w-r-penWidth,r+penWidth,r,360*a,270*a,true);
            ctx.lineTo(r+penWidth,penWidth);
            ctx.arc(r+penWidth,r+penWidth,r,270*a,180*a,true);
            ctx.closePath();
            ctx.stroke();
            ctx.fill();

        }
    }

}

