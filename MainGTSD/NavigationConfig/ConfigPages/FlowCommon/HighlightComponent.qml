import QtQuick 2.5

Rectangle {
    id:highlight;
    width:100;
    height: 100;
    color:m_mouse.containsMouse?Qt.rgba(0.97,0.145,0.145,0.4):"transparent";
    border.color: m_mouse.containsMouse?"red":"transparent";
    border.width: 2;
    radius: 2;
    transformOrigin: Item.Top;
    property Component como: null;
    MouseArea{
        id:m_mouse;
        anchors.fill: parent;
        hoverEnabled: true;
    }
    function setPosition(startPoint ,endPoint,width){
        highlight.x=startPoint.x-width/2;
        highlight.y=startPoint.y;
        highlight.width=width;
        highlight.height=segmentLength(startPoint,endPoint);
        highlight.rotation=57.3*angle(startPoint,endPoint);
//        console.log("Angle:"+highlight.rotation);
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
        var ang=Math.PI*1.5-Math.acos(temp);
//        console.log("acos()"+Math.acos(temp)*57.3)
        if(p2.y-p1.y>=0)
            ang=Math.PI*1.5+Math.acos(temp);
        return ang;
    }
    Component.onDestruction: {
//        console.log("highlightComponent release ->");
    }

}

