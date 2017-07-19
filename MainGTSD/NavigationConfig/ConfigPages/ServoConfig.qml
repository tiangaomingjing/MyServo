import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.XmlListModel 2.0
import com.googoltech.qmlcomponents 1.0
import com.googoltech.qmlcomponents.TreeModel 1.0
import "servoConfigJs.js" as ServoConfigFunctions
import "../mainJs.js" as MainFunctions

Item {
    id:servoConfig;
    objectName: "ServoConfig";
    property string m_servoInformationText: " "

    function executeCommand(){
        console.log("ServoConfig executeCommand");
        //检测有没有选择用户配置
        if(rootMain.mg_hasSelectUserConfig){
//            ServoConfigFunctions.saveUserConfig();
            MainFunctions.saveUserConfig();
            return true;
        }
        else{
            return false;
        }
    }

    //--------------------------界面布局-------------------------------------------
    RowLayout{
        anchors.fill: parent;
        anchors.margins: 10;
        spacing: 20;
        //-----------------------左边框---------------------------------------------
        Rectangle{
            width: 300;
            height: 200;
            Layout.fillHeight: true;
            color:Qt.lighter("gray",1.8);
            border.color: "#BBB9B9";
            radius: 5;
            ColumnLayout{
                anchors.fill: parent;
                spacing: 10;
                Image {
                    id: icon1
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "./Resource/Icon/common/configdialog1.png"
                }

                ListView{
                    id:m_servoConfigTypesListView;
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    spacing: 4;
                    focus: true;
                    highlight: Rectangle{
                        width: parent.width;
                        color:"gray";
                    }
                    model:servoTypeDataModel;
                    delegate: servoConfigListViewDelegate;

                    section.property: "type";//为其type分类
                    section.criteria: ViewSection.FullString;
                    section.delegate: servoTypeSectionHeader;
                    onCurrentIndexChanged: {
                        var model=servoTypeDataModel.get(currentIndex).model;
                        var type=servoTypeDataModel.get(currentIndex).type;

                        servoIcon.source="./Resource/Icon/servo/"+type+"/"+model+"/img.png";

                        //更新信息描述
                        ServoConfigFunctions.updateDecribeString();
                        //缓存配置参数，基中V119为默认，后面连接时再修改
                        mg_curTypeId=servoTypeDataModel.get(currentIndex).typeId;
                        mg_curTypeName=servoTypeDataModel.get(currentIndex).type;
                        mg_curModeName=servoTypeDataModel.get(currentIndex).model;
                        mg_curAxisCount=servoTypeDataModel.get(currentIndex).axisCount;
                        mg_curVersion=userConfigSrc.configSetting.version;

                        //已经选择了配置
                        rootMain.mg_hasSelectUserConfig=true;

                        console.log(qsTr("\nId:%1\nType:%2\nModel:%3\naxisCount:%4\nversion:%5")
                                    .arg(mg_curTypeId)
                                    .arg(mg_curTypeName)
                                    .arg(mg_curModeName)
                                    .arg(mg_curAxisCount)
                                    .arg(mg_curVersion));
                    }
                }
            }
        }

        //-----------------------右边框---------------------------------------------
        Rectangle{
            width: 200;
            height: 200;
            Layout.fillHeight: true;
            Layout.fillWidth: true;
            color:Qt.lighter("gray",1.8);
            border.color: "#BBB9B9";
            radius: 5;
            ColumnLayout{
                anchors.fill: parent;
                spacing: 10;
                Image {
                    id: servoIcon
                    fillMode: Image.PreserveAspectFit
                    Layout.maximumHeight: icon1.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: "./Resource/Icon/common/configdialog2.png"
                }
                Rectangle{
                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    color:Qt.lighter("gray",1.8);
                    border.color: "#BBB9B9";
                    ColumnLayout{
                        anchors.fill: parent;
                        spacing: 0;
                        Rectangle{
                            Layout.fillWidth: true;
                            height: 30;
                            color:"steelblue";
                            Text{
                                anchors.centerIn: parent;
                                text:"设备信息";
                                font.letterSpacing:5;
                                color:"white";
                                font.bold: true;
                                font.pixelSize: 16;
                            }
                        }
                        Rectangle{
                            id:m_detailInfoTextRect;
                            Layout.fillHeight: true;
                            Layout.fillWidth: true;
                            color:Qt.lighter("gray",1.8);
                            Text{
                                anchors.horizontalCenter: parent.horizontalCenter;
                                anchors.top: parent.top;
                                anchors.topMargin: 20;
                                text:m_servoInformationText;
                                font.letterSpacing: 10;
                                font.pixelSize: 16;
                            }
                        }
                    }

                }
            }
        }
    }


    //----------------------左边框相关数据--------------------------
    XmlListModel{
        id:servoTypeDataModel;
        source: "./Resource/DeviceData/common/ServoInfo.xml";
        query: "/ServoList/Servo";
        XmlRole{name:"type";query:"ServoTypeId/@typeName/string()"}
        XmlRole{name:"model";query:"@name/string()";}
        XmlRole{name:"typeId";query:"ServoTypeId/number()"}
        XmlRole{name:"axisCount";query:"AxisCount/number()"}
        XmlRole{name:"power";query:"Power/string()"}
        XmlRole{name:"powerIn";query:"PowerIn/string()"}
        XmlRole{name:"DI";query:"DigitalInput/number()"}
        XmlRole{name:"DO";query:"DigitalOutput/number()"}
        XmlRole{name:"AI";query:"AnalogInput/number()"}
        XmlRole{name:"AO";query:"AnalogOutput/number()"}
    }
    Component{
        id:servoConfigListViewDelegate;
        Rectangle {
            id: wrapper;
            width: parent.width;
            height: 40;
            color:mouseArea.containsMouse?"lightsteelblue":wrapper.ListView.isCurrentItem?mg_selectColor:"transparent";
            radius: 5;
            MouseArea{
                id:mouseArea;
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: wrapper.ListView.view.currentIndex=index;
            }
            Text{
                anchors.fill: parent;
                anchors.leftMargin: 10;
                text:model;
                verticalAlignment: Text.AlignVCenter;
                font.pixelSize: 16;
            }
        }
    }

    Component{
        id:servoTypeSectionHeader;
        Rectangle{
            width: parent.width;
            height:30;
            color:"steelblue"
            Text{
                text:section;
                height: parent.height;
                font.bold: true;
                font.pixelSize: 16;
                verticalAlignment: Text.AlignVCenter;
                color:"white"
            }
        }
    }
}

