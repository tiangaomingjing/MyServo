import QtQuick 2.5

Item {
    width: m_label.width+m_led.width;
    height: Math.max(m_label.height,m_led.height);
    property bool ledOn: true;
    property alias title: m_label.text;
    property string iconPath: "E:/work/Servo/release/Resource/Icon/";

    Image {
        id: m_led;
        anchors.top:parent.top;
        source: ledOn?"file:///"+iconPath+"led_on.png":"file:///"+iconPath+"led_off.png";
    }
    Text{
        id:m_label;
        anchors.left: m_led.right;
        anchors.leftMargin: 10;
        anchors.verticalCenter: m_led.verticalCenter;
        text:"过流";
    }
}

