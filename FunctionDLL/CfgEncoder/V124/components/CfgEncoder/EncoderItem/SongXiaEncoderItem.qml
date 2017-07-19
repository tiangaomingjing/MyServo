import QtQuick 2.0

AbstractEncoderItem {
    encConfigData:0x0004;
    function _alarmErrorStr(ecode){
        var str=" ";
        if(ecode&0x04)
            str+="Battery alarm \n";
        if(ecode&0x10)
            str+="Battery error \n";
        return str;
    }
    function _hasAlarmWarnning(ecode){
        var ret=false;
        if((ecode&0x0004)||(ecode&0x0010))
            ret=true;
        return ret;
    }

}


