import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Extras 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtClass 1.0
import "./components/CfgEncoder"
import "./components/CfgEncoder/"
import "./components/CfgEncoder/ToolBox"
import "./components/CfgEncoder/EncoderItem"
import com.googoltech.qmlcomponents 1.0

Rectangle {
    id:root;
    color: "#F0F0F0";
    width: 1000;
    height: 600;
    property double posAdjValue: 10;//寻相偏移角度
    property bool btnSearchIsClicked: false;//检测是否点击了寻相按钮，用于查伺服是否完成
    property var errorCode: 0x0000;
    property int cmdSrcDefault: 1;
    function updateUiFromServo(){
        console.log("driveEncoder -> onItemValueChanged")
    //            listView.setCurrentIndex(Number(factory.dataTree.textTopLevel(0,1))-1);
//        m_encoderComboBox.currentIndex=parseInt(factory.dataTree.textTopLevel(0,1))-1;
//        lineNumber.text=factory.dataTree.textTopLevel(1,1);

    }
    function onActiveNow(actived){
        console.log(actived);
        if(actived)
            m_timer.start();
        else{
            if(m_timer.running===true)
                m_timer.stop();
        }
//        m_encoderComboBox.replot();
        console.log("downCanvasArrow.onActiveNow");
    }
    function showMessage(msg){
        m_messageShow.text=msg;
        m_messageShow.visible=true;
        m_timerMSG.start();
    }

    QmlFactory{
        id:factory;
        property QTreeWidgetProxy dataTree: null;
        onInitialFactory:{
            dataTree=factory.createQTreeWidgetProxy(treeSource,driveEncoder);
        }
        Component.onCompleted:{
//            console.log("QmlFactory");
//            driveEncoder.itemValueChanged.connect(updateUiFromServo);
//            driveEncoder.activeNow.connect(onActiveNow);
        }
    }
    ServoCmd{
        id:m_cmd;
        axisIndex: axisIndexSrc;
        comType: comTypeSrc;
        Component.onCompleted: {
            m_cmd.loadCmdTree("file:///"+cmdFilePathSrc);
            m_cmd.loadAdvanceFastAccessCommandTree("file:///"+advCmdFilePathSrc);
            console.log("cmdfile="+cmdFilePathSrc);
            console.log("advCmdfile="+advCmdFilePathSrc);
        }
    }
    //------------------显示隐藏按钮----------------------------
    Rectangle{
        id:m_showButton;
        property bool poppedUp: false;
        color:Qt.darker("#F0F0F0");
        border.color: "#F0F0F0";
        border.width: 2;
        radius: m_showButton.width*0.2;
        anchors.top: parent.top;
        anchors.topMargin: 5;
        anchors.right: parent.right;
        anchors.rightMargin: 5;
        width: 40;
        height: 40;
        z:65535;
        opacity: m_showButtonMouse.containsMouse?1:0.5;
        Text{
            anchors.centerIn: parent;
            text: m_showButton.poppedUp?">>":"<<";
            font.bold: true;
        }

        MouseArea{
            id:m_showButtonMouse;
            anchors.fill: parent;
            hoverEnabled: true;
            onClicked: {
                m_showButton.poppedUp=!m_showButton.poppedUp;
                //读取当前的编码器厂家设置
                var connected=driveEncoder.getComConnectSatus();
                if(m_showButton.poppedUp&&connected){
                    var currentEncoderConfig=0x0000;
                    currentEncoderConfig=m_cmd.readAdvanceFlash("FPGA.prm.ABS_ENC_CFG.all");
                    console.log("currentConfig="+currentEncoderConfig.toString(2));
                    var index=currentEncoderConfig&0x0FFF;
                    if(index<m_encoderCfg.items.length){
                        console.log("toolbox current index="+index);
                        m_toolBox.setCurrentIndex(index)
                        m_encoderCfg.onItemClickedSlots(index);
                        m_lineNumberInput.text=m_cmd.readAdvanceFlash("gSevDrv.sev_obj.cur.rsv.prm.line_num_3");
                        m_lineNumberInput.resetLineEditColor();
                        if(index==1){
                            var comboxIndex=(currentEncoderConfig&0xF000)>>12;
                            m_encoderCfg.setCurrentBaudRateUi(comboxIndex);
                            console.log("setcombox ui= "+comboxIndex);
                        }
                    }
                }
            }
        }
    }

    //-------------------设置面板-------------------------
    Rectangle{
        id:m_encoderCfg;
        width: parent.width/3;
        height: parent.height;
        color: "#C0C0C0";
        anchors.top: parent.top;
        z:5000;
        property int leftX: parent.width-m_encoderCfg.width;
        property int  rightX: parent.width;
        visible:m_showButton.poppedUp?true:false;
        x:m_showButton.poppedUp?leftX:rightX;
        Behavior on x {NumberAnimation{}}

        function onItemClickedSlots(index){
            console.log(qsTr("item :%1 clicked").arg(index));
            if(index==1)
                m_baudRateBlock.visible=true;
            else
                m_baudRateBlock.visible=false;
            m_encoderCfg.absEncoderItem=m_encoderCfg.items[index];
            console.log(qsTr("encoder type=%1").arg(m_encoderCfg.absEncoderItem.encConfigData));
        }
        function setCurrentBaudRateUi(index){
            m_baudRateConfig.currentIndex=index;
        }

        property AbstractEncoderItem absEncoderItem: null;

        property AbstractEncoderItem duomoItem:DuoMoEncoderItem{
        }
        property AbstractEncoderItem haidehanItem:HaiDeHanEncoderItem{
        }
        property AbstractEncoderItem nikangItem:NiKangEncoderItem{
        }
        property AbstractEncoderItem sanxieItem:SanXieEncoderItem{
        }
        property AbstractEncoderItem songxiaItem:SongXiaEncoderItem{
        }

        property var items: [duomoItem,nikangItem,haidehanItem,sanxieItem,songxiaItem]

        ColumnLayout{
            anchors.fill: parent;
            ToolBox{
                id:m_toolBox
                Layout.fillWidth: true;
                Layout.fillHeight: true;
                onTitleClicked: {
                    console.log(qsTr("title :%1 clicked").arg(index));
                    if(index>0){
                        m_btnSaveConfig.enabled=false;
                    }
                    else{
                        m_btnSaveConfig.enabled=true;
                    }
                }
                onItemClicked:{
                    m_encoderCfg.onItemClickedSlots(index);
                }
            }
            //分辨率及波特率设置
            Item{
               Layout.fillWidth: true;
               height: 80;
               RowLayout{
                   anchors.fill: parent;

                   Item{
                       id:m_lineNumberBlock;
                       Layout.fillWidth: true;
                       Layout.fillHeight: true;
                       RowLayout{
                           anchors.fill: parent;
                           Text{
                               text:qsTr("编码器分辨率:")
                           }
                           TextField{
                               id:m_lineNumberInput;
                               Layout.fillWidth: true;
                               text:qsTr("0");
                               style: TextFieldStyle{
                                   textColor: "black";
                                   background: Rectangle{
                                       radius: 5;
                                       implicitHeight: 24;
                                       implicitWidth: 100;
                                       border.color: "gray";
                                       border.width: 1;
                                   }
                               }
                               function onLinerNumberTextChanged(){
                                   m_lineNumberInput.textColor="red";
                               }
                               function resetLineEditColor(){
                                   m_lineNumberInput.textColor="black";
                               }

                               Component.onCompleted: {
                                   m_lineNumberInput.textChanged.connect(onLinerNumberTextChanged);
                               }
                           }
                       }
                   }
                   Item{
                       id:m_baudRateBlock;
                       Layout.fillWidth: true;
                       Layout.fillHeight: true;
                       anchors.margins: 20;
                       RowLayout{
                           anchors.fill: parent;
                           Text{
                               text:qsTr("波特率:")
                           }
                           ComboBox{
                               id:m_baudRateConfig;
                               model: ListModel{
                                   id:m_baudRateModel;
                                   ListElement{text:"4M";data:0}
                                   ListElement{text:"2.5M";data:1}
                               }
                               onCurrentIndexChanged:{
                                   var oper=0x0000;
                                   oper=m_baudRateModel.get(currentIndex).data;
                                   oper=oper<<12;
                                   var cldata=0x0FFF;
                                   m_encoderCfg.nikangItem.encConfigData&=cldata;
                                   m_encoderCfg.nikangItem.encConfigData|=oper;
                                   console.log(qsTr("current index=%1,oper_data=%2").arg(m_baudRateModel.get(currentIndex).text).arg(oper.toString(16)));
                                   console.log(qsTr("encConfigData=%1").arg(m_encoderCfg.nikangItem.encConfigData.toString(16)));
                               }
                           }
                       }
                       Component.onCompleted: m_baudRateBlock.visible=false;
                   }

               }
            }
            //提示信息
            Text{
                id:m_msgText;
                Layout.fillWidth: true;
                text:qsTr("");
                visible: true;
                horizontalAlignment: Text.AlignHCenter;
            }
            //保存按钮
            Button{
                id:m_btnSaveConfig;
                anchors.horizontalCenter:  parent.horizontalCenter
                Layout.fillWidth: true;
                implicitHeight: 50;
                text: qsTr("保存配置");
                style: ButtonStyle {
                    background: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 25
                        border.width: control.activeFocus ? 2 : 1
                        border.color: "#888"
                        radius: 4
                        gradient: Gradient {
                            GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                            GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                        }
                    }
                }
                onClicked: {
                    var connected=driveEncoder.getComConnectSatus();
                    if(connected){
                        m_msgText.text=qsTr("保存成功! 设备重启后参数生效!");

                        m_encoderCfg.absEncoderItem.lineNumber=parseInt(m_lineNumberInput.text);
                        m_lineNumberInput.resetLineEditColor();
                        console.log("--------------------save-------------------------")
                        console.log("lineNumber="+m_encoderCfg.absEncoderItem.lineNumber);
                        console.log("enctype="+m_encoderCfg.absEncoderItem.encConfigData);
                        m_cmd.writeAdvanceFlash("FPGA.prm.ABS_ENC_CFG.all",m_encoderCfg.absEncoderItem.encConfigData);
                        m_cmd.writeAdvanceFlash("gSevDrv.sev_obj.cur.rsv.prm.line_num_3",m_encoderCfg.absEncoderItem.lineNumber);
                        factory.dataTree.setTopLevelText(1,1,m_lineNumberInput.text);//修改列表中的值
                    }
                    else{
                        m_msgText.text=qsTr("请先连接设备!");
                    }
                    if(m_timerMSG.running==false)
                        m_timerMSG.start();
                }

            }
        }
        Component.onCompleted: absEncoderItem=m_encoderCfg.items[0];
    }

    Text{
        text:"当前轴号："+axisIndexSrc;
        id:m_currentAxis;
//        text:"当前轴号："+0;
        anchors.horizontalCenter: m_centerRowLayout.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 30;
        font.bold: true;
        horizontalAlignment: Text.AlignHCenter;
        Layout.fillWidth: true;
    }
    //信息框
    Text{
        id:m_messageShow;
        anchors.horizontalCenter: m_currentAxis.horizontalCenter;
        anchors.top: m_currentAxis.bottom;
        anchors.topMargin: 30;
        horizontalAlignment: Text.AlignHCenter;
        text:qsTr(" ");
        visible: false;
    }
    ////报警框
    Item{
        id:m_encoderWarnningBlock;
        anchors.horizontalCenter: m_currentAxis.horizontalCenter;
        anchors.top: m_currentAxis.bottom;
        anchors.topMargin: 10;
        width: 200;
        visible: false;
        ColumnLayout{
            anchors.fill: parent;
            spacing: 0;
            Text{
                id:m_encoderWarnningMsg;
                Layout.fillWidth: true;
                horizontalAlignment: Text.AlignHCenter;
                text:m_encoderCfg.absEncoderItem.errorString(root.errorCode);
                color: "red"
                font.pixelSize: 16;
            }
            Button{
                id:m_btnClearAlarm;
                Layout.fillWidth: true;
                height: 40;
                style: ButtonStyle {
                    background: Rectangle {
                        implicitWidth: 150
                        implicitHeight: 40
                        border.width: control.activeFocus ? 2 : 1
                        border.color:"#888"
                        radius: 4
                        gradient: Gradient {
//                            GradientStop { position: 0 ; color:control.pressed ? "#ccc" : control.hovered?"#eee":"transparent" }
//                            GradientStop { position: 1 ; color:control.pressed ? "#aaa" : control.hovered?"#ccc":"transparent" }
                            GradientStop { position: 0 ; color:control.pressed ? "#ccc" : "#eee" }
                            GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                        }
                    }
                    label: Text{
                        text:qsTr("清 报 警");
                        color: control.hovered?"steelblue":"black";
                        horizontalAlignment: Text.AlignHCenter;
                        verticalAlignment: Text.AlignVCenter;
                    }
                }
                onClicked: {
                    var connected=driveEncoder.getComConnectSatus();
                    if(connected){
                        if("NULL"==m_cmd.writeCommand("gSevDrv.sev_obj.cur.pro.enc_info.all",0))
                            m_cmd.writeCommand("gSevDrv.sev_obj.cur.pro.enc_info.all",0)
                        m_encoderWarnningBlock.visible=false;
                        console.log("clear encoder alarm");
                    }
                }
            }
        }
    }

    //-------------------两个编码盘-----------------------
    RowLayout{
        id:m_centerRowLayout;
        anchors.fill: parent;
        anchors.margins: 0;
        spacing: 10;
        CircularGauge {
            id: gauge;
            implicitHeight: 400;
            implicitWidth: 400;

            minimumValue: 0;
            maximumValue: 360;
            stepSize: 1;
            value:0;

            style:CircularGaugeDarkStyle{}
            Component.onCompleted: {forceActiveFocus();console.log("CircularGauge-machine");}
//            Behavior on value {
//                NumberAnimation {
//                    duration: 200
//                }
//            }
            Item{
                width: 2*height;
                height: gauge.height*0.1;
                anchors.horizontalCenter: gauge.horizontalCenter;
                anchors.verticalCenter: gauge.verticalCenter;
                anchors.verticalCenterOffset:-gauge.height/5;
                Text{
                    id:m_angleText;
                    text:qsTr("%1").arg(gauge.value);
                    anchors.centerIn: parent;
                    color: "white"
                    font.bold: true;
                    font.pixelSize: parent.height*0.9;
                }
            }
            Text{
                anchors.top: parent.top;
                anchors.topMargin: -30;
                anchors.horizontalCenter: parent.horizontalCenter;
                text:qsTr("机械角");
                horizontalAlignment: Text.AlignHCenter;
            }
        }
        ColumnLayout{
            spacing: 40;
            Layout.fillHeight: true;
            Layout.minimumWidth: 200;

//            Item{
//                id:sapce0;
//                Layout.minimumHeight: 20;
//            }

            RowLayout{
                Layout.fillWidth: true;
                Text{
                    Layout.minimumWidth: 80;
                    Layout.fillWidth: true;
                    text:"编码器输入:";
                }
                TextInput{
                    id:posIn;
                    text:"0";
                    enabled: false;
                    Layout.minimumWidth: 50;
                    Layout.fillWidth: true;
                }
                Text{
                    text:"(pulse)"
                    Layout.minimumWidth: 60;
                }
            }
            RowLayout{
                Layout.fillWidth: true;
                Text{
                    text:"编码器偏移:";
                    Layout.minimumWidth: 80;
                    Layout.fillWidth: true;
                }
                TextInput{
                    id:posOffset;
                    text:"0";
                    enabled: false;
                    Layout.minimumWidth: 50;
                    Layout.fillWidth: true;
                }
                Text{
                    text:"(pulse)"
                    Layout.minimumWidth: 60;
                }
            }
            RowLayout{
                Layout.fillWidth: true;
                Text{
                    text:"编码器实际:";
                    Layout.minimumWidth: 80;
                    Layout.fillWidth: true;
                }
                TextInput{
                    id:pos;
                    text:"0";
                    enabled: false;
                    Layout.minimumWidth: 50;
                    Layout.fillWidth: true;
                }
                Text{
                    text:"(pulse)";
                    Layout.minimumWidth: 60;
                }
            }
        }
        CircularGauge {
            id: gauge_Electric;
            implicitHeight: 400;
            implicitWidth: 400;

            minimumValue: 0;
            maximumValue: 360;
            stepSize: 1;
            value:0;

            style:CircularGaugeDarkStyle{}
            Component.onCompleted: {forceActiveFocus();console.log("CircularGauge-electric");}

            Item{
                width: 2*height;
                height: gauge_Electric.height*0.1;
                anchors.horizontalCenter: gauge_Electric.horizontalCenter;
                anchors.verticalCenter: gauge_Electric.verticalCenter;
                anchors.verticalCenterOffset:-gauge_Electric.height/5;
                Text{
                    id:m_angleElectricText;
                    text:qsTr("%1").arg(gauge_Electric.value);
                    anchors.centerIn: parent;
                    color: "white"
                    font.bold: true;
                    font.pixelSize: parent.height*0.9;
                }
            }

            Text{
                anchors.top: parent.top;
                anchors.topMargin: -30;
                anchors.horizontalCenter: parent.horizontalCenter;
                text:qsTr("电气角");
                horizontalAlignment: Text.AlignHCenter;
            }
        }
    }

    //--------------------寻相操作------------------
    Column{
        id:m_editInputField
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 30;
        anchors.horizontalCenter: parent.horizontalCenter;
        spacing: 30;
        RollWheel{
            id:m_rollWheel;
            width: 250;
            height: 50;
        }
        Item{
            width: 250;
            height: 50;
            RowLayout{
                anchors.fill: parent;
                spacing: 30;
                CheckBox {
                    id:m_saveCheckBox;
                    text: qsTr("是否保存")
                    checked: false
                }
                Button{
                    id:m_btnStartTest;
                    height: 40;
                    property bool barVisible: false;
                    property int barValue: 0;
            //        text:"开 始 寻 相";
                    style: ButtonStyle {
                        background: Rectangle {
                            implicitWidth: 150
                            implicitHeight: 40
                            border.width: control.activeFocus ? 2 : 1
                            border.color: "#888"
                            radius: 4
                            gradient: Gradient {
                                GradientStop { position: 0 ; color:control.pressed ? "#ccc" : "#eee" }
                                GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                            }
                            ProgressBar {
                                id:m_progressBar
                                anchors.fill: parent;
                                value:control.barValue;
                                visible: control.barVisible;
                                maximumValue: 100;
                                minimumValue:0;
                                style: ProgressBarStyle {
                                    background: Rectangle {
                                        radius: 2
                                        color: "lightgray"
                                        border.color: "gray"
                                        border.width: 1
                                        implicitWidth: control.width;
                                        implicitHeight: 24
                                    }
                                    progress: Rectangle {
                                        color: "steelblue"
                                        border.color: "steelblue"
                                    }
                                }
                            }
                        }
                        label: Text{
                            text:qsTr("开 始 寻 相");
                            color: control.hovered?"steelblue":"black";
                            horizontalAlignment: Text.AlignHCenter;
                            verticalAlignment: Text.AlignVCenter;
                        }
                    }
                }
            }
        }

        //编码器类型下拉选择
/*        ComboBox{
            id:m_encoderComboBox;
            signal replot();
            width: 120;
            model:["1 绝对值","2 增量式","3 无传感"];
            style:ComboBoxStyle{
                dropDownButtonWidth: 20;
                background: Rectangle{
                    implicitHeight: 24;
                    border.width: control.editable?0:1;
                    border.color: (control.hovered||control.pressed)?"blue":"gray";
                    color:(control.hovered||control.pressed)?"#e0e0e0":"#c0c0c0";

                    Canvas{
                        id:downCanvasArrow;
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
                        Component.onCompleted: {
                            m_encoderComboBox.replot.connect(requestPaint);
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
                    font.pixelSize: 12;
                }
            }
            Component.onCompleted:console.log("ComboBox");
        }*/

/*        RowLayout{
            anchors.margins: 10;
            spacing: 10;
            Text{text:qsTr("编码器分辨率:")}
            TextField{
                id:lineNumber;
                text:qsTr("0");
                Layout.minimumWidth: 50;
                Layout.fillWidth: true;
                style: TextFieldStyle{
                    textColor: "black";
                    background: Rectangle{
                        radius: 6;
                        implicitHeight: 24;
                        implicitWidth: 100;
                        border.color: "#333";
                        border.width: 1;
                    }
                }
                function onLinerNumberTextChanged(){
                    lineNumber.textColor="red";
//                    factory.dataTree.setTopLevelText(1,1,lineNumber.text);
                }
                Component.onCompleted: {
                    lineNumber.textChanged.connect(onLinerNumberTextChanged);
                }
            }
        }*/
    }

    Timer{
        id:m_timer;
        interval: 500;
        repeat: true;
        triggeredOnStart: false;
        property int checkCount: 0;
        property var servoState: {"CheckServoOn":0,"CheckFinish":1,"Quit":2}
        property var currentState: servoState.CheckServoOn;
        onTriggered: {
//            console.log("current:"+m_cmd.axisIndex+"......");
            var strPos=m_cmd.readCommand("gSevDrv.sev_obj.cur.rsv.pos");
            pos.text=strPos;
//            console.log("pos="+strPos);

            var strPosIn=m_cmd.readCommand("gSevDrv.sev_obj.cur.rsv.pos_in");
            posIn.text=strPosIn;
            var precision=parseInt(m_cmd.readCommand("gSevDrv.sev_obj.cur.rsv.prm.line_num_3"));
            if(strPosIn!=="NULL")
                gauge.value=360*parseInt(strPosIn)/precision;
//            console.log("pos in="+strPosIn);
//            console.log("precision="+precision);

            var strPos_ofst=m_cmd.readCommand("gSevDrv.sev_obj.cur.rsv.prm.pos_ofst_3");
            posOffset.text=strPos_ofst;
//            console.log("strPos_ofst="+strPos_ofst);

            var strPPN=m_cmd.readCommand("gSevDrv.sev_obj.cur.mot.PPN_1");
            if(strPPN!=="NULL"){
                var PPN=parseInt(strPPN);
                var angleEle=(gauge.value*PPN)%360;
                gauge_Electric.value=parseInt(angleEle);
            }
//            console.log("strPPN="+strPPN);

            //关伺服逻辑
            if(root.btnSearchIsClicked){
                checkCount++;
                var servoIsOn=true;
                var flagFinish=false;
                var ret=0;
                var adjFlag;

                switch(currentState){

                case servoState.CheckServoOn:
                    m_btnStartTest.barValue+=10;
                    if(m_btnStartTest.barValue>=100){
                        m_btnStartTest.barValue=0;
                    }
                    console.log("servoState.CheckServoOn");
                    servoIsOn=m_cmd.checkServoIsReady();
                    if(servoIsOn)
                        currentState=servoState.CheckFinish;
                    if((servoIsOn===false)&&(checkCount>3))
                        currentState=servoState.Quit;
                    break;
                case servoState.CheckFinish:
                    m_btnStartTest.barValue+=10;
                    if(m_btnStartTest.barValue>=100){
                        m_btnStartTest.barValue=0;
                    }
                    console.log("servoState.CheckFinish");
                    adjFlag=m_cmd.readCommand("gSevDrv.sev_obj.mfj.pos_adj_flag");
                    if(adjFlag!=="NULL")
                        ret=parseInt(adjFlag);
                    flagFinish=Boolean(ret);
                    if(flagFinish){
                        checkCount=0;
                        if(m_saveCheckBox.checked){
                            factory.dataTree.setTopLevelText(2,1,strPos_ofst);
                            m_cmd.writeAdvanceFlash("gSevDrv.sev_obj.cur.rsv.prm.pos_ofst_3",parseInt(strPos_ofst));
                            root.showMessage(qsTr("寻相完成，并保存相位"));
                            console.log("save phase");
                        }
                        m_btnStartTest.barValue=100;
                        currentState=servoState.Quit;
                    }
                    else{
                        if(checkCount>30)
                            currentState=servoState.Quit;
                    }

                    break;
                case servoState.Quit:
                    console.log("servoState.Quit");
                    m_cmd.setServoOn(false);
                    root.btnSearchIsClicked=false;
                    checkCount=0;
                    m_btnStartTest.barVisible=false;
                    m_cmd.writeCommand("gSevDrv.sev_obj.pos.seq.prm.cmd_src_sel",root.cmdSrcDefault);
                    currentState=servoState.CheckServoOn;
                    break;
                }
                console.log("checkCount:"+checkCount);
            }

            //读取编码器报警信息
            var errorCodeStr=m_cmd.readCommand("gSevDrv.sev_obj.cur.pro.enc_info.all");
//            console.log("errorCodeStr="+errorCodeStr);
            if(errorCodeStr!=="NULL"){
                root.errorCode=parseInt(errorCodeStr);
                if(m_encoderCfg.absEncoderItem.hasError(root.errorCode))
                    m_encoderWarnningBlock.visible=true;
                else
                    m_encoderWarnningBlock.visible=false;
//                console.log("errorCode:"+root.errorCode.toString(16));
            }
        }
    }

    Timer{
        id:m_timerMSG;
        interval: 2000;
        repeat: false;
        onTriggered: {
            m_messageShow.visible=false;
            m_msgText.text=qsTr("");
        }
    }

    Connections{
        target: m_btnStartTest;
        onClicked:{
            var status=driveEncoder.getComConnectSatus();
            var servoIsOn=false;
            console.log("openState:"+status);
            if(status){
                servoIsOn=m_cmd.checkServoIsReady();
                //伺服开的话不允许寻相操作
                if(servoIsOn){
                    m_messageShow.text=qsTr("伺服正在工作中，禁止寻相");
                    m_messageShow.visible=true;
                    m_timerMSG.start();
                }
                else{
                    //先读控制源，0:后台控制 1:控制器控制  如果控制源不是0，则修改控制源
                    var srcSelect=100;
                    var srcString="gSevDrv.sev_obj.pos.seq.prm.cmd_src_sel";
                    srcSelect=parseInt(m_cmd.readCommand(srcString));
                    root.cmdSrcDefault=srcSelect;
                    console.log("srcSelect:"+srcSelect);
                    //获得控制权
                    if(srcSelect!==0){
                        var ret="0";
                        ret=m_cmd.writeCommand(srcString,0);
                        console.log("ret write value:"+ret);
                    }
                    m_cmd.setServoTaskMode(2);
                    m_cmd.setPosAdjRef(m_rollWheel.curValue);
                    m_cmd.setServoOn(true);

                    root.btnSearchIsClicked=true;
                    //显示进度条
                    m_btnStartTest.barVisible=true;
                    m_btnStartTest.barValue=0;
                }
            }
        }
    }

    Component.onCompleted:{
        console.log("CfgEncoder.qml");
//        m_timer.start();
        driveEncoder.activeNow.connect(onActiveNow);
    }
}

