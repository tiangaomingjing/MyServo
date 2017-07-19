import QtQuick 2.2

Rectangle {
    id:colorPicker;
    width: 50;
    height: 30;
    signal colorPicker(color clr);
    function configureBorder(){
        colorPicker.border.width=colorPicker.focus?5:0;
        colorPicker.border.color=colorPicker.focus?"#90D750":"#808080";
    }
    MouseArea{
        anchors.fill: parent;
        onClicked: {
            colorPicker.colorPicker(colorPicker.color);
        }
    }
    Keys.onReturnPressed: {
        colorPicker.colorPicker(colorPicker.color);
        event.accepted=ture;
    }
    Keys.onSpacePressed: {
        colorPicker.colorPicker(colorPicker.color);
        event.accepted=ture;
    }

    Component.onCompleted: {
        configureBorder();
    }
    onFocusChanged: {
       configureBorder();
    }

}

