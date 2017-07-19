import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle{
    color: "white";
    Layout.fillWidth: true;
    Layout.fillHeight: true;
    MouseArea{
        anchors.fill: parent;
        onClicked: console.log("IncrementalEncoder");
    }
}
