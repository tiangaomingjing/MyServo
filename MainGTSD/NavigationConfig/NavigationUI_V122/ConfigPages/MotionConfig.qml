import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

Item {
    id:motionConfig;
    objectName: "MotionConfig";
    property bool btnExclusicveFlag: true;//开始与停止的互斥操作
    property bool pageActived: false;//为了处理初始化时slider valueChanged一开始就读取了电机参数
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
        pageActived=true;
    }
    //运行控制定时器
    Timer{
        id:m_tryRunNoneCircleTimer;
        interval: 5000;
        repeat:false;
        triggeredOnStart: false;
        onTriggered: {
            mg_servoCmd.setSpeedRef(0);
            mg_servoCmd.delayMs(500);
            mg_servoCmd.setServoOn(false);
            console.log("times up.....");
            btnExclusicveFlag=true;
        }
    }
    Timer{
        id:m_tryRunCircleTimer;
        interval: 10;
        repeat:true;
        triggeredOnStart: false;
        property int circleTimeCount: 0;
        property bool triggeredFlag: false;

        onTriggered: {
            var totalCicle=0;
            totalCicle=parseInt(m_runCicleTimeInputSeq.text);
            var circleVelValue=0.0;
            circleVelValue=parseFloat(m_ampVelocityInputSeq.text);
            console.log(qsTr("totalCircle:%1").arg(totalCicle))
            console.log(qsTr("circleVelValue:%1").arg(circleVelValue))
            console.log(qsTr("circleTimeCount:%1").arg(circleTimeCount));
            circleTimeCount++;
            triggeredFlag=!triggeredFlag;
            console.log("circle times up..... "+triggeredFlag);
            if(triggeredFlag){
                mg_servoCmd.setSpeedRef(-circleVelValue);
            }
            else{
                mg_servoCmd.setSpeedRef(circleVelValue);
            }

            if(circleTimeCount>=totalCicle){
                mg_servoCmd.setSpeedRef(0);
                m_tryRunCircleTimer.stop();
                mg_servoCmd.delayMs(500);
                mg_servoCmd.setServoOn(false);
                circleTimeCount=0;
                btnExclusicveFlag=true;
            }
        }
    }
