import QtQuick 2.2
import QtQuick.Layouts 1.1

Rectangle {
    width: 210;
    height: 300;
    color: "#F0F0F0";
//    border.color: "black"
//    radius: height*0.1
    id: root;
    property int curValue: 5;
    property int  __viewItemCount: 5;
    ColumnLayout{
        anchors.fill: parent;
        spacing: 5;
        Text {
            Layout.fillWidth: true
            text: qsTr("寻相力度百分比");
            horizontalAlignment: Text.AlignHCenter;
        }
        PathView {
            id: pathView;
            Layout.fillHeight: true;
            Layout.fillWidth: true;

            interactive: true;
            focus: true;
            pathItemCount: __viewItemCount;
            // keep highlight sit on the middle of Path
            preferredHighlightBegin: 0.5;
            preferredHighlightEnd: 0.5;
            highlightRangeMode: PathView.StrictlyEnforceRange;
            snapMode: PathView.SnapToItem;
    //        snapMode: PathView.SnapOneItem;

            delegate: rectDelegate;
            model: numberListModel;

            path:Path {
                startX: 0;
                startY: root.height/2;
                PathAttribute { name: "itemAlpha"; value: 0.5; }
                PathLine {
                    x: root.width/2;
                    y: root.height/2;
                }
                PathAttribute { name: "itemAlpha"; value: 1; }
                PathLine {
                    x: root.width;
                    y: root.height/2;
                }
                PathAttribute { name: "itemAlpha"; value: 0.5; }
            }

            Keys.onLeftPressed: decrementCurrentIndex();
            Keys.onRightPressed: incrementCurrentIndex();
            Component.onCompleted: {
                pathView.currentIndex=0;
            }
        }
    }



    ListModel{
        id:numberListModel
        Component.onCompleted: {
            for(var i=5;i<35;i+=5){
                append({title:i.toString(),value:i})
            }
        }
    }
    Component {
        id: rectDelegate;
        Item {
            id: wrapper;
            opacity: PathView.itemAlpha;
            Rectangle {
                anchors.centerIn: parent;
                width: root.width/__viewItemCount;
                height: root.height;
                color: "gray"
                border.width: 2;
                border.color: wrapper.PathView.isCurrentItem ? "red" : "transparent";
                Text {
                    anchors.centerIn: parent;
                    font.pixelSize: 16;
                    text: title;
                    color: Qt.lighter(parent.color, 2);
                }
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        wrapper.PathView.view.currentIndex = index;
                        curValue=value;
                        console.log("value="+curValue);
                    }
                }
            }
        }
    }

}
