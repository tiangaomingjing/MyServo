import QtQuick 2.5

Item {
    id:root;
    property alias aWidth: flowNode.aWidth;
    property alias penColor: flowNode.penColor;
    property alias brushColor: flowNode.brushColor;
    function replot(){
        flowNode.requestPaint();
    }
    width: 40;
    height: 40;

    Canvas{
        id:flowNode;
//        width: 50;
//        height: 50;
        anchors.fill: parent;
        property int aWidth: 2;
        property color penColor: "blue";
        property color brushColor: Qt.lighter("gray");
        property bool isMousEnterIn: false;
        onPaint: {
            var ctx = getContext("2d")
            // 设置画笔
            ctx.lineWidth = aWidth;
            ctx.strokeStyle = penColor;
            // 设置填充
            if(isMousEnterIn){

                ctx.fillStyle=Qt.rgba(0.97,0.145,0.145,0.4);
            }
            else{
                ctx.fillStyle = brushColor;
            }

            // 开始绘制路径
            ctx.clearRect(0,0,flowNode.width,flowNode.height);

            ctx.save();
            ctx.beginPath()
            // 移动到左上点作为起始点

            var cx=flowNode.width/2;
            var cy=flowNode.height/2;
            var r=flowNode.width/2-aWidth;
            ctx.arc(cx,cy,r,0,2*Math.PI);
            ctx.stroke();
            ctx.closePath();
            ctx.fill();


            ctx.restore();

            ctx.save();
            var lc=Math.cos(Math.PI/4);
            var ls=Math.sin(Math.PI/4);
            ctx.moveTo(cx+r*lc,cy-r*ls);
            ctx.lineTo(cx-r*lc,cy+r*ls);
            ctx.moveTo(cx+r*lc,cy+r*ls);
            ctx.lineTo(cx-r*lc,cy-r*ls);
            ctx.stroke();
            ctx.restore();
        }
        MouseArea{
            id:m_mouse;
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: {
                console.log("aaaaaaa");
                flowNode.isMousEnterIn=true;
                flowNode.requestPaint();
            }
            onExited: {
                console.log("ddssdsf");
                flowNode.isMousEnterIn=false;
                flowNode.requestPaint();
            }
        }
    }

}

