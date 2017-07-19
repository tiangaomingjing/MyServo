import QtQuick 2.0

AbstractEncoderItem {
    encConfigData:0x0002;
    function _alarmErrorStr(ecode){
        return " ";
    }
    function _hasAlarmWarnning(ecode){
        return false;
    }

}
