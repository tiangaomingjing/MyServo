import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
Item{
    id:root;
    property alias points: segmentArrow.points;
    property alias aWidth: segmentArrow.aWidth;
    property alias aLength: segmentArrow.aLength;//箭头长度
    property alias penColor: segmentArrow.penColor;
    property alias brushColor: segmentArrow.brushColor;
    property alias  widthExtend: segmentArrow.widthExtend;
    property alias  heightExtend: segmentArrow.heightExtend;
    property alias aText: segmentArrow.aText;
    property alias textPosition: segmentArrow.textPosition;
    property alias hasBounding: segmentArrow.hasBounding;
    property alias hasArrow: segmentArrow.hasArrow;
    width: segmentArrow.width;
    height: segmentArrow.height;

    function replot(){
        segmentArrow.requestPaint();
        segmentArrow.updateInterationComponent();
    }

    Canvas {
        id:segmentArrow;
        width: 500;
        height:500;
        Layout.fillWidth: true;
        Layout.fillHeight: true;
        property string aText: "+";
//        property int textOffsetDis: 20;
//        property int textExtend: 20;
        property var textPosition: Qt.point(10,10);
        property int aLength: 10;//箭头长度
        property int aWidth: 2;
        property color penColor: "blue";
        property color brushColor: "yellow";
//        property var points: new Array;
//        property var points: [Qt.point(10,150),Qt.point(150,150),Qt.point(160,0),Qt.point(100,100)];
        property var points: [Qt.point(100,250),Qt.point(100,50)];
        property var interationComponents: new Array;
        property Component iComponent: null;
        property int  widthExtend: 0;
        property int  heightExtend: 0;
        property bool hasBounding: false;
        property bool hasArrow: true;

        //bounding
        Component{
            id:bounding;
            Rectangle{
                anchors.fill: parent;
                border.color: m_mouse.containsMouse?"black":"transparent"
                color:"transparent"
                MouseArea{
                    id:m_mouse
                    anchors.fill: parent;
                    hoverEnabled: hasBounding;
                }
            }
        }
        Loader{
            id:boundingLoader;
            anchors.fill: parent;
        }

        onPaint: {
            var ctx=getContext("2d");
            ctx.clearRect(0,0,segmentArrow.width,segmentArrow.height);

            //设置笔宽
            ctx.lineWidth = aWidth;
            //设置画笔
            ctx.strokeStyle = penColor;
            // 设置填充
            ctx.fillStyle = brushColor;
            // 开始绘制路径
            ctx.beginPath()
            // 移动到左上点作为起始点
            if(points.length==0) return;
            ctx.moveTo(points[0].x,points[0].y);
            for(var i=0;i<points.length;i++){
                ctx.lineTo(points[i].x,points[i].y);
            }
            ctx.stroke();
            ctx.closePath();

            //draw arrow
            if(hasArrow){
                var arrayLength=points.length;
//                console.log("-------------points length :"+arrayLength);
                if(arrayLength<2)
                    return;
                var lastPoint=points[arrayLength-1];
                var secLastPoint=points[arrayLength-2];
                var rangle=angle(secLastPoint,lastPoint);
                var arrowP1=Qt.point(0,0);
                var arrowP2=Qt.point(0,0);
        //        arrowP1=lastPoint+Qt.point(Math.sin(rangle-Math.PI/3)*aLength,
        //                                   Math.cos(rangle-Math.PI/3)*aLength);
        //        arrowP2=lastPoint+Qt.point(Math.sin(rangle-Math.PI+Math.PI/3)*aLength,
        //                                   Math.cos(rangle-Math.PI+Math.PI/3)*aLength);
                arrowP1.x=lastPoint.x+Math.sin(rangle-Math.PI/3)*aLength;
                arrowP1.y=lastPoint.y+Math.cos(rangle-Math.PI/3)*aLength;
                arrowP2.x=lastPoint.x+Math.sin(rangle-Math.PI+Math.PI/3)*aLength;
                arrowP2.y=lastPoint.y+Math.cos(rangle-Math.PI+Math.PI/3)*aLength;

//                console.log("P1 "+arrowP1+" P2 "+arrowP2);

                ctx.beginPath()
                ctx.moveTo(lastPoint.x,lastPoint.y);
                ctx.lineTo(arrowP1.x,arrowP1.y);
                ctx.lineTo(arrowP2.x,arrowP2.y);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
            }

            //draw text

//            var point=Qt.point(0,0);
//            var length=textExtend;
//            point.x=lastPoint.x-length*Math.cos(rangle);
//            point.y=lastPoint.y-length*Math.sin(rangle);
//            console.log("point "+point);
            ctx.font="28px sans-serif";
            ctx.beginPath();

            ctx.text(aText,textPosition.x,textPosition.y);
//            ctx.text(aText,point.x+Math.sin(rangle)*textOffsetDis,point.y+textOffsetDis*Math.cos(rangle));
            ctx.stroke();
            ctx.fill();

//            console.log("finish arrow");


        }
        onHasBoundingChanged: {
            if(hasBounding){
                boundingLoader.sourceComponent=bounding;//生成组件
            }
            else{
                boundingLoader.sourceComponent=undefined;//销毁组件
            }
        }
        onHasArrowChanged: {
            requestPaint();
        }

        Component.onCompleted: {
            updateInterationComponent();
            setCanvasSize();
            if(hasBounding){
                boundingLoader.sourceComponent=bounding;
            }
        }

        function segmentLength(p1,p2){
            var L1=Math.abs(p1.x-p2.x);
            var L2=Math.abs(p1.y-p2.y);
            var temp=L1*L1+L2*L2;
            return Math.sqrt(temp);
        }
        function angle(p1,p2){
            var dx=p2.x-p1.x;
            var temp=dx/segmentLength(p1,p2);
            var ang=Math.acos(temp);
            if(p2.y-p1.y>=0)
                ang=(2*Math.PI)-ang;
            return ang;
        }
        //动态生成组件
        function createInterationComponent(startPoint,endPoint){
            if(segmentArrow.iComponent==null){
                segmentArrow.iComponent=Qt.createComponent("HighlightComponent.qml");
//                console.log("component create .....");
            }
            var compo;
            if(segmentArrow.iComponent.status==Component.Ready){
                compo=segmentArrow.iComponent.createObject(segmentArrow,
                                                           {
                                                           });
                compo.setPosition(startPoint,endPoint,segmentArrow.aLength);
//                console.log("********var compo create ******");
                var len=segmentArrow.interationComponents.length;
                segmentArrow.interationComponents[len]=compo;//往数组容器里增加动态创建的对象
            }
        }
        //根据输入点更新highlightComponent
        function updateInterationComponent(){
            //先清零
            while(segmentArrow.interationComponents.length>0){
                var deleteComponent=segmentArrow.interationComponents.splice(-1,1);
                deleteComponent[0].destroy();
            }
            for(var i=0;i<segmentArrow.points.length-1;i++){
                createInterationComponent(segmentArrow.points[i],segmentArrow.points[i+1]);
            }
        }
        function setCanvasSize(){
            var minX;
            var maxX;
            var minY;
            var maxY;
            if(segmentArrow.points.length==0) return;
            minX=maxX=segmentArrow.points[0].x;
            minY=maxY=segmentArrow.points[0].y;
            for(var i=1 ;i<segmentArrow.points.length;i++ ){
                if(minX>segmentArrow.points[i].x)
                    minX=segmentArrow.points[i].x;
                if(maxX<segmentArrow.points[i].x)
                    maxX=segmentArrow.points[i].x;
                if(minY>segmentArrow.points[i].y)
                    minY=segmentArrow.points[i].y;
                if(maxY<segmentArrow.points[i].y)
                    maxY=segmentArrow.points[i].y;
            }
            segmentArrow.width=maxX+segmentArrow.aLength/2+widthExtend;
            segmentArrow.height=maxY+segmentArrow.aLength/2+heightExtend;
//            console.log(qsTr("minx:%1,maxX:%2,minY:%3,maxY:%4").arg(minX).arg(maxX).arg(minY).arg(maxY));
//            console.log("width:"+segmentArrow.width+"  height:"+segmentArrow.height);
        }

    }
}



