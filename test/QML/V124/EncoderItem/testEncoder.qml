//import QtQuick 2.0
//import QtQuick.Controls 1.4
//import QtQuick.Extras 1.4

//Rectangle {
//    width: 220
//    height: 350
//    color: "#F0F0F0"

//    ListModel {
//        id: listModel
//    }

//    Tumbler {
//        anchors.centerIn: parent

//        TumblerColumn {
//            model: listModel
//            role: "value"
//        }
//        Component.onCompleted: {
//            for(var i=5;i<35;){
//                listModel.append({value:i.toString()});
//                i+=5;
//            }
//        }
//    }
//}

import QtQuick 2.2

Rectangle {
    width: 210;
    height: 300;
    color: "#F0F0F0";
    id: root;
    property int  __viewItemCount: 5;
    property int curValue: 5;

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

    PathView {
        id: pathView;
        anchors.fill: parent;
        interactive: true;
        pathItemCount: __viewItemCount;
        // keep highlight sit on the middle of Path
        preferredHighlightBegin: 0.5;
        preferredHighlightEnd: 0.5;
        highlightRangeMode: PathView.StrictlyEnforceRange;
        //snapMode: PathView.SnapToItem;
        snapMode: PathView.SnapOneItem;

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

        focus: true;
        Keys.onLeftPressed: decrementCurrentIndex();
        Keys.onRightPressed: incrementCurrentIndex();
    }
    ListModel{
        id:numberListModel
        Component.onCompleted: {
            for(var i=5;i<35;i+=5){
                append({title:i.toString(),value:i})
            }
        }
    }

}


//import QtQuick 2.2

//Rectangle {
//    width: 480;
//    height: 300;
//    color: "black";
//    id: root;

//    Component {
//        id: rectDelegate;
//        Item {
//            id: wrapper;
//            z: PathView.zOrder;
//            opacity: PathView.itemAlpha;
//            scale: PathView.itemScale;
//            Rectangle {
//                //anchors.centerIn: parent;
//                width: 100;
//                height: 60;
//                color: Qt.rgba(Math.random(), Math.random(), Math.random(), 1);
//                border.width: 2;
//                border.color: wrapper.PathView.isCurrentItem ? "red" : "lightgray";
//                Text {
//                    anchors.centerIn: parent;
//                    font.pixelSize: 28;
//                    text: index;
//                    color: Qt.lighter(parent.color, 2);
//                }
//                MouseArea {
//                    anchors.fill: parent;
//                    onClicked: {
//                        wrapper.PathView.view.currentIndex = index;
//                    }
//                }
//            }
//        }
//    }

//    PathView {
//        id: pathView;
//        anchors.fill: parent;
//        interactive: true;
//        pathItemCount: 7;
//        // keep highlight sit on the middle of Path
//        preferredHighlightBegin: 0.5;
//        preferredHighlightEnd: 0.5;
//        highlightRangeMode: PathView.StrictlyEnforceRange;
//        //snapMode: PathView.SnapToItem;
//        snapMode: PathView.SnapOneItem;

//        delegate: rectDelegate;
//        model: 15;

//        path:Path {
//            startX: 10;
//            startY: 100;
//            PathAttribute { name: "zOrder"; value: 0; }
//            PathAttribute { name: "itemAlpha"; value: 0.1; }
//            PathAttribute { name: "itemScale"; value: 0.6; }
//            PathLine {
//                x: root.width/2 - 40;
//                y: 100;
//            }
//            PathAttribute { name: "zOrder"; value: 10; }
//            PathAttribute { name: "itemAlpha"; value: 1; }
//            PathAttribute { name: "itemScale"; value: 1.2; }
//            //PathPercent { value: 0.28; }
//            PathLine {
//                //x: root.width - 100;
//                //y: 100;
//                relativeX: root.width/2 - 60;
//                relativeY: 0;
//            }
//            PathAttribute { name: "zOrder"; value: 0; }
//            PathAttribute { name: "itemAlpha"; value: 0.1; }
//            PathAttribute { name: "itemScale"; value: 0.6; }
//        }

//        focus: true;
//        Keys.onLeftPressed: decrementCurrentIndex();
//        Keys.onRightPressed: incrementCurrentIndex();
//    }
//}



