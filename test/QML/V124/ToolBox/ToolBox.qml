import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle {
    id:m_toolBoxRoot;
    width: 300;
    height: 400;
    property int __lheight: height-2*m_tool1.btnHeight_RO;
    signal titleClicked(int index);
    signal itemClicked(int index);
    Column{
        anchors.fill: parent;
        ToolItem{
            id:m_tool1;
            index:0;
            loaderSource: "AbsoluteEncoder.qml"
            btnText: qsTr("绝对式")
            actived: true;
            width: parent.width;
            height: actived?__lheight:50;
            onBtnClicked: {
                __onItemClicked(idx);
            }

//            Layout.fillHeight: true;
//            Layout.fillWidth: true;
        }
        ToolItem{
            id:m_tool2;
            index:1;
            loaderSource:"IncrementalEncoder.qml";
            btnText: qsTr("增量式")
            actived: false;
            width: parent.width;
            height: actived?__lheight:50;
            onBtnClicked: {
                __onItemClicked(idx);
            }
        }
        ToolItem{
            id:m_tool3;
            index:2;
            loaderSource:"NoneEncoder.qml";
            btnText: qsTr("无传感式")
            actived: false;
            width: parent.width;
            height: actived?__lheight:50;
            onBtnClicked: {
                __onItemClicked(idx);
            }
        }
//        Rectangle{
//            color: "red";
//            Layout.fillHeight: true;
//            Layout.fillWidth: true;
//        }
    }
    Component.onCompleted: {
        m_tool1.controlItem.selectEncOwner.connect(itemClicked);
//        var i=0x8004;
//        var il2=i<<2;
//        var ir2=i>>2;
//        console.log("<<2 ="+il2);
//        console.log(">>2 ="+ir2);
    }

    function __onItemClicked(index){
        m_tool1.show(index);
        m_tool2.show(index);
        m_tool3.show(index);
        console.log(qsTr("encoder index=%1").arg(index));
        emit:titleClicked(index);
    }
}

