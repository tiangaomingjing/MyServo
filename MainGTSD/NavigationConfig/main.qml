import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import "./ConfigPages"
import QmlGlobalClass 1.0
import com.googoltech.qmlcomponents 1.0

Rectangle {
    id:rootMain
    visible: true
    color: Qt.rgba(0.9,0.9,1,0.4)
    width: 1000;
    height: 600;
    border.color: "white";
    border.width: 2;
    radius: 6;
    objectName: "rootMain";
    signal close();
    signal finishAll();
    //---------global value---------
    //1 userConfigSrc
    property bool mg_hasSelectUserConfig: false;//有没有选择相关配置
    property bool mg_connectFlag: false;//连接标志位
    property int mg_ppnValue: 5;//极对数
    property color mg_selectColor: Qt.darker("lightgray",1.3)

    property int mg_curTypeId: 0;
    property string mg_curTypeName: "GTSD_4X";
    property string mg_curModeName: "GTSD41";
    property string mg_curVersion: "V119";
    property int mg_curAxisCount: 4;

    property int mg_currentAxisIndex: 0;//当前轴

    //指令控制集
    ServoCmd{
        id:mg_servoCmd;
    }
    ControlName{
        id:mg_controlKeyName;
    }

    //界面title
    Text{
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 20;
        text:"欢迎使用设置向导专家"
        font.letterSpacing: 10;
        font.bold: true;
        color:"black"
    }

    //------------------拖动响应--------------
    MouseArea {
        id:dragRegion;
        property point clickPos: "0,0";
        anchors.fill: parent
        onPressed: {
            clickPos=Qt.point(mouse.x,mouse.y);
        }
        onPositionChanged: {
            var delta=Qt.point(mouse.x-clickPos.x,mouse.y-clickPos.y);
            quickViewWindowSrc.setX(quickViewWindowSrc.x+delta.x);
            quickViewWindowSrc.setY(quickViewWindowSrc.y+delta.y);
        }
    }
    //---------------------主界面布局-------------------------
    ColumnLayout{
        anchors.fill: parent;
        anchors.topMargin: 50;
        anchors.leftMargin: 20;
        anchors.rightMargin: 20;
        anchors.bottomMargin: 20;
        spacing: 10;
        RowLayout{
            spacing: 10;
            Layout.fillWidth: true;
            //-----------------------导航框-------------------------
            Rectangle{
                Layout.fillHeight: true;
                width: 200;
                color:"lightgray";
                border.color: "gray"
                border.width: 1;
                radius: 10;
                MouseArea{
                    anchors.fill: parent;
                    onClicked: {
                        console.log("inner ret clicked");
                    }
                }
                ListView{
                    anchors.fill: parent;
                    anchors.margins: 20;
                    id:m_listView;
                    delegate: delegateStyle;
                    model: theModel;
                    highlight: highlightBar;
                    highlightFollowsCurrentItem: true;
                }
                Text{
                    anchors.bottom: servoIcon.top;
                    anchors.bottomMargin: 10;
                    anchors.horizontalCenter: servoIcon.horizontalCenter;
                    text:qsTr("当前轴:%1").arg(mg_currentAxisIndex);
                    font.pixelSize: 16;
                }

                Image {
                    id: servoIcon
                    anchors.bottom: parent.bottom;
                    anchors.bottomMargin: 10;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    source: "./ConfigPages/Resource/Icon/common/servo_device.png"
                }

            }
            //----------------------------------工作框------------------------
            Rectangle{
                Layout.fillWidth: true;
                Layout.fillHeight: true;
                color:"lightgray";
                border.color: "gray"
                border.width: 1;
                radius: 10;
                StackView{
                    id:m_stackView;
                    anchors.fill: parent;
                    initialItem: m_servoConfig;
                }
            }
        }

        //--------------视图切换按钮框--------------
        RowLayout{
            Layout.fillWidth: true;
            spacing: 10;
            Item{
                Layout.fillWidth: true;
                height: 40;
                //提示信息
                //警告
                Item{
                    id:m_mainWarningMessageItem;
                    visible: false;
                    anchors.left: parent.left;
                    anchors.leftMargin: 30;
                    anchors.verticalCenter: btnRowLayout.verticalCenter;
                    Image {
                        id:warningIcon;
                        anchors.left: parent.left;
                        anchors.verticalCenter: parent.verticalCenter;
                        source: "./ConfigPages/Resource/Icon/common/servoerror.png"
                    }
                    Rectangle{
                        anchors.left: warningIcon.right;
                        anchors.leftMargin: 30;
                        anchors.verticalCenter: parent.verticalCenter;
                        height: 20;
                        width: 200;
                        color:Qt.lighter("gray",1.8);
                        Text{
                            anchors.fill: parent;
                            id:m_mainWarningMessageText;
                            text:"警告......";
                            verticalAlignment: Text.AlignVCenter;
                        }
                    }
                }

                //进度条
                ProgressBar{
                    id:m_mainProgressBar;
                    anchors.verticalCenter: btnRowLayout.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: 0;
                    minimumValue: 0;
                    maximumValue: 100;
                    value:0;
                    width:550;
                    height: 20;
                    visible:false;
                    style: ProgressBarStyle {
                        background: Rectangle {
                            radius: 2
                            color: "lightgray"
                            border.color: "gray"
                            border.width: 1
                            implicitWidth: 200
                            implicitHeight: 24
                        }
                        progress: Rectangle {
                            color: "steelblue"
                            border.color: "steelblue"
                        }
                    }


                }
                Text{
                    id:m_mainProgressTextValue;
                    anchors.left: m_mainProgressBar.right;
                    anchors.leftMargin: 10;
                    anchors.verticalCenter: m_mainProgressBar.verticalCenter;
                    font.pixelSize: 16;
                    visible: false;
                    text:m_mainProgressBar.value+"%";
                }

                Row{
                    id:btnRowLayout
                    anchors.right: parent.right;
                    anchors.rightMargin: 20;
                    anchors.bottom: parent.bottom;
                    anchors.bottomMargin: 10;
                    spacing: 20;
                    //连接状态
//                    Image {
//                        id: m_connectStatusIcon;
////                        anchors.bottom: servoIcon.top;
////                        anchors.bottomMargin: 5;
////                        anchors.horizontalCenter: parent.horizontalCenter
//                        source: "./ConfigPages/Resource/Icon/common/disconnect.png";
//                        height: 30;
//                        width: 55;
//                    }
                    Rectangle{
                        id:btn_cancel;
                        width: 50;
                        height: 30;
                        border.color: "#BBB9B9"
                        border.width: 1;
                        color: cancelClick.containsMouse?"red":rootMain.color;
                        radius: 5;
                        Text{
                            anchors.centerIn: parent;
                            text:"取消"
                            font.pixelSize: 16;
                        }
                        MouseArea{
                            id:cancelClick;
                            anchors.fill: parent
                            hoverEnabled: true;
                        }
                    }
                    Rectangle{
                        id:btn_home;
                        width: 50;
                        height: 30;
                        border.color: "#BBB9B9"
                        border.width: 1;
                        color: homeClick.containsMouse?"yellow":rootMain.color;
                        radius: 5;
                        Text{
                            anchors.centerIn: parent;
                            text:"Home"
                            font.pixelSize: 16;
                        }
                        MouseArea{
                            id:homeClick;
                            anchors.fill: parent
                            hoverEnabled: true;
                        }
                    }
                    Rectangle{
                        id:btn_nextAxis;
                        width: 50;
                        height: 30;
                        border.color: "#BBB9B9"
                        border.width: 1;
                        color: nextAxisClick.containsMouse?"yellow":rootMain.color;
                        radius: 5;
                        visible: (m_listView.currentIndex==m_listView.count-1)?true:false;
                        Text{
                            anchors.centerIn: parent;
                            text:"下一轴"
                            font.pixelSize: 16;
                        }
                        MouseArea{
                            id:nextAxisClick;
                            anchors.fill: parent
                            hoverEnabled: true;
                        }
                    }
                    Rectangle{
                        id:btn_previous;
                        width: 50;
                        height: 30;
                        border.color: "#BBB9B9"
                        border.width: 1;
                        color: previousClick.containsMouse?"yellow":rootMain.color;
                        radius: 5;
                        Text{
                            anchors.centerIn: parent;
                            text:"上一步"
                            font.pixelSize: 16;
                        }
                        MouseArea{
                            id:previousClick;
                            anchors.fill: parent
                            hoverEnabled: true;
                        }
                    }
                    Rectangle{
                        id:btn_next;
                        width: 50;
                        height: 30;
                        border.color: "#BBB9B9"
                        border.width: 1;
                        color: nextClick.containsMouse?"yellow":rootMain.color;
                        radius: 5;
                        enabled: true;
                        Text{
                            anchors.centerIn: parent;
                            text:(m_listView.currentIndex==m_listView.count-1)?"完成":"下一步";
                            font.pixelSize: 16;
                        }
                        MouseArea{
                            id:nextClick;
                            anchors.fill: parent
                            hoverEnabled: true;
                        }
                    }

                }
            }
        }
    }

    //-------------------------------close button---------------------
    //要置于MouseArea之后，否则无法响应鼠标点击
    Rectangle{
        id:closeBtn
        height: 25
        width: 35
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.top: parent.top
        anchors.topMargin: 2;
//        color:"#aaff0000"
        color:colseClick.containsMouse?"red":Qt.rgba(0.9,0.9,1,0.8);
        Text{
            text:"X"
            anchors.centerIn: parent
            font.pixelSize: 16;
        }
        MouseArea{
            id:colseClick;
            anchors.fill: parent
            hoverEnabled: true;
            onClicked:
            {
                //Qt.quit()无法关闭窗口
                quickViewWindowSrc.close();
                rootMain.close();
            }
        }
    }

    //---------------向导导航数据模型listview delegate model----------------------
    ListModel{
        id:theModel;
        ListElement{checked:true;name:qsTr("驱动器信息")}
        ListElement{checked:false;name:qsTr("通讯连接")}
        ListElement{checked:false;name:qsTr("电机设置")}
        ListElement{checked:false;name:qsTr("功率等级")}
        ListElement{checked:false;name:qsTr("阀值限定")}
        ListElement{checked:false;name:qsTr("反馈设置")}
        ListElement{checked:false;name:qsTr("试运行")}
    }
    // Define a highlight with customized movement between items.
    Component {
        id: highlightBar
        Rectangle {
            width: m_listView.width; height: 50
            color: "steelblue"
            radius: 5;
            y: m_listView.currentItem.y;
            Behavior on y { NumberAnimation {duration: 200} }
        }
    }
    Component{
        id:delegateStyle;
        Item{
            id:wrapper;
            width: m_listView.width;
            height: 40;
            Image {
                id: m_icon
                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;
                width: 40;
                height: 40;
                source:checked?"./ConfigPages/Resource/Icon/common/checkbox_checked.png":"./ConfigPages/Resource/Icon/common/checkbox_unchecked.png";
            }
            Text{
                id:tex;
                anchors.left: m_icon.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;
                text:name;
                color:wrapper.ListView.isCurrentItem?"white":"black";
                font.pixelSize: wrapper.ListView.isCurrentItem?16:mouse_area.containsMouse?18:16;
                font.bold: wrapper.ListView.isCurrentItem?true:false;
                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignHCenter;
            }
            MouseArea{
                id:mouse_area;
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
//                    wrapper.ListView.view.currentIndex=index;
                    console.log(tex.text+" "+m_listView.currentIndex);
                }
            }
        }
    }

    //---------------listview delegate model----------------------end

    //-----------------每一个子页面------------------
    property var m_servoConfig: ServoConfig{}
    property var m_connectionsConfig: ConnectionsConfig{}
    property var m_motorConfig: MotorConfig{}
    property var m_powerConfig: PowerConfig{}
    property var m_limitConfig: LimitConfig{}
    property var m_encoderConfig: EncoderConfig{}
    property var m_motionConfig: MotionConfig{}
    property var pages: [m_servoConfig,m_connectionsConfig,
                         m_motorConfig,m_powerConfig,
                         m_limitConfig,m_encoderConfig,m_motionConfig]
    property var warningMessagesList: ["-------请先选择驱动器配置-------",
                                       "-------请先连接驱动器----------",
                                       " ",
                                       "",
                                       " ",
                                       "-------请先找到电机初始相位角-------"]
    function showWarningMessage(index){
        m_mainWarningMessageText.text=warningMessagesList[index];
    }

    //------------------信号与槽---------------------------
    //btn 取消
    Connections{
        target: cancelClick;
        onClicked:{
            if(mg_connectFlag)
                mg_servoCmd.connectServo(false);
            rootMain.close();
        }
    }
    //btn home
    Connections{
        target: homeClick;
        onClicked:{
            m_listView.currentIndex=0;
            m_stackView.pop(pages[0]);
            for(var i=1;i<m_listView.count;i++){
                m_listView.model.get(i).checked=false;
            }
            m_encoderConfig.stopTimer();
        }
    }
    //下一轴
    Connections{
        target: nextAxisClick;
        onClicked:{
            m_listView.currentIndex=1;
            m_stackView.pop(pages[1]);
            for(var i=2;i<m_listView.count;i++){
                m_listView.model.get(i).checked=false;
            }
            m_encoderConfig.resetInitialPhase();
        }
    }
    //btn 上一个
    Connections{
        target: previousClick;
        onClicked:{
            var curIndex=m_listView.currentIndex;
            if(curIndex>0){
                m_listView.model.get(curIndex).checked=false;
                pages[curIndex].prevExecuteCommand();
                m_listView.currentIndex=curIndex-1;
                m_stackView.pop(pages[m_listView.currentIndex]);
            }
        }
    }
    //btn 下一个
    Connections{
        target: nextClick;
        onClicked:{
            var curIndex=m_listView.currentIndex;
            console.log("curIndex:"+curIndex+"   count:"+m_listView.count)

            if(curIndex<m_listView.count-1){
                var ret=pages[curIndex].executeCommand();
                if(ret){
                    m_mainWarningMessageItem.visible=false;
                    pages[curIndex+1].aHeadExecuteCommand();
                    m_listView.model.get(curIndex+1).checked=true;
                    m_listView.currentIndex=curIndex+1;
                    m_stackView.push(pages[m_listView.currentIndex]);
                }
                //有错误信息
                else{
                    m_mainWarningMessageItem.visible=true;
                    showWarningMessage(curIndex);
                }

                console.log(pages[m_listView.currentIndex].objectName);
            }
            else if(curIndex===(m_listView.count-1)){
                console.log("finish");
                emit:finishAll();
            }
        }
    }

    //连接主界面信息，获取完成更新状态
