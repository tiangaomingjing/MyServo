import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Extras 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtClass 1.0
import "./components/CfgEncoder"
import com.googoltech.qmlcomponents 1.0

Rectangle {
    id:root;
    color: "#F0F0F0";
    property double posAdjValue: 10;//寻相偏移角度
    property bool btnSearchIsClicked: false;//检测是否点击了寻相按钮，用于查伺服是否完成
    function updateUiFromServo(){
        console.log("driveEncoder -> onItemValueChanged")
    //            listView.setCurrentIndex(Number(factory.dataTree.textTopLevel(0,1))-1);
        m_encoderComboBox.currentIndex=parseInt(factory.dataTree.textTopLevel(0,1))-1;
        lineNumber.text=factory.dataTree.textTopLevel(1,1);
        lineNumber.textColor="black";
    }
    function onActiveNow(actived){
        console.log(actived);
        if(actived)
            m_timer.start();
        else{
            if(m_timer.running===true)
                m_timer.stop();
        }
        m_encoderComboBox.replot();
        console.log("downCanvasArrow.onActiveNow");
    }

    QmlFactory{
        id:factory;
        property QTreeWidgetProxy dataTree: null;
        onInitialFactory:{
            dataTree=factory.createQTreeWidgetProxy(treeSource,driveEncoder);
        }
        Component.onCompleted:{
            console.log("QmlFactory");
            driveEncoder.itemValueChanged.connect(updateUiFromServo);
            driveEncoder.activeNow.connect(onActiveNow);
        }
    }
    ServoCmd{
        id:m_cmd;
        axisIndex: axisIndexSrc;
        comType: comTypeSrc;
        Component.onCompleted: {
            m_cmd.loadCmdTree("file:///"+cmdFilePathSrc);
            console.log(cmdFilePathSrc);
        }
    }

    RowLayout{
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
            Text{
                text:"当前轴号："+axisIndexSrc;
                font.bold: true;
                horizontalAlignment: Text.AlignHCenter;
                Layout.fillWidth: true;
            }
//            Item{
//                id:sapce0;
//                Layout.minimumHeight: 20;
//            }

            RowLayout{
                Layout.fillWidth: true;
                Text{
                    Layout.minimumWidth: 80;
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

//        Rectangle{
//            color:Qt.lighter(border.color,1.2);
//            border.color: "#BBB9B9";
//            border.width: 2;
//            radius: 10;
//            width: 300;
//            height: 100;
//            Layout.minimumHeight: 260;
//            ColumnLayout{
//                anchors.fill: parent;
//                anchors.margins: 10;
//                spacing: 5;
//                ListView{
//                    id:listView;
//                    delegate: delegateStyle;
//                    model:theModel;
//                    highlight: Rectangle{color:"blue";radius:18;border.color: "black";}
//                    focus: true;
//                    Layout.minimumHeight: 200;
//                    Layout.maximumWidth: parent.width;
//                    Layout.minimumWidth: 300;
//                    signal indexChanged(var index);
//                    function setCurrentIndex(index){listView.currentIndex=index;}
//                }
//            }
//        }

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

    Row{
        id:m_editInputField
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 30;
        anchors.horizontalCenter: parent.horizontalCenter;
        spacing: 30;
        Button{
            id:m_btnStartTest;
            height: 24;
    //        text:"开 始 寻 相";
            style: ButtonStyle {
                background: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 25
                    border.width: control.activeFocus ? 2 : 1
                    border.color: "#888"
                    radius: 4
                    gradient: Gradient {
                        GradientStop { position: 0 ; color:control.pressed ? "#ccc" : "#eee" }
                        GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
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
        //编码器类型下拉选择
        ComboBox{
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
        }
        RowLayout{
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
                    factory.dataTree.setTopLevelText(1,1,lineNumber.text);
                }
                Component.onCompleted: {
                    lineNumber.textChanged.connect(onLinerNumberTextChanged);
                }
            }
        }
    }


    Timer{
        id:m_timer;
        interval: 200;
        repeat: true;
        triggeredOnStart: false;
        property int checkCount: 0;
        onTriggered: {
            console.log("current:"+m_cmd.axisIndex+"......");
            var strPos=m_cmd.readCommand("gSevDrv.sev_obj.cur.rsv.pos");
            pos.text=strPos;

            var strPosIn=m_cmd.readCommand("gSevDrv.sev_obj.cur.rsv.pos_in");
            posIn.text=strPosIn;
            var precision=parseInt(lineNumber.text);
            gauge.value=360*parseInt(strPosIn)/precision;

            var strPos_ofst=m_cmd.readCommand("gSevDrv.sev_obj.cur.rsv.prm.pos_ofst");
            posOffset.text=strPos_ofst;

            var strPPN=m_cmd.readCommand("gSevDrv.sev_obj.cur.mot.PPN_1");
            var PPN=parseInt(strPPN);

            var angleEle=(gauge.value*PPN)%360;
            gauge_Electric.value=parseInt(angleEle);

            //关伺服逻辑
            if(root.btnSearchIsClicked){
                checkCount++;
                console.log("checkCount:"+checkCount);
                var flagServo=false;
                var ret=0;
                ret=parseInt(m_cmd.readCommand("gSevDrv.sev_obj.mfj.pos_adj_flag"));
                flagServo=Boolean(ret);
                if(flagServo||checkCount>200){
                    m_cmd.setServoOn(false);
                    root.btnSearchIsClicked=false;
                    console.log("colse the servo enable");
                    checkCount=0;
                }
            }
        }
    }


    Connections{
        target: m_encoderComboBox;
        onActivated: {
            var curindex=index+1;
            factory.dataTree.setTopLevelText(0,1,curindex.toString());//编码器类开是从1开始的
        }
//        onCurrentIndexChanged: factory.dataTree.setTopLevelText(0,1,String(m_encoderComboBox.currentIndex+1));//编码器类开是从1开始的
    }

//    Connections{
//        target: driveEncoder;
////        onItemValueChanged:{
////            console.log("driveEncoder -> onItemValueChanged")
//////            listView.setCurrentIndex(Number(factory.dataTree.textTopLevel(0,1))-1);
////            m_encoderComboBox.currentIndex=parseInt(factory.dataTree.textTopLevel(0,1))-1;
////            lineNumber.text=factory.dataTree.textTopLevel(1,1);
////            lineNumber.textColor="black";

////        }
//        onActiveNow:{
//            console.log(actived);
//            if(actived)
//                m_timer.start();
//            else{
//                if(m_timer.running===true)
//                    m_timer.stop();
//            }
//            m_encoderComboBox.replot();
//            console.log("downCanvasArrow.onActiveNow");
//        }
//    }
    Connections{
        target: m_btnStartTest;
        onClicked:{
            var status=driveEncoder.getComConnectSatus();
            console.log("openState:"+status);
            if(status){
                m_cmd.setServoTaskMode(2);
                m_cmd.setPosAdjRef(root.posAdjValue);
                m_cmd.setServoOn(true);
                root.btnSearchIsClicked=true;
            }
        }
    }

    Component.onCompleted:console.log("CfgEncoder.qml")
}

