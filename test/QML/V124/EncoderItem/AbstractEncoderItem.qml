import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
    id:m_rootAbsEncoderItem;
    property int encType: 1;//1:绝对式 2：增量式 3:无传感
    property var encConfigData: 0x0000;
    property var lineNumber: 0x00020000;
//    property Component control: null;
    function hasError(ecode){
        var ret=false;
        var lost=false;
        if(__lostFlag(ecode)>0)
            lost=true;
        else
            lost=false;
        var alarm=false;
        alarm=_hasAlarmWarnning(ecode&__alarmOper)
        if(lost==true||alarm==true)
            ret=true;
        return ret;
    }
    //子类要根据实际拆分不同的信息
    function _alarmErrorStr(ecode){
        return " ";
    }
    function _hasAlarmWarnning(ecode){
        return false;
    }

    function errorString(ecode){
        var alarm=ecode&__alarmOper;
        var str="LostFlag:"+__lostFlag(ecode)+"  ";
        str+="crcErrCounter:"+__crcErrorCounter(ecode)+" ";
        str+="alarmcode: B["+alarm.toString(2)+"]\n";
        str+=_alarmErrorStr(alarm);
        return str;
    }
    //private
    property var __crcErrCounterOper: 0x7F00;
    property var __lostFlagOper: 0x8000;
    property var __alarmOper: 0x00FF;

    function __lostFlag(ecode){
        var lost=(ecode&__lostFlagOper);
        lost=(lost>>15)&0x0001;
        return lost;
    }
    function __crcErrorCounter(ecode){
        var counter=ecode&__crcErrCounterOper;
        counter>>=8;
        return counter;
    }


    /*RowLayout{
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
            Component.onCompleted: {
                m_lineNumberInput.textChanged.connect(onLinerNumberTextChanged);
            }
        }
        Loader{
            Layout.fillWidth: true;
            sourceComponent: control;
            Layout.minimumWidth: 50;
            Layout.minimumHeight: 40;
        }

    }*/
}