//    Connections{
//        target: mainWindowSrc;
//        onUpdateProgressBar:{
//            console.log("the progress:"+value);
//            m_mainProgressBar.value=value;
//            if(value>0&&value<100){
//                m_mainProgressTextValue.visible=true;
//                m_mainProgressBar.visible=true;
//            }

//            if(value>=100){
//                m_mainProgressBar.visible=false;
//                m_mainProgressTextValue.visible=false;
//            }
//        }
//    }

    //界面初始化
    Component.onCompleted: {
        /*mg_servoCmd.loadExtendTree("file:///"+sysConfigFilePathSrc+mg_curTypeName+"/"+mg_curModeName+"/"+mg_curVersion+"/PrmFuncExtension.xml")
        mg_servoCmd.loadCmdTree("file:///"+sysConfigFilePathSrc+mg_curTypeName+"/"+mg_curModeName+"/"+mg_curVersion+"/PrmFuncCmd.xml");
        mg_servoCmd.loadAdvanceFastAccessCommandTree("file:///"+sysConfigFilePathSrc+mg_curTypeName+"/"+mg_curModeName+"/"+mg_curVersion+"/AdvanceFastCommand.xml");
        m_motionConfig.startTimer.connect(m_encoderConfig.onStartTimer);//从试运行往回切换到编码盘时打开更新时钟*/
        var type=userConfigSrc.configSetting.typeName;
        var model=userConfigSrc.configSetting.modelName;
        var version=userConfigSrc.configSetting.version;
        mg_servoCmd.loadExtendTree("file:///"+sysConfigFilePathSrc+type+"/"+model+"/"+version+"/PrmFuncExtension.xml")
        mg_servoCmd.loadCmdTree("file:///"+sysConfigFilePathSrc+type+"/"+model+"/"+version+"/PrmFuncCmd.xml");
        mg_servoCmd.loadAdvanceFastAccessCommandTree("file:///"+sysConfigFilePathSrc+type+"/"+model+"/"+version+"/AdvanceFastCommand.xml");
        m_motionConfig.startTimer.connect(m_encoderConfig.onStartTimer);//从试运行往回切换到编码盘时打开更新时钟
        mg_controlKeyName.source="./ConfigPages/Resource/DeviceData/"+type+"/"+model+"/"+version+"/ControlName.json";
    }

    Component.onDestruction: {
        console.log("release navigation dialog->");
    }
}

