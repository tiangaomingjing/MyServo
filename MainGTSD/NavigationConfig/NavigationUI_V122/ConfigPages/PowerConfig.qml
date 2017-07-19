import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import com.googoltech.qmlcomponents 1.0
import com.googoltech.qmlcomponents.TreeModel 1.0
import "../mainJs.js" as MainFunctions

Item {
    id:powerConfig;
    objectName: "PowerConfig";

    property string curTypeName: "GTSD_4X";
    property string curModeName: "GTSD41";
    property string curVersion: "V119";
    Component.onCompleted: {
        curTypeName=userConfigSrc.configSetting.typeName;
        curModeName=userConfigSrc.configSetting.modelName;
        curVersion=userConfigSrc.configSetting.version;
    }

    function prevExecuteCommand(){
        console.log("PowerConfig prev executeCommand");
    }

    function executeCommand(){
        console.log("PowerConfig executeCommand");
        return true;
    }
    function aHeadExecuteCommand(){
        console.log("PowerConfig aHeadExecuteCommand");
        //检查配置有没有变化,有变化就重新加载树结构
        var change=false;
        if((userConfigSrc.configSetting.typeName===curTypeName)&&
                (userConfigSrc.configSetting.modelName===curModeName)&&
                (userConfigSrc.configSetting.version===curVersion)){
            change=false;
        }
        else{
            change= true;
        }
        console.log("checkUseConfigChanged:"+change);
        if(change){
            //1加载新的配置
            m_treeModelPower.source="file:///"+sysConfigFilePathSrc+userConfigSrc.configSetting.typeName+
                    "/"+userConfigSrc.configSetting.modelName+
                    "/"+userConfigSrc.configSetting.version+
                    "/ui/NavigationTreeModelFile/Power.ui";
            for(var i=0;i<m_treeModelPower.rowCount();i++){
                m_treePower.expand(m_treeModelPower.index(i,0));
            }
            //2更新
            curTypeName=userConfigSrc.configSetting.typeName;
            curModeName=userConfigSrc.configSetting.modelName;
            curVersion=userConfigSrc.configSetting.version;
            console.log("MotorConfig treemodel reload!");
        }
        mg_servoCmd.updateFunctionTreeModelFromFlash(m_treeModelPower);
        for(var j=0;j<m_treeModelPower.rowCount();j++){
            m_treePower.expand(m_treeModelPower.index(j,0));
        }
    }

    RowLayout{
        anchors.fill: parent;
        anchors.margins: 10;
        spacing: 10;
        TreeView{
            id:m_treePower;
            Layout.minimumWidth: 500;
    //            Layout.fillWidth: true;
            Layout.fillHeight: true;
            model:TreeModel{
                id:m_treeModelPower;
                source: "../../GTSD_4X/GTSD41/V119/ui/NavigationTreeModelFile/Power.ui";
            }
            style:PowerTreeViewStyle{}
            TableViewColumn{
                title: "功率参数";
                width: 200;
                role:"Text9";
            }
            TableViewColumn{
                title: "参数值";
                width: 150;
                role:"Text2";
            }
            TableViewColumn{
                title: "单位";
                width: 100;
                role:"Text4";
            }
            onActivated:{
    //                console.log("clicked activated");
    //                console.log("children count:"+m_treeModelMotor.childrenCount(index));
    //                if(!(m_treeModelMotor.childrenCount(index)>0)){
    //                    console.log(m_treeModelMotor.childrenCount(index));
    //                }
            }
            Component.onCompleted: {
                m_treeModelPower.source="file:///"+sysConfigFilePathSrc+userConfigSrc.configSetting.typeName+
                        "/"+userConfigSrc.configSetting.modelName+
                        "/"+userConfigSrc.configSetting.version+
                        "/ui/NavigationTreeModelFile/Power.ui";
                console.log("cureent tree data"+userConfigSrc.configSetting.version);
                for(var i=0;i<m_treeModelPower.rowCount();i++){
                    m_treePower.expand(m_treeModelPower.index(i,0));
                }
            }
        }

        Rectangle{
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            color:Qt.lighter("gray",1.8);
            border.color: "#BBB9B9";
            radius:5;
            ColumnLayout{
                anchors.fill: parent;
                anchors.margins: 10;
                Image {
                    id: m_powerIcon1
                    anchors.horizontalCenter: parent.horizontalCenter;
                    Layout.maximumWidth: parent.width*0.8;
                    Layout.maximumHeight: 1.6*Layout.maximumWidth;
                    source: "./Resource/Icon/common/powerV.png"
                }
                /*Rectangle{
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    color:m_btnImportPowerParMouse.containsPress?Qt.darker("yellow"):
                          m_btnImportPowerParMouse.containsMouse?Qt.lighter("yellow",1.3):"transparent";
                    border.color: "#BBB9B9";
                    border.width: 1;
                    radius: 5;
                    Text{
                        anchors.centerIn: parent;
                        text:"导入功率参数";
                        font.letterSpacing:5;
                        color:"black";
                        font.bold: true;
                        font.pixelSize: 16;
                    }
                    MouseArea{
                        id:m_btnImportPowerParMouse;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            var count=m_treeModelPower.childrenCount();
                            console.log("connect clicked childrenCount:"+count);
                        }
                    }
                }
                Rectangle{
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    color:m_btnOutPutPowerParMouse.containsPress?Qt.darker("yellow"):
                          m_btnOutPutPowerParMouse.containsMouse?Qt.lighter("yellow",1.3):"transparent";
                    border.color: "#BBB9B9";
                    border.width: 1;
                    radius: 5;
                    Text{
                        anchors.centerIn: parent;
                        text:"导出功率参数";
                        font.letterSpacing:5;
                        color:"black";
                        font.bold: true;
                        font.pixelSize: 16;
                    }
                    MouseArea{
                        id:m_btnOutPutPowerParMouse;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            var count=m_treeModelPower.childrenCount();
                            console.log("connect clicked childrenCount:"+count);
                        }
                    }
                }
                Rectangle{
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    color:m_btnWritePowerParMouse.containsPress?Qt.darker("yellow"):
                          m_btnWritePowerParMouse.containsMouse?Qt.lighter("yellow",1.3):"transparent";
                    border.color: "#BBB9B9";
                    border.width: 1;
                    radius: 5;
                    Text{
                        anchors.centerIn: parent;
                        text:"使能功率参数";
                        font.letterSpacing:5;
                        color:"black";
                        font.bold: true;
                        font.pixelSize: 16;
                    }
                    MouseArea{
                        id:m_btnWritePowerParMouse;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            var count=m_treeModelP     ower.childrenCount();
                            console.log("connect clicked childrenCount:"+count);
                            mg_servoCmd.loadExtendTree("D:/Smart/ServoMaster/project/release/SystemConfiguration/GTSD_4X/GTSD41/V119/PrmFuncExtension.xml")
                            mg_servoCmd.loadCmdTree("D:/Smart/ServoMaster/project/release/SystemConfiguration/GTSD_4X/GTSD41/V119/PrmFuncCmd.xml");
                            mg_servoCmd.writeFunctionTreeModel2Flash(m_treeModelPower);
                        }
                    }
                }*/
            }
        }
    }
}

