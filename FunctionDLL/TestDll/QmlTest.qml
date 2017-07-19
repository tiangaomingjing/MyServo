import QtQuick 2.0
import QtQuick.Extras 1.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
//CircularGauge {
//    value: accelerating ? maximumValue : 0
//    anchors.centerIn: parent

//    property bool accelerating: false

//    Keys.onSpacePressed: accelerating = true
//    Keys.onReleased: {
//        if (event.key === Qt.Key_Space) {
//            accelerating = false;
//            event.accepted = true;
//        }
//    }

//    Component.onCompleted: forceActiveFocus()

//    Behavior on value {
//        NumberAnimation {
//            duration: 1000
//        }
//    }
//}

Rectangle {
    width: 300
    height: 300
    color: "#F0F0F0"

    CircularGauge {
        id: gauge
//        anchors.fill: parent;
        anchors.top: parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: height;
        height: parent.height*0.8;
        stepSize: 1;

        minimumValue: 0;
        maximumValue: 360;
        value: accelerating?maximumValue:0;
        property bool accelerating: false;

        style:CircularGaugeDarkStyle{}
        Keys.onSpacePressed: accelerating = true
        Keys.onReleased: {
            if (event.key === Qt.Key_Space) {
                accelerating = false;
                event.accepted = true;
            }
        }

        Component.onCompleted: forceActiveFocus()
        Behavior on value {
            NumberAnimation {
                duration: 500
            }
        }
        Rectangle{
            color:"transparent";
            border.color: "white"
            border.width: 1;
            radius: 10;
            width: 2*height;
            height: gauge.height*0.1;
            anchors.horizontalCenter: gauge.horizontalCenter;
            anchors.verticalCenter: gauge.verticalCenter;
            anchors.verticalCenterOffset:-gauge.height/5;
            Text{
                id:m_angleText;
                text:gauge.value;
                anchors.centerIn: parent;
                color: "white"
                font.bold: true;
                font.pixelSize: parent.height*0.9;
            }
        }
    }
    Slider{
        id:controlSlider;
        anchors.top: gauge.bottom;
        anchors.topMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;
        width: parent.width*0.8;
        height: parent.height*0.15;
        stepSize: 1;
        activeFocusOnPress:true;
        maximumValue: 360;
        minimumValue: 0;
//        tickmarksEnabled: true;
        style: SliderStyle {
            groove: Rectangle {
                implicitWidth: controlSlider.width;
                implicitHeight: controlSlider.height*0.2
                color: "gray"
                radius: 8
            }
            handle: Rectangle {
                anchors.centerIn: parent
                color: control.pressed ? "white" : "lightgray"
                border.color: "gray"
                border.width: 2
                implicitWidth: controlSlider.height*0.8
                implicitHeight: implicitWidth
                radius: implicitWidth/2;
                Text{
                    anchors.bottom: parent.top;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    text:controlSlider.value;
                }
            }
        }
        onValueChanged: {
            gauge.value=value;
        }
    }
}


//import QtQuick 2.0
//import QtQuick.Controls 1.4
//import QtQuick.Controls.Styles 1.4
//import QtQuick.Extras 1.4
//import QtQuick.Extras.Private 1.0

//Rectangle {
//    width: 300
//    height: 300
//    color: "gray"
//    radius: width/2;

//    CircularGauge {
//        id: gauge
//        anchors.fill: parent;
//        style: CircularGaugeStyle {
//            id: style

//            function degreesToRadians(degrees) {
//                return degrees * (Math.PI / 180);
//            }

//            background: Canvas {
//                onPaint: {
//                    var ctx = getContext("2d");
//                    ctx.reset();

//                    ctx.beginPath();
//                    ctx.strokeStyle = "#e34c22";
//                    ctx.lineWidth = outerRadius * 0.02;

//                    ctx.arc(outerRadius, outerRadius, outerRadius - ctx.lineWidth / 2,
//                        degreesToRadians(valueToAngle(80) - 90), degreesToRadians(valueToAngle(100) - 90));
//                    ctx.stroke();
//                }
//            }

//            tickmark: Rectangle {
//                visible: styleData.value < 80 || styleData.value % 10 == 0
//                implicitWidth: outerRadius * 0.02
//                antialiasing: true
//                implicitHeight: outerRadius * 0.06
//                color: styleData.value >= 80 ? "#e34c22" : "#e5e5e5"
//            }

//            minorTickmark: Rectangle {
//                visible: styleData.value < 80
//                implicitWidth: outerRadius * 0.01
//                antialiasing: true
//                implicitHeight: outerRadius * 0.03
//                color: "#e5e5e5"
//            }

//            tickmarkLabel:  Text {
//                font.pixelSize: Math.max(6, outerRadius * 0.1)
//                text: styleData.value
//                color: styleData.value >= 80 ? "#e34c22" : "#e5e5e5"
//                antialiasing: true
//            }

//            needle: Rectangle {
//                y: outerRadius * 0.15
//                implicitWidth: outerRadius * 0.03
//                implicitHeight: outerRadius * 0.9
//                antialiasing: true
//                color: "#e5e5e5"
//            }

//            foreground: Item {
//                Rectangle {
//                    width: outerRadius * 0.2
//                    height: width
//                    radius: width / 2
//                    color: "#e5e5e5"
//                    anchors.centerIn: parent
//                }
//            }
//        }
//    }
//}
