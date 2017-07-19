import QtQuick 2.5
//import com.googoltech.qmlcomponents 1.0
import "./FlowCommon"
import "./CfgPositionLoop"
import "./CfgSpeedLoop"

Item {
    id:limitConfig;
    objectName: "limitConfig";

    function prevExecuteCommand(){
        console.log("limitConfig prev executeCommand");
    }

    function executeCommand(){
        console.log("limitConfig executeCommand");
        var maxV=0.0;
        var negV=0.0;
        var posV=0.0;
        var maxT=0.0;
        var negT=0.0;
        var posT=0.0;
        maxV=parseFloat(m_posLimitBlock.editText1);
        posV=parseFloat(m_posLimitBlock.editText2);
        negV=parseFloat(m_posLimitBlock.editText3);
        maxT=parseFloat(m_speedLimitBlock.absEditText);
        negT=parseFloat(m_speedLimitBlock.negEditText);
        posT=parseFloat(m_speedLimitBlock.posEditText);
        console.log(qsTr("maxV:%1,negV:%2,posV:%3").arg(maxV).arg(negV).arg(posV));
        console.log(qsTr("maxT:%1,negT:%2,posT:%3").arg(maxT).arg(negT).arg(posT));

        m_posLimitBlock.editText1Color="black";
        m_posLimitBlock.editText2Color="black";
        m_posLimitBlock.editText3Color="black";

        m_speedLimitBlock.absEditTextColor="black";
        m_speedLimitBlock.negEditTextColor="black";
        m_speedLimitBlock.posEditTextColor="black";

        var posFilePath="file:///"+sysConfigFilePathSrc+userConfigSrc.configSetting.typeName+
                "/"+userConfigSrc.configSetting.modelName+
                "/"+userConfigSrc.configSetting.version+
                "/PrmFuncPositionloop.xml";
        var velFilePath="file:///"+sysConfigFilePathSrc+userConfigSrc.configSetting.typeName+
                "/"+userConfigSrc.configSetting.modelName+
                "/"+userConfigSrc.configSetting.version+
                "/PrmFuncSpeedloop.xml";

//        mg_servoCmd.writeCommand("gSevDrv.sev_obj.pos.ctl.prm.abs_rat_6",maxV);
//        mg_servoCmd.writeCommand("gSevDrv.sev_obj.pos.ctl.prm.pos_rat_6",posV);
//        mg_servoCmd.writeCommand("gSevDrv.sev_obj.pos.ctl.prm.neg_rat_6",negV);
        mg_servoCmd.writeFunctionValue2Flash(posFilePath,"gSevDrv.sev_obj.pos.ctl.prm.abs_rat_6",maxV);
        mg_servoCmd.writeFunctionValue2Flash(posFilePath,"gSevDrv.sev_obj.pos.ctl.prm.pos_rat_6",posV);
        mg_servoCmd.writeFunctionValue2Flash(posFilePath,"gSevDrv.sev_obj.pos.ctl.prm.neg_rat_6",negV);

//        mg_servoCmd.writeCommand("gSevDrv.sev_obj.vel.ctl.prm.abs_rat_5",maxT);
//        mg_servoCmd.writeCommand("gSevDrv.sev_obj.vel.ctl.prm.pos_rat_5",posT);
//        mg_servoCmd.writeCommand("gSevDrv.sev_obj.vel.ctl.prm.neg_rat_5",negT);
        mg_servoCmd.writeFunctionValue2Flash(velFilePath,"gSevDrv.sev_obj.vel.ctl.prm.abs_rat_5",maxT);
        mg_servoCmd.writeFunctionValue2Flash(velFilePath,"gSevDrv.sev_obj.vel.ctl.prm.pos_rat_5",posT);
        mg_servoCmd.writeFunctionValue2Flash(velFilePath,"gSevDrv.sev_obj.vel.ctl.prm.neg_rat_5",negT);

        return true;
    }
    function aHeadExecuteCommand(){
        console.log("limitConfig aHeadExecuteCommand");
    }
    FilterBlock{
        id:m_limitPositionLoop;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.verticalCenterOffset: -10;
        anchors.left: parent.left;
        anchors.leftMargin: 60;
        width: 80;
        height: 50;
        name.text: "位置环";
        name.font.pixelSize: 16;
    }
    Saturation{
        id:m_limitSaturationSpeed;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: m_limitPositionLoop.right;
        anchors.leftMargin: 60;
        width: 60;
        height: 80;
    }
    FilterBlock{
        id:m_limitSpeedLoop;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.verticalCenterOffset: -10;
        anchors.left: m_limitSaturationSpeed.right;
        anchors.leftMargin: 60;
        width: 80;
        height: 50;
        name.text: "速度环";
        name.font.pixelSize: 16;
    }
    Saturation{
        id:m_limitSaturationCurrent;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: m_limitSpeedLoop.right;
        anchors.leftMargin: 60;
        width: 60;
        height: 80;
    }
    FilterBlock{
        id:m_limitCurrentLoop;
        anchors.verticalCenter: parent.verticalCenter;
        anchors.verticalCenterOffset: -10;
        anchors.left: m_limitSaturationCurrent.right;
        anchors.leftMargin: 60;
        width: 100;
        height: 50;
        name.text: "电流/力矩环";
        name.font.pixelSize: 16;
    }
    PositionLimit{
        id:m_posLimitBlock;
        anchors.horizontalCenter: m_limitSaturationSpeed.horizontalCenter;
        anchors.horizontalCenterOffset: 110;
        anchors.bottom: m_limitSaturationSpeed.top;
        anchors.bottomMargin: -35;
    }
    SpeedLimit{
        id:m_speedLimitBlock;
        height: 220;
        anchors.horizontalCenter: m_limitSaturationCurrent.horizontalCenter;
        anchors.horizontalCenterOffset: -150;
        anchors.top: m_limitSaturationCurrent.bottom;
        anchors.topMargin: -20;
    }

    //箭头
    SegmentArrow{
        id:arrow1;
        points:[Qt.point(0,10),Qt.point(60,10)];
        aText:" ";
        anchors.left:m_limitPositionLoop.right;
        anchors.verticalCenter: m_limitPositionLoop.verticalCenter;
    }
    SegmentArrow{
        id:arrow2;
        points:[Qt.point(0,10),Qt.point(60,10)];
        aText:" ";
        anchors.left:m_limitSaturationSpeed.right;
        anchors.verticalCenter: m_limitSpeedLoop.verticalCenter;
    }
    SegmentArrow{
        id:arrow3;
        points:[Qt.point(0,10),Qt.point(60,10)];
        aText:" ";
        anchors.left:m_limitSpeedLoop.right;
        anchors.verticalCenter: m_limitSpeedLoop.verticalCenter;
    }
    SegmentArrow{
        id:arrow4;
        points:[Qt.point(0,10),Qt.point(60,10)];
        aText:" ";
        anchors.left:m_limitSaturationCurrent.right;
        anchors.verticalCenter: m_limitSpeedLoop.verticalCenter;
    }
    Component.onCompleted: {
        m_posLimitBlock.editText1=100;
        m_posLimitBlock.editText2=100;
        m_posLimitBlock.editText3=100;
        m_speedLimitBlock.absEditText=100;
        m_speedLimitBlock.negEditText=100;
        m_speedLimitBlock.posEditText=100;
    }

}

