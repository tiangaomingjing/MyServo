import QtQuick 2.5

Rectangle {
    id:filterBlock;
    color:m_mouse.containsMouse?Qt.rgba(0.97,0.145,0.145,0.5):Qt.lighter(border.color,1.2);
    border.color: "#BBB9B9";
    border.width: 2;
    radius: 10;
    width: 150;
    height: 100;
    property alias name: tex;
    signal clicked();
    Text{
        id:tex;
        anchors.centerIn: parent;
        text:"button"
    }
    MouseArea{
        id:m_mouse;
        anchors.fill: parent;
        hoverEnabled: true;
        onClicked: {
            filterBlock.clicked();
        }
    }
}

