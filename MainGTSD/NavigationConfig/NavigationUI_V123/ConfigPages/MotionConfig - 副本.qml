import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

Item {
    id:motionConfig;
    objectName: "MotionConfig";
    signal startTimer();

    function prevExecuteCommand(){
        emit:startTimer();
        console.log("MotionConfig prev executeCommand");
    }

    function executeCommand(){
        console.log("MotionConfig executeCommand");
        return true;
    }
    function aHeadExecuteCommand(){
        console.log("MotionConfig aHeadExecuteCommand");
    }

    Item{
        anchors.centerIn: parent;
        width: parent.width*0.7;
        height: parent.height;
        ColumnLayout{
            anchors.fill: parent;
            anchors.margins: 10;
            spacing: 10;
            CheckBox{
                text:"往复运动";
                checked: false;
                style: CheckBoxStyle {
                        indicator: Rectangle {
                                implicitWidth: 24
                                implicitHeight: 24
                                radius: 3;
                                color:"transparent"
                                border.color: control.activeFocus ? "darkblue" : "gray"
                                border.width: 1
                                Canvas{
                                    anchors.fill: parent;
                                    anchors.margins: parent.width*0.1;
                                    visible: control.checked
                                    onPaint: {
                                        var ctx=getContext("2d");
                                        ctx.lineWidth=2;
                                        ctx.strokeStyle="darkblue";
                                        ctx.beginPath();
                                        ctx.moveTo(0,height/2);
                                        ctx.lineTo(width/2,height-2);
                                        ctx.lineTo(width,0);
                                        ctx.stroke();
                                    }
                                }
                        }
                        label:Text{
                            text:control.text;
                            font.pixelSize: 16;
                        }
                    }
            }
            RowLayout{
                spacing: 10;
                Text{
                    text:"目标位置:"
                    font.pixelSize: 16;
                }
                TextField{
                    id:m_targetPostion;
                    Layout.fillWidth: true;
                    style:m_motionInputTextFieldStyle;
                    font.pixelSize: 16;
                }
                Text{
                    text:"count"
                    font.pixelSize: 16;
                }
            }
            RowLayout{
                spacing: 10;
                Text{
                    text:"目标速度:"
                    font.pixelSize: 16;
                }
                TextField{
                    id:m_targetVelocity;
                    Layout.fillWidth: true;
                    style:m_motionInputTextFieldStyle;
                    font.pixelSize: 16;
                }
                Text{
                    text:"转每分钟"
                    font.pixelSize: 16;
                }
            }
            RowLayout{
                spacing: 10;
                Text{
                    text:"目标加速度:"
                    font.pixelSize: 16;
                }
                TextField{
                    id:m_targetAcceleration;
                    Layout.fillWidth: true;
                    style:m_motionInputTextFieldStyle;
                    font.pixelSize: 16;
                }
                Text{
                    text:"rps/s"
                    font.pixelSize: 16;
                }
            }
            Item{
                Layout.fillWidth: true;
                height: 20;
                Text{
                    anchors.left: parent.left;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.verticalCenterOffset: 0;
                    text:"柔性"
                    font.pixelSize: 16;
                }
                Text{
                    anchors.right:  parent.right;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.verticalCenterOffset: 0;
                    text:"刚性"
                    font.pixelSize: 16;
                }
            }

            Slider{
                Layout.fillWidth: true;
                minimumValue: 0.1;
                maximumValue: 1;
                stepSize: 0.05;
                value: 0.5;
                tickmarksEnabled: true;
                style:m_motionInputSliderStyle;
            }
            RowLayout{
                Layout.fillWidth: true;
                Rectangle{
                    Layout.fillWidth: true;
                    height:50;
                    color:m_btnMotionSetMouse.containsPress?Qt.darker("yellow"):
                          m_btnMotionSetMouse.containsMouse?Qt.lighter("yellow",1.3):"transparent";
                    border.color: "#BBB9B9";
                    border.width: 1;
                    radius: 5;
                    Text{
                        anchors.centerIn: parent;
                        text:"设置参数";
                        font.letterSpacing:5;
                        color:"black";
                        font.bold: true;
                        font.pixelSize: 16;
                    }
                    MouseArea{
                        id:m_btnMotionSetMouse;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            console.log("connect clicked");
                            m_btnMotionStart.enabled=true;
                            m_btnMotionStop.enabled=true;
                        }
                    }
                }
                Rectangle{
                    id:m_btnMotionStart;
                    enabled: false;
                    Layout.fillWidth: true;
                    height:50;
                    color:m_btnMotionStartMouse.containsPress?Qt.darker("yellow"):
                          m_btnMotionStartMouse.containsMouse?Qt.lighter("yellow",1.3):"transparent";
                    border.color: "#BBB9B9";
                    border.width: 1;
                    radius: 5;
                    Text{
                        anchors.centerIn: parent;
                        text:"开始";
                        font.letterSpacing:5;
                        color:parent.enabled?"black":"gray";
                        font.bold: true;
                        font.pixelSize: 16;
                    }
                    MouseArea{
                        id:m_btnMotionStartMouse;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            console.log("connect clicked");
                        }
                    }
                }
                Rectangle{
                    id:m_btnMotionStop;
                    enabled: false;
                    Layout.fillWidth: true;
                    height:50;
                    color:m_btnMotionStopMouse.containsPress?Qt.darker("yellow"):
                          m_btnMotionStopMouse.containsMouse?Qt.lighter("yellow",1.3):"transparent";
                    border.color: "#BBB9B9";
                    border.width: 1;
                    radius: 5;
                    Text{
                        anchors.centerIn: parent;
                        text:"停止";
                        font.letterSpacing:5;
                        color:parent.enabled?"black":"gray";
                        font.bold: true;
                        font.pixelSize: 16;
                    }
                    MouseArea{
                        id:m_btnMotionStopMouse;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            console.log("connect clicked");
                        }
                    }
                }
            }
        }
    }

    //输入框样式
    Component{
        id:m_motionInputTextFieldStyle;
        TextFieldStyle{
            textColor: "black";
            background: Rectangle{
                radius: 4;
                implicitWidth: 100;
                implicitHeight: 24;
                border.color: "#333";
                border.width: 1;
            }
        }
    }
    Component{
        id:m_motionInputSliderStyle;
        SliderStyle{
            handle: Rectangle{
                anchors.centerIn: parent;
                color: control.pressed?"yellow":"lightgray";
                border.color: "gray";
                border.width: 2;
                width: 34;
                height: 34;
                radius: 17;
                Text{
                    anchors.bottom: parent.top;
                    anchors.bottomMargin: 10;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    text:qsTr("%1").arg(control.value);
                    color:"black";
                    font.pixelSize: 16;
                }
            }
        }
    }

}

