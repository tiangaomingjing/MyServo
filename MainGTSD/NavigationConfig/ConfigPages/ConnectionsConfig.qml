import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import com.googoltech.qmlcomponents 1.0
import com.googoltech.qmlcomponents.TreeModel 1.0
import "../mainJs.js" as MainFunctions
import QtQuick.Extras 1.4

Item {
    id:connectionsConfig;
    objectName: "ConnectionsConfig";
    property int  comId: 0;
    property int axisIndex: 0;

    //执行上一步指令
    function prevExecuteCommand(){
        console.log("ConnectionsConfig prev executeCommand");
    }

    //执行下一步指令
    function executeCommand(){
        console.log("ConnectionsConfig executeCommand");
        if(rootMain.mg_connectFlag) return true;
        else return false;
    }

    //提前一步执行指令
    function aHeadExecuteCommand(){
        //根据配置重新更新轴号
        m_axisModel.clear();
        for(var i=0;i<userConfigSrc.configSetting.axisCount;i++){
            m_axisModel.append({"name":qsTr("轴号 %1").arg(i),"index":i});
        }
        console.log("ConnectionsConfig aHeadExecuteCommand");
    }

    RowLayout{
        anchors.fill: parent;
        anchors.margins: 10;
        spacing: 20;
        //-----------------------------左边框------------------------
        Rectangle{
            id:m_connectionLeftBlock;
            width: 400;
            height: 200;
            Layout.fillHeight: true;
            color:Qt.lighter("gray",1.8);
            border.color: "#BBB9B9";
            radius: 5;
            property bool connectFlag: mg_connectFlag;
            ColumnLayout{
                anchors.fill: parent;
                spacing: 10;
                Image {
                    id: icon1
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top;
                    anchors.topMargin: 10;
                    source: "./Resource/Icon/common/net.png"
                }
                Rectangle{
                    Layout.fillWidth: true;
                    height: 30;
                    color:"steelblue";
                    Text{
                        anchors.centerIn: parent;
                        text:"选择连接";
                        font.letterSpacing:5;
                        color:"white";
                        font.bold: true;
                        font.pixelSize: 16;
                    }
                }
                StackView{
                    id:m_connectionStackView;
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    initialItem:comListView;
                    delegate: StackViewDelegate {
                        function transitionFinished(properties)
                        {
                            properties.exitItem.opacity = 1
                        }

                        pushTransition: StackViewTransition {
                            PropertyAnimation {
                                target: enterItem
                                property: "opacity"
                                from: 0
                                to: 1
                            }
                            PropertyAnimation {
                                target: exitItem
                                property: "opacity"
                                from: 1
                                to: 0
                            }
                        }
                    }
                }

            }
        }

        //------------------右边框------------------
        Rectangle{
            width: 200;
            height: 200;
            Layout.fillHeight: true;
            Layout.fillWidth: true;
            color:Qt.lighter("gray",1.8);
            border.color: "#BBB9B9";
            radius: 5;
            ColumnLayout{
                anchors.fill: parent;
                spacing: 12;
                Image {
                    id: icon2
                    Layout.minimumHeight: icon1.height;
                    fillMode: Image.PreserveAspectFit
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top;
                    anchors.topMargin: 10;
                    source: "./Resource/Icon/common/joints.png"
                }
                Rectangle{
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    color:Qt.lighter("gray",1.8);
                    border.color: "#BBB9B9";
                    ColumnLayout{
                        anchors.fill: parent;
                        spacing: 0;
                        Rectangle{
                            Layout.fillWidth: true;
                            height: 30;
                            color:"steelblue";
                            Text{
                                anchors.centerIn: parent;
                                text:"选择轴号";
                                font.letterSpacing:5;
                                color:"white";
                                font.bold: true;
                                font.pixelSize: 16;
                            }
                        }
                        ListView{
                            id:m_axisListView;
                            Layout.fillWidth: true;
                            Layout.fillHeight: true;
                            model:ListModel{
                                id:m_axisModel;
                                ListElement{
                                    name:"轴号 0";
                                    index:0;
                                }
                                ListElement{
                                    name:"轴号 1";
                                    index:1;
                                }
                                ListElement{
                                    name:"轴号 2";
                                    index:2;
                                }
                                ListElement{
                                    name:"轴号 3";
                                    index:3;
                                }
                            }
                            delegate:m_listViewStyle1;
                        }
                    }

                }
            }
        }
    }


    //--------------------左边框数据----------------------
    ListView{
        id:comListView;
        width:120;
        height:300;
        model: ListModel{
            id:comListModel;
            ListElement{
                name:"PcDebug"
                typeid:"0"
                iconSrc:"./Resource/Icon/common/net_ethernet.png"
            }
            ListElement{
                name:"RnNet"
                typeid:"1"
                iconSrc:"./Resource/Icon/common/net_ilink.png"
            }
            ListElement{
                name:"Tcp/Ip"
                typeid:"2"
                iconSrc:"./Resource/Icon/common/net_tcp.png"
            }
        }
        delegate: comListViewDelegate;
        highlight: Rectangle{
            color:mg_selectColor
        }
        Component.onCompleted: comListView.currentIndex=100;
    }
    //------------------comListView 显示代理-------------------------
    Component{
        id:comListViewDelegate;
        Item {
            id: wrapper;
            width: parent.width;
            height:wrapper.ListView.view.height/wrapper.ListView.view.model.count;
            Rectangle{
                anchors.fill: parent;
                color:mouseArea.containsMouse?"lightsteelblue":wrapper.ListView.isCurrentItem?mg_selectColor:"transparent";
                radius: 5;
                RowLayout{
                    spacing: 5;
                    anchors.fill: parent;
                    anchors.margins: 10;
                    Image {
                        anchors.verticalCenter: parent.verticalCenter;
                        Layout.maximumHeight:  wrapper.height*0.7;
                        Layout.maximumWidth:  wrapper.height*0.7;
                        fillMode: Image.PreserveAspectFit
                        source: iconSrc
                    }
                    Item {
                        Layout.fillHeight: true;
                        Layout.fillWidth: true;
                        Text{
                            anchors.fill: parent;
                            anchors.leftMargin: 20;
                            text:name;
                            verticalAlignment: Text.AlignVCenter;
                            font.pixelSize: 16;
                        }
                    }
                }
            }

            MouseArea{
                id:mouseArea
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    wrapper.ListView.view.currentIndex=index;
                    m_connectionStackView.push(comListViews[index]);
                }
            }
        }
    }//------------------comListView 显示代理-------------------------end

    //---------------------每一种通信内的显示界面-----------------------
    function back(){
        m_connectionStackView.pop();
    }

    //裸机网口
    function ethernetConnect(){
        //进度条显示
        m_mainProgressBar.visible=true;
        m_mainProgressBar.value=10;
        mg_servoCmd.delayMs(20);
        connectionsConfig.axisIndex=m_axisListView.currentIndex;
        connectionsConfig.comId=comListModel.get(comListView.currentIndex).typeid;
        mg_servoCmd.axisIndex=m_axisListView.currentIndex;
        mg_servoCmd.comType=connectionsConfig.comId;
        console.log("select com id:"+mg_servoCmd.comType);
        m_mainProgressBar.value=50;
        mg_servoCmd.delayMs(20);

        var connectFlag=mg_servoCmd.connectServo(true);//连接伺服,返回0代表连接上了
        if(connectFlag===0){
            m_mainProgressBar.value=100;
            mg_servoCmd.delayMs(20);
    //        var connectFlag=true;
            console.log("connectFlag:"+connectFlag);
            //查询固件的实际版本，直接设置其当前版本为固件版本
            var ver=0;
            ver=mg_servoCmd.readDeviceVersion();
            console.log(qsTr("V%1").arg(ver));
            var versionString=qsTr("V%1").arg(ver);
            userConfigSrc.setVersion(versionString);
            mg_curVersion=versionString;
            var str=qsTr("current userconfig\nid:%1\ntypename:%2\nmodelname:%3\naxiscount:%4\nversion:%5")
            .arg(userConfigSrc.configSetting.typeId)
            .arg(userConfigSrc.configSetting.typeName)
            .arg(userConfigSrc.configSetting.modelName)
            .arg(userConfigSrc.configSetting.axisCount)
            .arg(userConfigSrc.configSetting.version);
            console.log(str);
            m_mainProgressBar.visible=false;
            return 0;
        }
        else{
            m_mainProgressBar.visible=false;
            return 1;
        }
    }
    function ethernetDisconnect(){
        console.log("disconnect clicked");
        m_mainProgressBar.indeterminate=false;
        m_mainProgressBar.visible=false;
        var connectFlag=mg_servoCmd.connectServo(false);//断开连接伺服
        return connectFlag;
    }

    property Item ethernetView: ControlView{
        id:ethernet;
        property bool ledActive: false;
        control: Rectangle{
            color:Qt.lighter("gray",1.8);
            StatusIndicator{
                id:ethernetConnectLed;
                anchors.centerIn: parent;
                anchors.verticalCenterOffset: -30;
                width: 40;
                height: 40;
                color:"green";
                active: ethernet.ledActive;
            }
            Text{
                anchors.top: ethernetConnectLed.bottom;
                anchors.topMargin: 10;
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.centerIn: parent;
                text:"PcDebug"
                font.pixelSize: 16;
            }

        }

        onBackBtnClicked: back();
        onConnectBtnClicked:{
            var connectFlag=ethernetConnect();

            //界面逻辑
            if(connectFlag===0){
                //连接上
                mg_connectFlag=true;

                MainFunctions.reloadControlCmdFile();

                console.log("connect");

                ledActive=true;
                ethernet.isConnected=true;
                m_mainWarningMessageItem.visible=false;
            }
            else{
                //连接不上
                mg_servoCmd.connectServo(false);
                mg_connectFlag=false;
                ledActive=false;
                ethernet.isConnected=false;
                console.log("unable connect");
                m_mainWarningMessageItem.visible=true;
                m_mainWarningMessageText.text=qsTr("连接失败，请检查通信端口");
            }
        }

        onDisConnectBtnClicked:{
            //界面逻辑
            var connectFlag=ethernetDisconnect();
            if(connectFlag===0){
                mg_connectFlag=false;
                ledActive=false;
                console.log("disconnect");
                m_mainWarningMessageItem.visible=false;
            }
        }
    }

    //等环网
    function gLink2Connect(){
        //进度条显示
        m_mainProgressBar.visible=true;
        m_mainProgressBar.value=10;
        mg_servoCmd.delayMs(20);
        connectionsConfig.axisIndex=m_axisListView.currentIndex;
        connectionsConfig.comId=comListModel.get(comListView.currentIndex).typeid;
        mg_servoCmd.axisIndex=m_axisListView.currentIndex;
        mg_servoCmd.comType=connectionsConfig.comId;
        mg_servoCmd.comStation=ilink.controlItem.m_stationNumber;
        console.log("select com id:"+mg_servoCmd.comType);
        console.log("select com station:"+mg_servoCmd.comStation);
        m_mainProgressBar.value=50;
        mg_servoCmd.delayMs(20);

        var connectFlag=mg_servoCmd.connectServo(true);//连接伺服,返回0代表连接上了
        if(connectFlag===0){
    //        var connectFlag=true;
            console.log("connectFlag:"+connectFlag);
            m_mainProgressBar.value=100;
            mg_servoCmd.delayMs(20);
            //查询固件的实际版本，直接设置其当前版本为固件版本
            var ver=0;
            ver=mg_servoCmd.readDeviceVersion();
            console.log(qsTr("V%1").arg(ver));
            var versionString=qsTr("V%1").arg(ver);
            userConfigSrc.setVersion(versionString);
            mg_curVersion=versionString;
            var str=qsTr("current userconfig\nid:%1\ntypename:%2\nmodelname:%3\naxiscount:%4\nversion:%5")
            .arg(userConfigSrc.configSetting.typeId)
            .arg(userConfigSrc.configSetting.typeName)
            .arg(userConfigSrc.configSetting.modelName)
            .arg(userConfigSrc.configSetting.axisCount)
            .arg(userConfigSrc.configSetting.version);
            console.log(str);
            m_mainProgressBar.visible=false;
            return 0;//连接成功
        }
        else{
            m_mainProgressBar.visible=false;
            return 1;//没有连接成功
        }
    }
    function gLink2Disconnect(){
        console.log("disconnect clicked");
        m_mainProgressBar.indeterminate=false;
        m_mainProgressBar.visible=false;
        var connectFlag=mg_servoCmd.connectServo(false);//断开连接伺服
        return connectFlag;
    }

    property Item gLinkView: ControlView{
        id:ilink;
        property bool ledActive: false;
        control: Rectangle{
            color:Qt.lighter("gray",1.8);
            property int m_stationNumber: parseInt(stationNumber.text)
            StatusIndicator{
                id:glinkConnectLed;
                anchors.centerIn: parent;
                anchors.verticalCenterOffset: -60;
                width: 40;
                height: 40;
                color:"green";
                active: ilink.ledActive;
            }
            Text{
                id:glink2Text
                anchors.top: glinkConnectLed.bottom;
                anchors.topMargin: 10;
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.centerIn: parent;
                text:"RnNet"
                font.pixelSize: 16;
            }
            Row{
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.top: glink2Text.bottom;
                anchors.topMargin: 20;
                spacing: 20;
                Text{
                    text:qsTr("Station Number:");
                    font.pixelSize: 16;
                    verticalAlignment: Text.AlignVCenter;
                }
                TextField{
                    id:stationNumber;
                    text: "240";
                    width: 100;
                    height: 24;
                }
            }

        }

        onBackBtnClicked: back();
        onConnectBtnClicked:{
            var connectFlag=gLink2Connect();

            //界面逻辑
            if(connectFlag===0){
                //连接上
                MainFunctions.reloadControlCmdFile();
                mg_connectFlag=true;

                console.log("connect");

                ledActive=true;
                ilink.isConnected=true;
                m_mainWarningMessageItem.visible=false;
            }
            else{
                //连接不上
                mg_servoCmd.connectServo(false);
                mg_connectFlag=false;
                ledActive=false;
                ilink.isConnected=false;
                console.log("unable connect");
                m_mainWarningMessageItem.visible=true;
                m_mainWarningMessageText.text=qsTr("连接失败，请检查通信端口");
            }
        }
        onDisConnectBtnClicked:{
            var connectFlag=gLink2Disconnect();
            if(connectFlag===0){
                mg_connectFlag=false;
                ledActive=false;
                ilink.isConnected=false;
                console.log("disconnect");
                m_mainWarningMessageItem.visible=false;
            }
        }
    }

    //TcpIP
    property Item tcpIpView:ControlView{
        id:tcpip;
        control: Rectangle{
            color:Qt.lighter("gray",1.8);
            Text{
            anchors.centerIn: parent;
            text:"tcpIpView"
            }
        }

        onBackBtnClicked: back();
    }
    property var comListViews: [ethernetView,gLinkView,tcpIpView]
    //---------------------每一种通信内的显示界面-----------------------end


    //-------------------右边框数据-----------------------------------
    Component{
        id:m_listViewStyle1;
        Item{
            id:wrapper1;
            width: m_axisListView.width;
            height: m_axisListView.height/m_axisModel.count;
            Rectangle{
                id:wrapperRect;
                anchors.fill: parent;
//                color:m_mouseWrapperRect.containsMouse?"lightblue":"transparent"
                gradient:Gradient{
                    GradientStop{position: 1;color:m_mouseWrapperRect.containsMouse?"lightsteelblue":wrapper1.ListView.isCurrentItem?Qt.darker("gray",1.8):"transparent"}
                    GradientStop{position: 0;color:m_mouseWrapperRect.containsMouse?"lightsteelblue":wrapper1.ListView.isCurrentItem?Qt.lighter("gray"):"transparent"}
                }

                RowLayout{
                    anchors.fill: parent;
                    anchors.leftMargin: 20;
                    anchors.rightMargin: 20;
                    spacing: 20;
                    Image {
                        id: wrapperIcon1;
                        Layout.maximumHeight: wrapperRect.height;
                        Layout.maximumWidth: height;
                        height: parent.height;
                        width:height;
                        source: "./Resource/Icon/common/motor3d.png"
                    }
                    Text{
                        id:wrapperText1;
                        Layout.fillHeight: true;
                        Layout.fillWidth: true;
                        verticalAlignment: Text.AlignVCenter;
                        text:name;
                        font.pixelSize: 16;
                        color:wrapper1.ListView.isCurrentItem?"white":"black"
                    }
                }
                MouseArea{
                    id:m_mouseWrapperRect;
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onClicked: {
                        m_axisListView.currentIndex=index;
                        mg_servoCmd.axisIndex=index;//更新命令的轴
                        mg_currentAxisIndex=index;
                        console.log(index);
                    }
                }
            }
        }
    }

}