//    Timer{
//        id:m_runDelayTimer;
//        interval: 10;
//        repeat:false;
//        triggeredOnStart: false;
//        property bool timeOutFlag: false;
//        onTriggered: {
//            timeOutFlag=true;
//            console.log("delay time"+timeOutFlag)
//        }
//    }

    Item{
        anchors.centerIn: parent;
        width: parent.width*0.8;
        height: parent.height;
        ColumnLayout{
            anchors.fill: parent;
            anchors.margins: 0;
            spacing: 5;
            CheckBox{
                id:m_checkBoxCircle;
                text:"往复运动";
                checked: false;
                enabled: btnExclusicveFlag;
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
            Item{
                id:m_noneCircleItem;
                visible: !(m_checkBoxCircle.checked);
                Layout.fillWidth: true;
                height:200;
                enabled: btnExclusicveFlag;
                Image{
                    id:imgNoseq;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    width: 500;
                    height: 250;
                    fillMode: Image.Stretch
                    source: "./Resource/Icon/common/wavenoseq.png";
                }
                Row{
                    anchors.left: imgNoseq.left;
                    anchors.leftMargin: -70;
                    anchors.top: imgNoseq.top;
                    anchors.topMargin: 60;
                    spacing: 10;
                    Text{
                        text:qsTr("速度幅值");
                        height:24;
                        font.pixelSize: 16;
                        verticalAlignment: Text.AlignVCenter
                    }
                    TextField{
                        id:m_ampVelocityInputNoSeq;
                        style: m_motionInputTextFieldStyle;
                        text:"10";
                    }
                    Text{
                        text:qsTr("%");
                        height:24;
                        font.pixelSize: 16
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Row{
                    anchors.horizontalCenter: imgNoseq.horizontalCenter;
                    anchors.horizontalCenterOffset: 10;
                    anchors.bottom: imgNoseq.bottom;
                    anchors.bottomMargin: 40;
                    spacing: 10;
                    Text{
                        text:qsTr("试运行时间");
                        height:24;
                        font.pixelSize: 16
                        verticalAlignment: Text.AlignVCenter
                    }
                    TextField{
                        id:m_runTimeInputNoSeq;
                        style: m_motionInputTextFieldStyle;
                        text:"5";
                    }
                    Text{
                        text:qsTr("s");
                        height:24;
                        font.pixelSize: 16
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
            Item{
                id:m_circleItem;
                visible: m_checkBoxCircle.checked;
                Layout.fillWidth: true;
                height:200;
                enabled: btnExclusicveFlag;
                Image{
                    id:imgSeq;
                    width: 500;
                    height: 250;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    source: "./Resource/Icon/common/waveseq.png";
                }
                Row{
                    anchors.left: imgSeq.left;
                    anchors.leftMargin: -60;
                    anchors.top: imgSeq.top;
                    anchors.topMargin: 30;
                    spacing: 10;
                    Text{
                        text:qsTr("速度幅值");
                        height:24;
                        font.pixelSize: 16;
                        verticalAlignment: Text.AlignVCenter
                    }
                    TextField{
                        id:m_ampVelocityInputSeq;
                        style: m_motionInputTextFieldStyle;
                        text:"10";
                    }
                    Text{
                        text:qsTr("%");
                        height:24;
                        font.pixelSize: 16
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Row{
                    anchors.horizontalCenter: imgSeq.horizontalCenter;
                    anchors.horizontalCenterOffset: -100;
                    anchors.bottom: imgSeq.bottom;
                    anchors.bottomMargin: 75;
                    spacing: 10;
                    Text{
                        text:qsTr("周期T");
                        height:24;
                        font.pixelSize:16
                        verticalAlignment: Text.AlignVCenter
                    }
                    TextField{
                        id:m_runTimeInputSeq;
                        style: m_motionInputTextFieldStyle;
                        text:"400"
                    }
                    Text{
                        text:qsTr("ms");
                        height:24;
                        font.pixelSize:16
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Row{
                    anchors.horizontalCenter: imgSeq.horizontalCenter;
                    anchors.horizontalCenterOffset: 10;
                    anchors.bottom: imgSeq.bottom;
                    anchors.bottomMargin: 20;
                    spacing: 10;
                    Text{
                        text:qsTr("循环次数:");
                        height:24;
                        font.pixelSize:16
                        verticalAlignment: Text.AlignVCenter
                    }
                    TextField{
                        id:m_runCicleTimeInputSeq;
                        style: m_motionInputTextFieldStyle;
                        text:"10";
                    }
                    Text{
                        text:qsTr("次");
                        height:24;
                        font.pixelSize:16
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
            Item{
                Layout.fillWidth: true;
                height: 24;
                Text{
                    anchors.left: parent.left;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.verticalCenterOffset: 0;
                    height:24;
                    text:"柔性"
                    font.pixelSize: 16;
                }
                Text{
                    anchors.right:  parent.right;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.verticalCenterOffset: 0;
                    height:24;
                    text:"刚性"
                    font.pixelSize: 16;
                }
            }

            Slider{
                Layout.fillWidth: true;
                minimumValue: 0.1;
                maximumValue: 1;
                stepSize: 0.01;
                value: 0.5;
                tickmarksEnabled: true;
                style:m_motionInputSliderStyle;
                enabled: btnExclusicveFlag;
                onValueChanged: {
                    console.log(qsTr("value change:%1").arg(value));
                    //先从fram(铁电)中读取当前的增益，(位置与速度的P) AdvanceFastCommand中找到偏移地址
                    //写到RAM中
                    //再将当前的增益乘以现在设置的百分比
                    if(pageActived){
                        var posP=0;
                        var velP=0;
                        posP=mg_servoCmd.readAdvanceFlash("gSevDrv.sev_obj.pos.ctl.prm.fn_fst_6");
                        velP=mg_servoCmd.readAdvanceFlash("gSevDrv.sev_obj.vel.ctl.prm.fn_fst_5");
                        var pgain=mg_servoCmd.getKgian("gSevDrv.sev_obj.pos.ctl.prm.fn_fst_6");
                        var vgain=mg_servoCmd.getKgian("gSevDrv.sev_obj.vel.ctl.prm.fn_fst_5");

                        var posPset=0.0;
                        posPset=posP*value/pgain;
                        var velPset=0.0;
                        velPset=velP*value/vgain;
                        var ret1=" ";
                        ret1=mg_servoCmd.writeCommand("gSevDrv.sev_obj.pos.ctl.prm.fn_fst_6",posPset);
                        var ret2=" ";
                        ret2=mg_servoCmd.writeCommand("gSevDrv.sev_obj.vel.ctl.prm.fn_fst_5",velPset);
                        console.log(qsTr("posP:%1,velP:%2").arg(posP).arg(velP));
                        console.log(qsTr("posPset:%1,velPset:%2").arg(posPset).arg(velPset));
                        console.log(qsTr("pos ret:%1,vel ret:%2").arg(ret1).arg(ret2));
                        console.log(qsTr("pos gain:%1,vel gain:%2").arg(pgain).arg(vgain));
                    }
                }
            }

            RowLayout{
                Layout.fillWidth: true;
//                Rectangle{
//                    Layout.fillWidth: true;
//                    height:50;
//                    color:m_btnMotionSetMouse.containsPress?Qt.darker("yellow"):
//                          m_btnMotionSetMouse.containsMouse?Qt.lighter("yellow",1.3):"transparent";
//                    border.color: "#BBB9B9";
//                    border.width: 1;
//                    radius: 5;
//                    Text{
//                        anchors.centerIn: parent;
//                        text:"设置参数";
//                        font.letterSpacing:5;
//                        color:"black";
//                        font.bold: true;
//                        font.pixelSize: 16;
//                    }
//                    MouseArea{
//                        id:m_btnMotionSetMouse;
//                        anchors.fill: parent;
//                        hoverEnabled: true;
//                        onClicked: {
//                            console.log("connect clicked");
//                            m_btnMotionStart.enabled=true;
//                            m_btnMotionStop.enabled=true;
//                        }
//                    }
//                }
                Rectangle{
                    id:m_btnMotionStart;
                    enabled: btnExclusicveFlag;
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
                            btnExclusicveFlag=false;
                            //周期循环运动
                            if(m_checkBoxCircle.checked){
                                console.log("circle ....");
                                var circleVelValue=0.0;
                                circleVelValue=parseFloat(m_ampVelocityInputSeq.text);
                                var circleTime=0.0;
                                circleTime=parseFloat(m_runTimeInputSeq.text);
                                mg_servoCmd.setServoTaskMode(6);
                                mg_servoCmd.setSpeedRef(0);
                                mg_servoCmd.setServoOn(true);
                                mg_servoCmd.delayMs(500);
                                m_tryRunCircleTimer.interval=circleTime;
                                m_tryRunCircleTimer.repeat=true;
                                m_tryRunCircleTimer.circleTimeCount=0;
                                m_tryRunCircleTimer.triggeredFlag=false;
                                m_tryRunCircleTimer.start();
                                mg_servoCmd.setSpeedRef(circleVelValue);

                            }
                            //非周期运动
                            else{
                                console.log("none circle ....");
                                var velValue=0.0;
                                velValue=parseFloat(m_ampVelocityInputNoSeq.text);
                                var delayTime=0.0;
                                delayTime=parseFloat(m_runTimeInputNoSeq.text);
                                mg_servoCmd.setServoTaskMode(6);
                                mg_servoCmd.setSpeedRef(0);
                                mg_servoCmd.setServoOn(true);
                                mg_servoCmd.delayMs(500);
                                m_tryRunNoneCircleTimer.interval=delayTime*1000;
                                m_tryRunNoneCircleTimer.repeat=false;
                                m_tryRunNoneCircleTimer.start();
                                mg_servoCmd.setSpeedRef(velValue);
                            }
                        }
                    }
                }
                Rectangle{
                    id:m_btnMotionStop;
                    enabled: !btnExclusicveFlag;
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
                            console.log("stop clicked");
                            btnExclusicveFlag=true;
                            m_tryRunNoneCircleTimer.stop();
                            m_tryRunCircleTimer.stop();
                            mg_servoCmd.setSpeedRef(0);
                            mg_servoCmd.delayMs(500);
                            mg_servoCmd.setServoOn(false);

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
                implicitWidth: 50;
                implicitHeight: 24;
                border.color: "#333";
                border.width: 1;
            }
        }
    }
    //滑动条样式
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

