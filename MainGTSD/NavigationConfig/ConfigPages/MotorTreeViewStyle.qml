import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import com.googoltech.qmlcomponents.TreeModel 1.0

TreeViewStyle{
//            id:treeViewMotorStyleId;
    activateItemOnSingleClick: true;
    alternateBackgroundColor: Qt.lighter(backgroundColor);
    backgroundColor: Qt.lighter("gray",1.8);
    branchDelegate:m_branchDelegateStyle;
    highlightedTextColor: "white";
    textColor: "black";
    itemDelegate:Rectangle{
        id:itemDelegatRrect1;
        height:30;
        property bool isEdit: false;
//                        border.color: "lightgray"
//                        border.width: 1;
        color:styleData.selected?"gray":itemMouseRetArea.containsMouse?"lightblue":"transparent";
        Text{
            id:m_textShowItem1;
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left;
            anchors.leftMargin: 0;
            width: parent.width;
            height:parent.height;
            text:styleData.value;
            font.pixelSize: 16;
            verticalAlignment: Text.AlignVCenter;
            color:styleData.selected?"white":"black";
            visible: !itemDelegatRrect1.isEdit;
            onVisibleChanged: {
                if((styleData.column===1)&&(!styleData.hasChildren)){
                    var str=m_treeMotor.model.data(styleData.index,TreeModel.TEXT2);
                    if(str!==undefined)
                        text=str;
                }
            }

        }
        TextField{
            id:m_textFieldItem1;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 0;
            width: parent.width-anchors.leftMargin;
            height: parent.height*0.8;
            visible: itemDelegatRrect1.isEdit;
            font.pixelSize: 16;
            activeFocusOnPress: true;
            onEditingFinished: {
                console.log("now value :"+text);
                m_treeMotor.model.setDataByColumn(styleData.index,text,styleData.column);
                itemDelegatRrect1.isEdit=false;
                console.log("editing finish......"+styleData.index.column+"value:"+m_treeMotor.model.data(styleData.index,TreeModel.TEXT2));
            }
//                    onVisibleChanged: {
//                        if(visible) {text=String(m_treeMotor.model.data(styleData.index,TreeModel.TEXT2));}
//                    }
            onTextChanged: {
                motorConfig.editValueChanged(styleData.index);
                console.log("text changed :"+text+"  index "+styleData.index);
            }
            Component.onCompleted: {
                text=String(m_treeMotor.model.data(styleData.index,TreeModel.TEXT2));
            }

        }
        MouseArea{
            id:itemMouseRetArea;
            anchors.fill: parent;
            propagateComposedEvents: true;
            hoverEnabled: true;
            onClicked: {
                console.log("column click:"+styleData.column);
//                            if((styleData.column===1)&&(!styleData.hasChildren))//等于第2列，是叶子节点
//                                itemDelegatRrect1.isEdit=true;
//                            mouse.accepted=false;
            }
            onPressed: {
                console.log("column PressedClick:"+styleData.column);
                if((styleData.column===1)&&(!styleData.hasChildren))//等于第2列，是叶子节点
                    itemDelegatRrect1.isEdit=true;
                mouse.accepted=false;
            }
            onReleased: {
                console.log("column ReleasedClick:"+styleData.column);
//                                mouse.accepted=false;
            }
            onDoubleClicked: {
                console.log("column DoubleClicked:"+styleData.column);
//                            mouse.accepted=false;
            }
        }
    }
    rowDelegate: Item{
        id:m_rowDelegate;
        width: 200;
        height: 30;
//                    color:(m_tree.model.childrenCount(m_tree.currentIndex)>0)?Qt.lighter("gray",1.6):"white";
    }
    headerDelegate: Rectangle{
        height: 30;
        visible: true;
        color:"steelblue"
        Text{
            anchors.centerIn: parent;
            text:styleData.value;
            font.pixelSize: 16;
            color:"white"
            font.bold: true;
        }
    }
    Component{
        id:m_branchDelegateStyle
        Item{
            width: 40;
            height: 20;
            Rectangle{
                id:ret;
                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter;
                width: 20;
                height: 20;
                color:m_mouseRet.containsMouse?"yellow":"transparent"
                radius:5;
                border.width: 1;
                border.color: "gray";
                MouseArea{
                    id:m_mouseRet;
                    anchors.fill: parent;
                    hoverEnabled: true;
                    propagateComposedEvents: true;
                    onClicked: {
                        mouse.accepted=false;
                    }
                    onPressed: {
                        mouse.accepted=false;
                    }
                    onReleased: {
                        mouse.accepted=false;
                    }
                }

                Text{
                    anchors.centerIn: parent;
                    text:styleData.isExpanded?"-":"+";
                    font.pixelSize: 24;
                    font.bold: true;
                }
            }
        }
    }
}
