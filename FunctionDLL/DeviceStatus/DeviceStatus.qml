import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "./components/DeviceStatus"
import QtClass 1.0
Rectangle{
    id:root;
    property int number: 0;
    property string iconPath: iconFilePath;
    property int axisIndexNumber: axisIndex;
    color:"#F0F0F0";
//    property var stateArray: ["OFF","INIT","ON"];
//    property var idArray: ["0 IDLE","1 AOA","2 MIPA","3 MPSA","4 COLD","5 CCLD","6 VCLD","7 PSOCLD"];
//    property var modeArray: ["0 IDLE_SEQ","1 AOA_SEQ","2 MIPA_SEQ","3 MPSA_SEQ",
//                             "4 COLD_SEQ","5 CCLD_SEQ","6 VCLD_SEQ","7 OPEN_VCTL_SEQ",
//                             "8 CLOSED_VCTL_SEQ","9 FIX_PCTL_SEQ","10 TRA_PCTL_SEQ","11 STOP_VCTL_SEQ_WORK",
//                              "12 CIRCLE_CURRENT_FOLLOW" ];
    property var stateArray: ["关","初始化","开"];
    property var idArray: ["0 空闲","1 ADC校正","2 电机初始相位校正","3 机械模型辨识","4 电压开环模式","5 电流闭环模式","6 速度闭环模式","7 位置闭环模式"];
    property var modeArray: ["0 空闲","1 ADC校正","2 电机初始相位校正","3 机械模型辨识",
                             "4 电压开环调试","5 电流闭环调试","6 速度闭环调试","7 轮廓速度跟踪",
                             "8 周期同步速度跟踪","9 准停","10 周期同步位置跟踪","11 规划停止",
                              "12 周期同步电流跟踪" ];
    QmlFactory{
        id:factory;
        property QTreeWidgetProxy dataTree: null;
        onInitialFactory: {
            dataTree=factory.createQTreeWidgetProxy(treeSource,driverStatus);
        }
    }

    Timer{
        id:countUp;
        interval: 1000;
        repeat: true;
        triggeredOnStart: true;
        onTriggered:{
            root.number+=1;
            if(root.number==65536) root.number=0;
            console.log(root.number);
            root.updateUiData();
        }
    }
    RowLayout{
        anchors.fill: parent;
        anchors.margins: 50;
        spacing: 30;
        Rectangle{
            id:ret1;
            border.color: "#BBB9B9";
            border.width: 2;
            color: Qt.lighter(border.color,1.2);
            width: 250;
            height: 100;
            Layout.fillHeight: true;
            Layout.minimumHeight: 400;
            Layout.minimumWidth: 250;
            radius: 10;
            property int leftDistance: -60;
            ColumnLayout{
                anchors.fill: parent;
                anchors.margins: 20;
                spacing: 10;
                Item{
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                }

                LedIndicator{
                    id:m_allLedFlag;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    ledOn: true;
                    iconPath:root.iconPath;
                    title:"";
                }
                Text{
                    id:m_axisSatus;
                    text:m_allLedFlag.ledOn?"当前轴号:"+root.axisIndexNumber+" OK...":"当前轴号:"+root.axisIndexNumber+" Error...";
                    horizontalAlignment: Text.AlignHCenter;
                    Layout.fillWidth: true;
                }
                Image {
                    id: m_servoImage;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    source: "file:///"+root.iconPath+"servo_device.png";
                }
                Button{
                    id:m_clearAlarm;
                    anchors.horizontalCenter:  parent.horizontalCenter;
                    text:"清报警"
                    onClicked: {
                        console.log("clear alarm clicked...");
                        driverStatus.clearAlarm();
                    }
                }
                Item{
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                }
                RowLayout{
                    spacing: 10;
                    anchors.left: parent.horizontalCenter;
                    anchors.leftMargin: ret1.leftDistance;
                    Text{
                        text:"伺服状态:";
                    }
                    Text{
                        id:m_state;
                        text:"undefined";
                    }
                }
                RowLayout{
                    spacing: 10;
                    anchors.left: parent.horizontalCenter;
                    anchors.leftMargin: ret1.leftDistance;
                    Text{
                        text:"母线电压:";
                    }
                    Text{
                        id:m_vdc;
                        text:"0";
                    }
                }
                RowLayout{
                    spacing: 10;
                    anchors.left: parent.horizontalCenter;
                    anchors.leftMargin: ret1.leftDistance;
                    Text{
                        text:"命令模式:";
                    }
                    Text{
                        id:m_cmdid;
                        text:"0 IDLE";
                    }
                }
                RowLayout{
                    spacing: 10;
                    anchors.left: parent.horizontalCenter;
                    anchors.leftMargin: ret1.leftDistance;
                    Text{
                        text:"当前模式:";
                    }
                    Text{
                        id:m_currid;
                        text:"undefined";
                    }
                }
                RowLayout{
                    spacing: 10;
                    anchors.left: parent.horizontalCenter;
                    anchors.leftMargin: ret1.leftDistance;
                    Text{
                        text:"用户模式:";
                    }
                    Text{
                        id:m_usrmode;
                        text:"undefined";
                    }
                }
                Item{
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                }
            }
        }
        Rectangle{
            id:ret2;
            border.color: "#BBB9B9";
            border.width: 2;
            color: Qt.lighter(border.color,1.2);
            width: 100;
            height: 100;
            Layout.fillHeight: true;
            Layout.fillWidth: true;
            radius: 10;
            RowLayout{
                anchors.fill: parent;
                anchors.margins: 20;
                spacing: 10;
                Item{
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                }
                ColumnLayout{
                    spacing: 10;
                    LedIndicator{
                        id:led_OC;
                        title: "过流";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_OV;
                        title: "过压";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_UV;
                        title: "欠压";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_BrkPh;
                        title: "输入断线";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_ResErr;
                        title: "编码器故障";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_OL;
                        title: "过载";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_OT;
                        title: "过温";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_IOerr;
                        title: "IO故障";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                }
                Item{
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                }
                ColumnLayout{
                    spacing: 10;
                    LedIndicator{
                        id:led_REG;
                        title: "寄存器故障";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_PS;
                        title: "功率模块故障";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_OS;
                        title: "过速";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_OPRE;
                        title: "过压力";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_DIR;
                        title: "方向错误";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_SOC;
                        title: "瞬时过流";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_OBPH;
                        title: "输出断线";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                    LedIndicator{
                        id:led_rsvd;
                        title: "保留位";
                        iconPath: root.iconPath;
                        Layout.fillHeight: true;
                    }
                }
                Item{
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                }
            }
        }
    }
    function updateUiData(){
        led_OC.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,0,1)));
        led_OV.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,1,1)));
        led_UV.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,2,1)));
        led_BrkPh.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,3,1)));
        led_ResErr.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,4,1)));
        led_OL.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,5,1)));
        led_OT.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,6,1)));
        led_IOerr.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,7,1)));;
        led_REG.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,8,1)));
        led_PS.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,9,1)));
        led_OS.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,10,1)));
        led_OPRE.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,11,1)));
        led_DIR.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,12,1)));
        led_SOC.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,13,1)));
        led_OBPH.ledOn=!Boolean(parseInt(factory.dataTree.textChild(3,14,1)));
        m_allLedFlag.ledOn=!Boolean(parseInt(factory.dataTree.textTopLevel(2,1)));

        m_vdc.text=factory.dataTree.textTopLevel(1,1);

        var index=parseInt(factory.dataTree.textTopLevel(0,1))-1;
        var str="";
        if(index<stateArray.length){
            str=stateArray[index];
            m_state.text=str;
        }

        index=parseInt(factory.dataTree.textTopLevel(4,1));
        if(index<idArray.length)
        {
            str=idArray[index];
            m_cmdid.text=str;
        }

        index=parseInt(factory.dataTree.textTopLevel(5,1));
        if(index<idArray.length)
        {
            str=idArray[index];
            m_currid.text=str;
        }

        index=parseInt(factory.dataTree.textTopLevel(6,1));
        if(index<modeArray.length){
            str=modeArray[index];
            m_usrmode.text=str;
        }

    }
    Connections{
        target: driverStatus;
        onTimeOutToQml:{
            updateUiData();
            console.log(qsTr("updata qml ui....")+axisIndex);
        }
    }
}

