import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

    TreeViewStyle{
                id:treeViewStyleId;
                activateItemOnSingleClick: true;
                alternateBackgroundColor: Qt.lighter(backgroundColor);
                backgroundColor: Qt.lighter("gray",1.8);
                branchDelegate:m_branchDelegateStyle;
                highlightedTextColor: "white";
                textColor: "black";
                itemDelegate:Rectangle{
                    height:30;
//                        border.color: "lightgray"
//                        border.width: 1;
                    color:styleData.selected?"gray":itemMouseRetArea.containsMouse?"lightblue":"transparent";
                    Text{
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left;
                        anchors.leftMargin: 10;
                        text:styleData.value;
                        font.pixelSize: 16;
                        color:styleData.selected?"white":"black";
                    }
                    MouseArea{
                        id:itemMouseRetArea;
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onClicked: {
                            console.log("current clicked");
                            mouse.accepted=false;
                        }
                        onPressed: {
                            mouse.accepted=false;
                        }
                        onReleased: {
                            mouse.accepted=false;
                        }
                        onDoubleClicked: {
                            mouse.accepted=false;
                        }
                    }
                }
                rowDelegate: Item{
                    id:m_rowDelegate;
                    width: 200;
                    height: 30;
//                        color:styleData.row%2==0?Qt.lighter("gray",1.6):"white";
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
