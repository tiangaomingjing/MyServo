import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Extras 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import "./Encoder"

Item {
    id:encoderConfig;
    objectName: "EncoderConfig";
    property bool initialPhaseClicked: false//控制是否点击了寻相操作
    property bool searchPhaseIsClicked: false;//控制关寻相下伺服
    property int checkCount: 0;
    property double posAdjValue: 5;//寻相偏移强度
    //连接到MotionConfig发射的信号
    function onStartTimer(){
        m_updateTimer.start();
    }
    function stopTimer(){
        m_updateTimer.stop();
    }
    function resetInitialPhase(){
        initialPhaseClicked=false;
        m_btnSearchPhaseRet.enabled=false;
    }

    function prevExecuteCommand(){
        m_updateTimer.stop();
        console.log("EncoderConfig prev executeCommand");
    }

    function executeCommand(){
        console.log("EncoderConfig executeCommand");
        if(initialPhaseClicked){
            m_updateTimer.stop();
            return true;
        }
        else{
            return false;
        }
    }
    function aHeadExecuteCommand(){
        console.log("EncoderConfig aHeadExecuteCommand");
        m_updateTimer.start();
    }
    property var posIn: 0;
    property var pos: 0;
    property var posOffset: 0;
    //更新时钟
    Timer{
        id:m_updateTimer;
        interval: 200;
        repeat:true;
        triggeredOnStart: false;
        onTriggered: {
            var posInStr=mg_controlKeyName.value("pos_in");
            var posStr=mg_controlKeyName.value("pos");
            var posOfstStr=mg_controlKeyName.value("pos_ofst");
            console.log(posInStr+" "+posStr+" "+posOfstStr);
            posIn=parseInt(mg_servoCmd.readCommand(posInStr));
            pos=parseInt(mg_servoCmd.readCommand(posStr));
            posOffset=parseInt(mg_servoCmd.readCommand(posOfstStr));
            var precision=parseInt(m_encoderLineNumber.text);
            var angleMac=(posIn/precision)*360.0
            var angleEle=(angleMac*rootMain.mg_ppnValue)%360;
            m_gaugeMachine.value=angleMac;
            m_gaugeElectronic.value=angleEle;
            console.log(qsTr("angleMac:%1,angleEle:%2,precision:%3").arg(angleMac).arg(angleEle).arg(precision));

            //关寻相操作
            if(searchPhaseIsClicked){
                var flagServo=false;
                var ret=0;
                var posAdjStr=mg_controlKeyName.value("pos_adj_flag");
                ret=parseInt(mg_servoCmd.readCommand(posAdjStr))
                flagServo=Boolean(ret);
                console.log("check servo flag");
                checkCount++;
                //进度条操作
                m_mainProgressBar.visible=true;
                m_mainProgressTextValue.visible=true;
                m_mainProgressBar.value=checkCount;
                if(checkCount>100){
                    searchPhaseIsClicked=false;//不再进入关寻相操作查询
                    checkCount=0;
                    mg_servoCmd.setServoOn(false);
                    m_mainProgressBar.value=100;
                    m_mainProgressBar.visible=false;
                    m_mainProgressTextValue.visible=false;
                    m_btnEncoderPrmExecuteRet.enabled=true;
                }

                if(flagServo){
                    m_mainProgressBar.value=100;
                    checkCount=0;
                    mg_servoCmd.delayMs(50);
                    mg_servoCmd.setServoOn(false);
                    var retV=mg_servoCmd.writeAdvanceFlash(posOfstStr,posOffset);//从AdvanceFastCommand.xml中找地址
                    console.log(qsTr("writeAdvanceFlash retvalue:%1").arg(retV));
                    searchPhaseIsClicked=false;
                    console.log("servo off");

                    m_mainProgressBar.visible=false;
                    m_mainProgressTextValue.visible=false;
                    m_btnEncoderPrmExecuteRet.enabled=true;
                }
            }
        }
    }

    Text{
        anchors.bottom: m_gaugeMachine.top;
        anchors.bottomMargin: 20;
        anchors.horizontalCenter: m_gaugeMachine.horizontalCenter;
        text:"机械角度"
        font.pixelSize: 16;
    }

    CircularGauge {
        id: m_gaugeMachine;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        width: 250;
        height: 250;

        minimumValue: 0;
        maximumValue: 360;
        stepSize: 1;
//            value: accelerating?maximumValue:0;
        value:0;

        style:CircularGaugeDarkStyle{}

        Component.onCompleted: forceActiveFocus()
//        Behavior on value {
//            NumberAnimation {
//                duration: 200
//            }
//        }
        Item{
            width: 2*height;
            height: m_gaugeMachine.height*0.1;
            anchors.horizontalCenter: m_gaugeMachine.horizontalCenter;
            anchors.verticalCenter: m_gaugeMachine.verticalCenter;
            anchors.verticalCenterOffset:-m_gaugeMachine.height/5;
            Text{
                id:m_angleText;
                text:m_gaugeMachine.value;
                anchors.centerIn: parent;
                color: "white"
                font.bold: true;
                font.pixelSize: parent.height*0.9;
            }
        }
    }

    Text{
        anchors.bottom: m_gaugeElectronic.top;
        anchors.bottomMargin: 20;
        anchors.horizontalCenter: m_gaugeElectronic.horizontalCenter;
        text:"电角度"
        font.pixelSize: 16;
    }
    CircularGauge {
        id: m_gaugeElectronic;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.rightMargin: 5;
        width: 250;
        height: 250;

        minimumValue: 0;
        maximumValue: 360;
        stepSize: 1;
        value:0;

        style:CircularGaugeDarkStyle{}

        Component.onCompleted: forceActiveFocus()
//        Behavior on value {
//            NumberAnimation {
//                duration: 200
//            }
//        }
        Item{
            width: 2*height;
            height: m_gaugeElectronic.height*0.1;
            anchors.horizontalCenter: m_gaugeElectronic.horizontalCenter;
            anchors.verticalCenter: m_gaugeElectronic.verticalCenter;
            anchors.verticalCenterOffset:-m_gaugeElectronic.height/5;
            Text{
                id:m_angle1Text;
                text:m_gaugeElectronic.value;
                anchors.centerIn: parent;
                color: "white"
                font.bold: true;
                font.pixelSize: parent.height*0.9;
            }
        }
    }


    Row{
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 40;
        anchors.left: parent.left;
        anchors.leftMargin: 20;
        spacing:20;
        //编码器类型下拉选择
        ComboBox{
            id:m_encoderComboBox;
            width: 200;
            model:["1 绝对值","2 增量式","3 无传感"];
            style:ComboBoxStyle{
                dropDownButtonWidth: 20;
                background: Rectangle{
                    implicitHeight: 24;
                    border.width: control.editable?0:1;
                    border.color: (control.hovered||control.pressed)?"blue":"gray";
                    color:(control.hovered||control.pressed)?"#e0e0e0":"#c0c0c0";
                    Canvas{
                        width: 16;
                        height:18;
                        anchors.right: parent.right;
                        anchors.rightMargin: 2;
                        anchors.top: parent.top;
                        anchors.topMargin: 1;
                        onPaint: {
                            var ctx=getContext("2d");
                            ctx.save();
                            ctx.strokeStyle="black";
                            ctx.lineWidth=2;
                            ctx.beginPath();
                            ctx.moveTo(1,8);
                            ctx.lineTo(8,16);
                            ctx.lineTo(15,8);
                            ctx.stroke();
                            ctx.restore();
                        }
                    }
                }
                label:Text{
                    anchors.left: parent.left;
                    anchors.leftMargin: 2;
                    height: parent.height;
                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: Text.AlignHCenter;
                    text:control.currentText;
                    color:(control.hovered||control.pressed)?"blue":"gray";
                    font.pixelSize: 16;
                }
            }
        }
        Item{
            id:lineNumberItem;
            width: 200;
            height: 20;
            Row{
                anchors.fill: parent;
                Text{
                    anchors.verticalCenter: parent.verticalCenter;
                    text:"编码盘分辨率："
                    font.pixelSize: 16;
                }
                TextField{
                    id:m_encoderLineNumber;
                    anchors.verticalCenter: parent.verticalCenter;
                    style:TextFieldStyle{
                        textColor: "black";
                        background: Rectangle{
                            radius: 6;
                            implicitWidth: 100;
                            implicitHeight: 24;
                            border.color: "#333";
                            border.width: 1;
                        }
                    }
                    onTextChanged: {
                        m_encoderLineNumber.textColor="red";
                        m_btnSearchPhaseRet.enabled=false;
                        initialPhaseClicked=false;
                    }
                    Component.onCompleted: {
                        text="131072";
                        m_encoderLineNumber.textColor="black";
                    }
                }
            }
        }
        Rectangle{
            id:m_btnEncoderPrmExecuteRet;
            width: 100;
            height: 24;
            color:m_btnEncoderPrmExecute.containsPress?Qt.darker("yellow"):
                  m_btnEncoderPrmExecute.containsMouse?Qt.lighter("yellow",1.3):"transparent";
            border.color: "#BBB9B9";
            border.width: 1;
            radius: 5;
            Text{
                anchors.centerIn: parent;
                text:"参数整定";
                font.letterSpacing:5;
                color:"black";
                font.pixelSize: 16;
            }
            MouseArea{
                id:m_btnEncoderPrmExecute;
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    m_updateTimer.stop();
                    console.log("connect clicked childrenCount:");
                    m_btnSearchPhaseRet.enabled=true;
                    var versionFilePath="file:///"+sysConfigFilePathSrc+userConfigSrc.configSetting.typeName+
                            "/"+userConfigSrc.configSetting.modelName+
                            "/"+userConfigSrc.configSetting.version;
                    //设置线数
                    var encoderXmlPath=versionFilePath+"/PrmFuncEncoder.xml"
                    var value=parseInt(m_encoderLineNumber.text);
                    var lineNumStr=mg_controlKeyName.value("line_num");
                    var coderTypStr=mg_controlKeyName.value("coder_typ");
                    mg_servoCmd.writeFunctionValue2Flash(encoderXmlPath,lineNumStr,value);
                    //设置编码盘类型
                    value=m_encoderComboBox.currentIndex+1;
                    mg_servoCmd.writeFunctionValue2Flash(encoderXmlPath,coderTypStr,value);

                    //软复位
                    var dspNumber=mg_servoCmd.getDspTotalNumber();
                    var dspAxis=mg_servoCmd.getDspAxis();
                    var pv0=100/dspNumber;
                    var delayTimes=6;
                    var pvInc=pv0/delayTimes;
                    var sum=0;
                    var resetFinished=false;
                    m_mainProgressBar.visible=true;
                    m_mainProgressTextValue.visible=true;
                    m_mainProgressBar.value=0;
                    for(var i=0;i<dspNumber;i++){
                        var flagFinish=false;
                        var checkCount=0;
                        console.log(qsTr("i:%1,dspNumber:%2,dspAxis:%3").arg(i).arg(dspNumber).arg(dspAxis));
                        mg_servoCmd.resetDsp(i*dspAxis);
                        for(var j=0;j<delayTimes;j++){
                            sum+=pvInc;
                            m_mainProgressBar.value=parseInt(sum);
                            mg_servoCmd.delayMs(500);
                            console.log(qsTr("sum:%1").arg(sum));
                        }
                        do{
                            resetFinished=mg_servoCmd.checkRestDspFinish(i*dspAxis);
                            checkCount++;
                            mg_servoCmd.delayMs(10);
                            console.log(qsTr("resetFinished:%1").arg(resetFinished));
                            console.log(qsTr("checkCount:%1,reset dspnumber:%2").arg(checkCount).arg(i));
                            if(checkCount>2000) break;
                        }while(!resetFinished)
                    }
                    m_mainProgressBar.value=100;
                    m_mainProgressBar.visible=false;
                    m_mainProgressTextValue.visible=false;
                    //软复位--end
                    m_updateTimer.start();
                }
            }
        }
    }

    //角度显示
    Item{
        id:m_displayAngle;
        anchors.centerIn: parent;
        width: 200;
        height: 100;
        Text{
            id:m_pos_in;
            anchors.left: parent.left;
            anchors.top: parent.top;
            text:"位置反馈: "+posIn;
            font.pixelSize: 16;
        }
        Text{
            anchors.right: parent.right;
            anchors.top:parent.top;
            text:"(count)";
            font.pixelSize: 16;
        }

        Text{
            id:m_posOffset;
            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;
            text:"位置偏置: "+posOffset;
            font.pixelSize: 16;
        }
        Text{
            anchors.right: parent.right;
            anchors.verticalCenter: parent.verticalCenter;
            text:"(count)";
            font.pixelSize: 16;
        }
        Text{
            id:m_pos;
            anchors.bottom: parent.bottom;
            anchors.left: parent.left;
            text:"电机机械角: "+pos;
            font.pixelSize: 16;
        }
        Text{
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;
            text:"(count)";
            font.pixelSize: 16;
        }
    }
    //寻相按钮
    Rectangle{
        id:m_btnSearchPhaseRet;
        width: 200;
        height: 30;
        anchors.top: m_displayAngle.bottom;
        anchors.topMargin: 20;
        anchors.horizontalCenter: parent.horizontalCenter;
        color:m_btnSearchPhase.containsPress?Qt.darker("yellow"):
              m_btnSearchPhase.containsMouse?Qt.lighter("yellow",1.3):"transparent";
        border.color: "#BBB9B9";
        border.width: 1;
        radius: 5;
        Text{
            anchors.centerIn: parent;
            text:"开始寻相";
            font.letterSpacing:8;
            font.bold: true;
            color:parent.enabled?"black":"gray";
            font.pixelSize: 16;
        }
        MouseArea{
            id:m_btnSearchPhase;
            anchors.fill: parent;
            hoverEnabled: true;
            onClicked: {
                m_btnEncoderPrmExecuteRet.enabled=false;
                //先读控制源，0:后台控制 1:控制器控制  如果控制源不是0，则修改控制源
                var srcSelect=100;
                var srcSelectStr=mg_controlKeyName.value("cmd_src_sel");
                console.log("cmd_src_sel: "+srcSelectStr);
                srcSelect=parseInt(mg_servoCmd.readCommand(srcSelectStr));
                console.log("srcSelect:"+srcSelect);
                if(srcSelect!==0){
                    var ret="0";
                    ret=mg_servoCmd.writeCommand(srcSelectStr,0);
//                    ret=mg_servoCmd.writeCommand("gSevDrv.sev_obj.pos.seq.prm.cmd_src_sel",0);
                    console.log("ret write value:"+ret);
                }

                mg_servoCmd.setServoTaskMode(2);
                mg_servoCmd.setPosAdjRef(posAdjValue);//设置寻相强度%
                mg_servoCmd.setServoOn(true);

                initialPhaseClicked=true;
                m_mainWarningMessageItem.visible=false;
                searchPhaseIsClicked=true;
            }
        }
        Component.onCompleted: m_btnSearchPhaseRet.enabled=false;
    }

}

