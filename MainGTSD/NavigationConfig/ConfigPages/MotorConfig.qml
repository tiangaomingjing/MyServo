import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import com.googoltech.qmlcomponents 1.0
import com.googoltech.qmlcomponents.TreeModel 1.0
import "../mainJs.js" as MainFunctions
import QmlGlobalClass 1.0

Item {
    id:motorConfig;
    objectName: "MotorConfig";
    signal editValueChanged(var index);
    property bool updateServoDataEnable: false;//只有参数有修改时，点下一步才写到驱动器
    property int curSelectAxis: -1;
    property bool valueChangeFlag: false;
    property int ppnValue;
    property var chineseNameMap: {
        "Imax":"最大电流","Irat":"额定电流","Sct":"额定转速","Srat":"过速百分比","Nos":"最大转速","Rm":"电阻","Ldm":"D轴电感",
        "Lqm":"Q轴电感","Jm":"电机转动惯量","Jrat":"惯量比","Fm":"摩擦系数","PPN":"极对数",
        "Tqr":"额定转矩","PHIm":"反电动势系数","Vmax":"最大电压"
    }
    property var unitNameMap: {
        "Imax":"A","Irat":"A","Sct":"rpm","Srat":"%","Nos":"rpm","Rm":"Ohm	","Ldm":"mH",
        "Lqm":"mH","Jm":"10^-6.kg.m^2","Jrat":"%","Fm":"10^-4.N.m/(rad/s)","PPN":"对",
        "Tqr":"N.m","PHIm":"mV/rpm","Vmax":"V"
    }
    property var controlNameMap: {
        "Imax":"gSevDrv.sev_obj.cur.mot.Imax_1","Irat":"gSevDrv.sev_obj.cur.mot.Irat_1",
        "Sct":"gSevDrv.sev_obj.cur.mot.Sct_1","Srat":"gSevDrv.sev_obj.cur.mot.Srat_1",
        "Nos":"gSevDrv.sev_obj.cur.mot.Nos_1","Rm":"gSevDrv.sev_obj.cur.mot.Rm_1",
        "Ldm":"gSevDrv.sev_obj.cur.mot.Ldm_1","Lqm":"gSevDrv.sev_obj.cur.mot.Lqm_1",
        "Jm":"gSevDrv.sev_obj.cur.mot.Jm_1","Jrat":"gSevDrv.sev_obj.cur.mot.Jrat_1",
        "Fm":"gSevDrv.sev_obj.cur.mot.Fm_1","PPN":"gSevDrv.sev_obj.cur.mot.PPN_1",
        "Tqr":"gSevDrv.sev_obj.cur.mot.Tqr_1","PHIm":"gSevDrv.sev_obj.cur.mot.PHIm_1",
        "Vmax":"gSevDrv.sev_obj.cur.mot.Vmax_1"
    }
    property bool saveEnable: false;//逻辑：1新建时saveEnable=true 2点击到其它库的电机时，回车发现改变=true 3保存后=false
    property bool saveDialogShowFlag: false;
    property bool leftViewShowFlag: false;

    function prevExecuteCommand(){
        console.log("MotorConfig prev executeCommand");
    }

    function executeCommand(){
        console.log("MotorConfig executeCommand");
        var ppn;
        ppn=findMotorPPN();//用于编码器模块计算
        rootMain.mg_ppnValue=ppn;
        console.log("ppn number:"+rootMain.mg_ppnValue);

        if(updateServoDataEnable){
            var controlName="";
            var setValue=0.0;
            var motorFilePath="file:///"+sysConfigFilePathSrc+userConfigSrc.configSetting.typeName+
                    "/"+userConfigSrc.configSetting.modelName+
                    "/"+userConfigSrc.configSetting.version+
                    "/PrmFuncMotor.xml";

            for(var i=0;i<m_motorPrmModel.count;i++){
                controlName=m_motorPrmModel.get(i).controlKey;
                setValue=parseFloat(m_motorPrmModel.get(i).value);
                var retValue=mg_servoCmd.writeFunctionValue2Flash(motorFilePath,controlName,setValue);
                console.log(qsTr("setValue:%1 \tretValue:%2").arg(setValue).arg(retValue));
            }
            updateServoDataEnable=false;
        }
        return true;
    }
    function aHeadExecuteCommand(){
        console.log("MotorConfig aHeadExecuteCommand");
        //检查选择的轴号有没有变
        if(mg_servoCmd.axisIndex!==curSelectAxis){
            console.log(qsTr("检查选择的轴号有没有变"));
            curSelectAxis=mg_servoCmd.axisIndex;
            //从驱动器更新界面数值
            var motorFilePath="file:///"+sysConfigFilePathSrc+userConfigSrc.configSetting.typeName+
                    "/"+userConfigSrc.configSetting.modelName+
                    "/"+userConfigSrc.configSetting.version+
                    "/PrmFuncMotor.xml";

            var columnCount=m_motorTableModel.recordColumnCount();
            console.log(qsTr("rec:columnCount %1").arg(columnCount));
            m_motorPrmModel.clear();
            for(var j=SqlTableModel.MotorImax;j<columnCount;j++){
                var name=m_motorTableModel.recordFieldName(j);
                var unit=unitNameMap[name];
                var controlKey=controlNameMap[name];
                var chineseName=chineseNameMap[name];
                var val="0";
                console.log(qsTr("rec:name %1,value:%2, chineseName:%3").arg(name).arg(val).arg(chineseName));
                //jsobject  role-name
                m_motorPrmModel.append({"chineseName":chineseName,"value":val,"unit":unit,"name":name,"controlKey":controlKey});
            }

            var retValue=0.0;
            var controlName="";
            for(var i=0;i<m_motorPrmModel.count;i++){
                controlName=m_motorPrmModel.get(i).controlKey;

                retValue=mg_servoCmd.readFunctionValueFromFlash(motorFilePath,controlName);
                console.log(qsTr("contorlKey:%1 \tvalue:%2").arg(controlName).arg(retValue.toFixed(2)));
                m_motorPrmModel.setProperty(i,"value",qsTr("%1").arg(retValue.toFixed(2)));
            }

        }
    }

//------------------从listView中找到电机的极对数----------------
    function findMotorPPN(){
        var ppnIndex=11;
        return parseInt(m_motorPrmModel.get(ppnIndex).value);
    }
//------------------从listView中找到电机的极对数--------------end

    property string curTypeName: "GTSD_4X";
    property string curModeName: "GTSD41";
    property string curVersion: "V119";

    RowLayout{
        anchors.fill: parent;
        anchors.margins: 10;
        spacing: 20;
        //-------------------------左边的框-----------------------
        Rectangle{
            implicitWidth: 300;
            Layout.fillHeight: true;
            color:Qt.lighter("gray",1.8);
            border.color: "#BBB9B9";
            radius: 5;
            visible: leftViewShowFlag;
            StackView{
                id:motorNavigationStackView;
                anchors.fill: parent;
                anchors.margins: 5;
                initialItem: m_mainCompany;
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

        //----------------------------右边框--------------------------------
        //--电机详细参数表---
        Rectangle{
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            color:Qt.lighter("gray",1.8);
            border.color: "#BBB9B9";
            radius: 5;

            ColumnLayout{
                anchors.fill: parent;
                anchors.margins: 5;
                Rectangle{
                    Layout.fillWidth: true;
                    height: 30;
                    color:"steelblue";
                    Text{
                        anchors.centerIn: parent;
                        text:"输入电机参数";
                        font.letterSpacing:10;
                        color:"white";
                        font.bold: true;
                        font.pixelSize: 16;
                    }
                }
                ListView{
                    id:m_listViewMotorPrm;
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                    anchors.margins: 5;
                    model:m_motorPrmModel;
                    delegate: motorPrmDelegate;
                    clip: true;
                }

                //保存 新建按钮
                Item{
                    id:newAndSaveRect;
                    Layout.fillWidth: true;
                    Layout.minimumHeight: 30;
                    height: 30;
                    visible: !saveDialogShowFlag;
                    property color backgroundColor: Qt.darker("gray",1.2);
                    RowLayout{
                        anchors.fill: parent;
                        spacing: 10;
                        //重新安装
                        Rectangle{
                            radius: 5;
                            Layout.fillHeight: true;
                            Layout.fillWidth: true;
                            color:installMotorBtn.containsMouse||installMotorBtn.pressed?Qt.darker(newAndSaveRect.backgroundColor,2):newAndSaveRect.backgroundColor;
                            Text {
                                anchors.centerIn: parent;
                                text: qsTr("重新安装")
                                font.pixelSize: 16;
                                color:"white"
                            }
                            MouseArea{
                                id:installMotorBtn;
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onClicked: {
                                    leftViewShowFlag=!leftViewShowFlag;
                                }
                            }
                        }
                        //新建按钮
                        Rectangle{
                            radius: 5;
                            Layout.fillHeight: true;
                            Layout.fillWidth: true;
                            color:newMotorBtn.containsMouse||newMotorBtn.pressed?Qt.darker(newAndSaveRect.backgroundColor,2):newAndSaveRect.backgroundColor;
                            Text {
                                anchors.centerIn: parent;
                                text: qsTr("新建电机库")
                                font.pixelSize: 16;
                                color:"white"
                            }
                            MouseArea{
                                id:newMotorBtn;
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onClicked: {
                                    //更新电机参数model
                                    var columnCount=m_motorTableModel.recordColumnCount();
                                    console.log(qsTr("rec:columnCount %1").arg(columnCount));
                                    m_motorPrmModel.clear();
                                    for(var j=SqlTableModel.MotorImax;j<columnCount;j++){
                                        var name=m_motorTableModel.recordFieldName(j);
                                        var unit=unitNameMap[name];
                                        var controlKey=controlNameMap[name];
                                        var chineseName=chineseNameMap[name];
                                        var val='0';
                                        console.log(qsTr("rec:name %1,value:%2, chineseName:%3").arg(name).arg(val).arg(chineseName));
                                        m_motorPrmModel.append({"chineseName":chineseName,"value":val,"unit":unit,"name":name,"controlKey":controlKey});
                                    }
                                    saveEnable=true;
                                }
                            }
                        }
                        //保存按钮
                        Rectangle{
                            radius: 5;
                            Layout.fillHeight: true;
                            Layout.fillWidth: true;
                            enabled: saveEnable;
                            color:saveMotorBtn.containsMouse||saveMotorBtn.pressed?Qt.darker(newAndSaveRect.backgroundColor,2):newAndSaveRect.backgroundColor;
                            Text {
                                anchors.centerIn: parent;
                                text: qsTr("增加至用户库")
                                font.pixelSize: 16;
                                color:parent.enabled?"white":"black"
                            }
                            MouseArea{
                                id:saveMotorBtn;
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onClicked: {
                                    saveDialogShowFlag=true;
                                    saveEnable=false;
                                    motorInputName.text=qsTr("Motor_xxxx");
                                }
                            }
                        }
                    }
                }
                //保存对话框
                Item{
                    id:saveDialog;
                    Layout.fillWidth: true;
                    Layout.minimumHeight: 30;
                    height: 30;
                    visible: saveDialogShowFlag
                    RowLayout{
                        anchors.fill: parent;
                        spacing: 10;
                        Text{
                            text:qsTr("输入电机名称");
                            font.pixelSize: 16;
                            verticalAlignment: Text.AlignVCenter;
                        }

                        TextField{
                            id:motorInputName;
                            placeholderText:"请输入你的电机名称......"
                            style:TextFieldStyle{
                                textColor: "black";
                                placeholderTextColor:"lightgray"
                                background: Rectangle{
                                    radius: 5;
                                    implicitWidth: 180;
                                    implicitHeight: 24;
                                    border.color: "#333";
                                    border.width: 1;
                                }
                            }
                        }
                        Rectangle{
                            radius: 5;
                            Layout.fillHeight: true;
                            Layout.fillWidth: true;
//                            implicitWidth: (newAndSaveRect.width-newAndSaveRect.height)/2
                            color:saveOkBtn.containsMouse||saveOkBtn.pressed?Qt.darker(newAndSaveRect.backgroundColor,2):newAndSaveRect.backgroundColor;
                            Text {
                                anchors.centerIn: parent;
                                text: qsTr("确定")
                                font.letterSpacing: 5;
                                font.pixelSize: 16;
                                color:"white"
                            }
                            MouseArea{
                                id:saveOkBtn;
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onClicked: {
                                    saveDialogShowFlag=false;
                                    saveEnable=false;
                                    //写入数据库
                                    var rowCount=m_motorNameListTableModel.rowCount();
                                    console.log(qsTr("company count:%1").arg(companyTableModel.rowCount()));
                                    m_motorNameListTableModel.insertRows(rowCount,1);
                                    m_motorNameListTableModel.setData(
                                                m_motorNameListTableModel.index(rowCount,SqlTableModel.MotorName),motorInputName.text);
                                    m_motorNameListTableModel.setData(
                                                m_motorNameListTableModel.index(rowCount,SqlTableModel.MotorCompanyId),companyTableModel.rowCount());
                                    var columnCount=m_motorPrmModel.count;
                                    for(var i=0;i<columnCount;i++){
                                        var value=m_motorPrmModel.get(i).value;
                                        console.log("value "+value);
                                        m_motorNameListTableModel.setData(
                                                    m_motorNameListTableModel.index(rowCount,i+SqlTableModel.MotorImax),value);
                                    }
                                    m_motorNameListTableModel.submitAll();
                                }
                            }
                        }
                        Rectangle{
                            radius: 5;
                            Layout.fillHeight: true;
                            Layout.fillWidth: true;
//                            implicitWidth: (newAndSaveRect.width-newAndSaveRect.height)/2
                            color:saveCancelBtn.containsMouse||saveCancelBtn.pressed?Qt.darker(newAndSaveRect.backgroundColor,2):newAndSaveRect.backgroundColor;
                            Text {
                                anchors.centerIn: parent;
                                text: qsTr("取消")
                                font.pixelSize: 16;
                                font.letterSpacing: 5;
                                color:"white"
                            }
                            MouseArea{
                                id:saveCancelBtn;
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onClicked: {
                                    saveDialogShowFlag=false;
                                    saveEnable=true;
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    //------------------------------左边框数据------------------------------
    SqlTableModel{
        id:companyTableModel;
    }
    SqlTableModel{
        id:m_motorTableModel;//具体的某一个电机
    }
    SqlTableModel{
        id:m_motorNameListTableModel;//电机列表
    }


    ListModel{
        id:m_companyListModel;
    }
    ListModel{
        id:m_motorPrmModel;
    }

    //---------------公司表-------------------------
    //当currentIndex改变时，从设置新搜索条件更新电机库
    //当点击某一项事件 :
    //1)进入当前公司的电机库  push()方法在显示代理中处理
    //2)同时设置当前的电机库ListView index=0
    //3)并更新电机参数列表 timer->start()
    Rectangle{
        id:m_mainCompany;
        width: 200;
        height: 300;
        color:Qt.lighter("gray",1.8);

        ListView{
            anchors.fill: parent;
            id:m_listViewCompany;
            model:m_companyListModel;
            delegate: companyDelegate;
            highlight: Rectangle{
                color:"gray";
            }
            onCurrentIndexChanged: {
                console.log(qsTr("公司表currentIndexChange"));
                console.log("currenID:"+m_companyListModel.get(currentIndex).Id);
                m_motorNameListTableModel.setFilter(qsTr("CompanyId=%1").arg(m_companyListModel.get(currentIndex).Id));

            }
            Component.onCompleted:{
                //更新公司列表模型参数
                companyTableModel.setTable("Company");
                companyTableModel.select();
                var count=companyTableModel.rowCount();
                console.log("count:"+count);
                var i=0;
                for(;i<count;i++){
                    var companyName=companyTableModel.recordValueAt(i,SqlTableModel.CompanyName);
                    var companyId=companyTableModel.recordValueAt(i,SqlTableModel.CompanyId);
                    console.log("name:"+companyName);
                    m_companyListModel.append({"name":companyName,"Id":companyId});
                }


                //获得电机列表
                m_motorNameListTableModel.setTable("Motor");
                m_motorNameListTableModel.setEditStrategy(SqlTableModel.OnManualSubmit);
                m_motorNameListTableModel.setFilter(qsTr("CompanyId=%1").arg(count));//筛选出最后一个公司
                m_motorNameListTableModel.select();

                currentIndex=count-1;

                var motorCount=m_motorNameListTableModel.rowCount();//最后一个公司下一共有多少个电机
                var currentMotorId=0;
                currentMotorId=m_motorNameListTableModel.recordValueAt(motorCount-1,"Id");//最后公司中最后一个电机记录Id
                var currentCompanyId=m_companyListModel.get(currentIndex).Id;

                var filter=qsTr("CompanyId=%1 AND Id=%2").arg(currentCompanyId).arg(currentMotorId);
                m_motorTableModel.setTable("Motor");
                m_motorTableModel.setFilter(filter);
                m_motorTableModel.select();

                console.log(qsTr("m_motorNameListTableModel.rowCount() %1").arg(motorCount));

                //更新电机详细参数model
                if(m_motorTableModel.rowCount()>0){
                    var columnCount=m_motorTableModel.recordColumnCount();
                    console.log(qsTr("rec:columnCount %1").arg(columnCount));
                    m_motorPrmModel.clear();
                    for(var j=SqlTableModel.MotorImax;j<columnCount;j++){
                        var name=m_motorTableModel.recordFieldName(j);
                        var unit=unitNameMap[name];
                        var controlKey=controlNameMap[name];
                        var chineseName=chineseNameMap[name];
                        var val=String(m_motorTableModel.recordValueAt(0,j));
                        console.log(qsTr("rec:name %1,value:%2, chineseName:%3").arg(name).arg(val).arg(chineseName));
                        //jsobject  role-name
                        m_motorPrmModel.append({"chineseName":chineseName,"value":val,"unit":unit,"name":name,"controlKey":controlKey});
                    }
                }

            }
        }
    }
    //公司表显示代理
    Component{
        id:companyDelegate;
        Item{
            id:compannyWrapper;
            width: parent.width;
            height: 40;
            Rectangle{
                anchors.fill: parent;
                color:compannyWrapperMouseArea.containsMouse?"lightblue":compannyWrapper.ListView.isCurrentItem?mg_selectColor:Qt.lighter("gray",1.8);
                Row{
                    anchors.fill: parent;
                    spacing: 30;
                    Image {
                        id: companyIcon
                        anchors.verticalCenter: parent.verticalCenter;
                        height: 20;
                        width: 20;
                        fillMode: Image.PreserveAspectFit
                        source: "./Resource/Icon/common/company.png"
                    }
                    Text{
                        text:name;
                        anchors.verticalCenter: parent.verticalCenter;
                        height: 24;
                        width: parent.width-companyIcon.width;
                        verticalAlignment: Text.AlignVCenter;
                        font.pixelSize: 16;
                    }
                }
            }
            MouseArea{
                id:compannyWrapperMouseArea;
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    compannyWrapper.ListView.view.currentIndex=index;
                    motorNavigationStackView.push(m_motorListRect);
                    m_listViewMotorName.currentIndex=0;
                    timer.start();
                    saveEnable=false;
                }
            }
        }
    }

    //---------------电机列表------------------------
    Rectangle{
        id:m_motorListRect
        color:Qt.lighter("gray",1.8);
        width: 200;
        height: 300;
        visible: false;
        ColumnLayout{
            anchors.fill: parent;
            spacing: 0
            ListView{
                id:m_listViewMotorName;
                Layout.fillWidth: true;
                Layout.fillHeight: true;
                model:m_motorNameListTableModel;
                delegate: motorNameDelegate;
                clip:true;
                highlight: Rectangle{
                    color:mg_selectColor;
                }
                Component.onCompleted:{
                    currentIndex=0;
                }
                // 滚动条
                Rectangle {
                    id: scrollbar
                    anchors.right: parent.right;
                    anchors.top: parent.top;
                    width: 15;
                    height: parent.height;
                    color: "#ccbfbf"
                    property var fullViewHeight: m_listViewMotorName.currentItem.height*m_motorNameListTableModel.rowCount();
                    visible: fullViewHeight<m_listViewMotorName.height?false:true
                    // 按钮
                    Rectangle {
                        id: button
                        x: 0
                        y: m_listViewMotorName.visibleArea.yPosition * scrollbar.height
                        width: scrollbar.width
                        height: m_listViewMotorName.visibleArea.heightRatio * scrollbar.height;
                        color: mouseArea.containsMouse||mouseArea.pressed?"lightblue":"gray";
                        radius: 10
                        visible: parent.visible;
                        // 鼠标区域
                        MouseArea {
                            id: mouseArea
                            anchors.fill: button
                            hoverEnabled: true;
                            drag.target: button
                            drag.axis: Drag.YAxis
                            drag.minimumY: 0
                            drag.maximumY: scrollbar.height - button.height
                            // 拖动
                            onMouseYChanged: {
                                m_listViewMotorName.contentY = button.y / scrollbar.height * m_listViewMotorName.contentHeight
                            }
                        }
                    }
                }

            }
            //回退框
            Rectangle{
                id:backRect;
                Layout.fillWidth: true;
                height: 30;
                color:Qt.darker("gray",1.2);
                Text{
                    anchors.centerIn: parent;
                    text:m_companyListModel.get(m_listViewCompany.currentIndex).name;
                    color: "white"
                    font.pixelSize: 16;
                }

                Rectangle{
                    anchors.left: parent.left;
                    anchors.top: parent.top;
                    anchors.bottom: parent.bottom;
                    width: height;
                    color:backBtn.containsMouse||backBtn.pressed?Qt.darker(parent.color,1.89):parent.color;
                    Image {
                        anchors.centerIn: parent;
                        width: height*0.7;
                        height: parent.height*0.7;
                        source: "./Resource/Icon/common/icon-back.png"
                    }
                    MouseArea{
                        id:backBtn;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            motorNavigationStackView.pop()
                        }
                    }
                }
            }
        }
        Component.onDestruction: {
            console.log("m_motorListView destruction")
        }
    }
    //电机列表显示代理
    Component{
        id:motorNameDelegate;
        Item{
            id:motorNameWrapper;
            width: parent.width;
            property int itemHeight: 40;
            height: itemHeight;
            Rectangle{
                anchors.fill: parent;
                color:motorNameWrapperMouseArea.containsMouse?"lightblue":motorNameWrapper.ListView.isCurrentItem?mg_selectColor:Qt.lighter("gray",1.8);
                Item{
                    id:focusItem;
                    anchors.left: parent.left;
                    anchors.verticalCenter: parent.verticalCenter;
                    height: parent.height;
                    width: parent.width-60;
                    Image {
                        id:imgMotor;
                        anchors.left: parent.left;
                        anchors.verticalCenter: parent.verticalCenter;
                        width: itemHeight;
                        height:  itemHeight;
                        source: "./Resource/Icon/common/motor3d.png"
                    }
                    Text{
                        id:motorNameText;
                        anchors.left: imgMotor.right;
                        anchors.leftMargin: 30;
                        anchors.verticalCenter: parent.verticalCenter;
                        height:  itemHeight;
                        text:MotorName;
                        font.pixelSize: 16;
                        verticalAlignment: Text.AlignVCenter;
                    }
                    MouseArea{
                        id:motorNameWrapperMouseArea;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            motorNameWrapper.ListView.view.currentIndex=index;
                            timer.start();
                            saveEnable=false;
                        }
                    }
                }
                Loader {
                    id: loaderDelete
                    anchors.left:focusItem.right;
                    anchors.verticalCenter: parent.verticalCenter;
                    width: 60;
                    height:  itemHeight;
                    sourceComponent: (m_listViewCompany.currentIndex===m_companyListModel.count-1)&&(index!==0) ? editor : null
                    Component {
                        id: editor
                        Rectangle{
                            anchors.fill: parent;
                            width: 60;
                            height: 30;
                            color: loaderMouse.containsMouse?Qt.lighter("red"):"transparent"
                            Image {
                                anchors.centerIn: parent;
                                height: 20;
                                width: 20;
                                id: deleteMotorImg;
                                fillMode: Image.PreserveAspectFit
                                source: "./Resource/Icon/common/delete.png";
                            }
                            MouseArea{
                                id:loaderMouse;
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onClicked: {
                                    if(m_motorNameListTableModel.rowCount()>0){
                                        motorNameWrapper.ListView.view.currentIndex=index;
                                        console.log("index :"+index);
                                        m_motorNameListTableModel.removeRows(index,1);
                                        m_motorNameListTableModel.submitAll();
                                        console.log("delete record ....");
                                    }

                                }
                            }
                        }
                    }
                }
            }
        }
    }


    //------------------------------右边框数据------------------------------
    //电机参数显示代理
    Component{
        id:motorPrmDelegate;
        Item{
            id:motorPrmWrapper;
            width: parent.width;
            height: 24;
            Rectangle{
                anchors.fill: parent;
                color:motorPrmWrapperMouseArea.containsMouse?"lightblue":motorPrmWrapper.ListView.isCurrentItem?mg_selectColor:Qt.lighter("gray",1.8);
                Text{
                    id:motorPrmName;
                    anchors.left: parent.left;
                    anchors.top: parent.top;
                    text:chineseName;
                    font.pixelSize: 16;
                    height: 24;
                    width: parent.width/3;
                    verticalAlignment: Text.AlignVCenter;
                }
                Loader {
                    id: loaderEditor
                    anchors.left: motorPrmName.right;
                    anchors.leftMargin: -10;
                    anchors.top: parent.top;
                    anchors.margins: 0
                    width: 0;
                    height: motorPrmValue.height;
                    sourceComponent: motorPrmValue.focus ? editor : null
                    Component {
                        id: editor
                        Rectangle{
                            color:"white"
                            radius: 5;
                            Behavior on width {
                                NumberAnimation{
                                    easing.type: Easing.Linear
                                    duration: 100;
                                }
                            }
                        }
                    }
                    onSourceComponentChanged: {
                        if(loaderEditor.item!==null){
                            loaderEditor.item.width=motorPrmValue.width;
                        }
                    }
                }
                TextInput{
                    id:motorPrmValue;
                    anchors.left: motorPrmName.right;
                    anchors.top: parent.top;
                    text:value;
                    font.pixelSize: 16
                    height: 24;
                    width: parent.width/3;
                    verticalAlignment: Text.AlignVCenter;
                    validator: DoubleValidator{}
                    selectByMouse: true;

                    onEditingFinished: {
                        text=m_motorPrmModel.get(index).value;
                        console.log("onEditingFinished "+m_motorPrmModel.get(index).value);
                    }
                    onAccepted: {
                        var prevValue=m_motorPrmModel.get(index).value;
                        console.log("typeof "+typeof prevValue);
                        if(prevValue!==text){
                            saveEnable=true;
                            updateServoDataEnable=true;//只有参数有修改时，点下一步才写到驱动器
                        }

                        m_motorPrmModel.setProperty(index,"value",motorPrmValue.text);
                        motorPrmValue.focus=false;//退出编辑状态
                        console.log("onAccepted "+m_motorPrmModel.get(index).value);
                    }
                }

                Text{
                    id:motorPrmUnit;
                    anchors.left: motorPrmValue.right;
                    anchors.top: parent.top;
                    text:unit;
                    font.pixelSize: 16;
                    height: 24;
                    width: parent.width/3;
                    verticalAlignment: Text.AlignVCenter;
                }
                MouseArea{
                    id:motorPrmWrapperMouseArea;
                    anchors.fill: parent;
                    hoverEnabled: true;
                    propagateComposedEvents: true
                    onPressed: {
                        motorPrmWrapper.ListView.view.currentIndex=index;
                        mouse.accepted=false;
                    }
                }
            }
        }
    }



    //------------------------------信号与槽--------------------------------
    Connections{
        target: motorConfig;
        onEditValueChanged:{
            console.log("slot:"+index);
            valueChangeFlag=true;
        }
    }


    Timer{
        id:timer;
        interval: 100;
        repeat: false;
        triggeredOnStart: false;
        onTriggered: {
            var motorCount=m_motorNameListTableModel.rowCount();

            if(motorCount>0){
                var currentMotorId=0;
                currentMotorId=m_motorNameListTableModel.recordValueAt(m_listViewMotorName.currentIndex,"Id");
                var companyId=m_companyListModel.get(m_listViewCompany.currentIndex).Id;

                var filter=qsTr("CompanyId=%1 AND Id='%2'").arg(companyId).arg(currentMotorId);

                m_motorTableModel.setFilter(filter);
                m_motorTableModel.select();

                console.log(currentMotorId);

                //更新电机参数model
                if(m_motorTableModel.rowCount()>0){
                    var columnCount=m_motorTableModel.recordColumnCount();
                    console.log(qsTr("rec:columnCount %1").arg(columnCount));
                    m_motorPrmModel.clear();
                    for(var j=SqlTableModel.MotorImax;j<columnCount;j++){
                        var name=m_motorTableModel.recordFieldName(j);
                        var unit=unitNameMap[name];
                        var controlKey=controlNameMap[name];
                        var chineseName=chineseNameMap[name];
                        var val=String(m_motorTableModel.recordValueAt(0,j));
                        console.log(qsTr("rec:name %1,value:%2, chineseName:%3").arg(name).arg(val).arg(chineseName));
                        m_motorPrmModel.append({"chineseName":chineseName,"value":val,"unit":unit,"name":name,"controlKey":controlKey});
                    }
                }

                updateServoDataEnable=true;//允许下一步时写电机能数
            }
        }
    }
}

