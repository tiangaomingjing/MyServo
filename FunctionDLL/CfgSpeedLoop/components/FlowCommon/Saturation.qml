import QtQuick 2.5
import QtQuick.Layouts 1.1
Item{
    id:saturation;
    width: 100;
    height: 100;
    property alias retPenColor: pic.penColor;
    property alias retBorder: ret.border;

    signal clicked();
    function replot(){
        pic.requestPaint();
    }

    ColumnLayout{
        anchors.fill: parent;
        spacing: 10;
        Rectangle {
            id:ret;
            Layout.minimumWidth: 30;
            Layout.minimumHeight: 30;
//            width: 100;
//            height: 100;
            Layout.fillHeight: true;
            Layout.fillWidth: true;
            color:m_mouse.containsMouse?Qt.rgba(0.97,0.145,0.145,0.5):Qt.lighter(border.color,1.2);
            border.color: "#BBB9B9";
            border.width: 2;
            radius: 10;

            MouseArea{
                id:m_mouse;
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    saturation.clicked();
                }
            }
            Canvas{
                id:pic;
                width: parent.width;
                height: parent.height;

                property int aWidth: 2;
                property color penColor: "black";
                property color brushColor: "yellow";
                onPaint: {
                    var ctx=getContext("2d");
                    ctx.clearRect(0,0,pic.width,pic.height);

                    ctx.lineWidth = aWidth;
                    ctx.strokeStyle = penColor;
                    ctx.fillStyle = brushColor;

                    var cx=width/2;
                    var cy=height/2;
                    var w=width;
                    var h=height;
                    var pa=Qt.point(cx,0);
                    var pb=Qt.point(0,cy);
                    var pc=Qt.point(0,1.5*cy);
                    var pd=Qt.point(0.5*cx,1.5*cy);
                    var pe=Qt.point(cx,h);
                    var pf=Qt.point(w,cy);
                    var pg=Qt.point(w,0.5*cy);
                    var ph=Qt.point(1.5*cx,0.5*cy);

                    ctx.beginPath();
                    ctx.moveTo(pa.x,pa.y);
                    ctx.lineTo(pe.x,pe.y);
                    ctx.moveTo(pb.x,pb.y);
                    ctx.lineTo(pf.x,pf.y);
                    ctx.moveTo(pc.x,pc.y);
                    ctx.lineTo(pd.x,pd.y);
                    ctx.lineTo(ph.x,ph.y);
                    ctx.lineTo(pg.x,pg.y);
                    ctx.stroke();
                }
            }
        }
        Text{
            id:title;
//            anchors.horizontalCenter: parent.horizontalCenter
            text:qsTr("饱和输出控制器")
            Layout.fillWidth: true;
            horizontalAlignment:Text.AlignHCenter
        }
    }
}


//Text{
//    text:qsTr("饱和输出调节器")
//}

