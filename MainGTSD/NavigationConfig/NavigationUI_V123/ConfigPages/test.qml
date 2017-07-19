import QtQuick 2.5
import QtQuick.XmlListModel 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Rectangle {
    Text{
        text:"hello world";
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
    }
    RowLayout{
        anchors.fill: parent;
        ListView{
            id:m_testListView;
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            spacing: 4;
            focus: true;
            highlight: Rectangle{
                width: parent.width;
                color:"lightgray";
            }
            model:typeDataModel;
            delegate: listViewDelegate;

            section.property: "type";
            section.criteria: ViewSection.FullString;
            section.delegate: sectionHeader;
            onCurrentIndexChanged: {
                var model=typeDataModel.get(currentIndex).model;
                var type=typeDataModel.get(currentIndex).type;
                console.log("index:"+currentIndex+"  model:"+typeDataModel.get(currentIndex).model
                            +"  type:"+typeDataModel.get(currentIndex).type);

                var ss=["./Resource/servo/GTSD_4X/GTSD41/img.png","./Resource/servo/GTSD_4X/GTSD42/img.png"]
                servoIcon.source="./Resource/Icon/common/servo/"+type+"/"+model+"/img.png";
            }
        }
        Item{
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            ColumnLayout{
                anchors.fill: parent
                Item{
                    Layout.fillWidth: true;
                    height: 250;
                    Image {
                        id: servoIcon;
                        anchors.horizontalCenter: parent.horizontalCenter;
                        width: 200;
                        height: parent.height;
                        fillMode: Image.PreserveAspectFit
                        source: "./Resource/Icon/common/servo/GTSD_4X/GTSD41/img.png"
                    }
                }
                Rectangle{
                    id:m_detailInfoTextRect;
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                    color:"lightgray";
                    Text{
                        anchors.horizontalCenter: parent.horizontalCenter;
                        anchors.top: parent.top;
                        anchors.topMargin: 20;
                        text:qsTr("设备类型：%1\n\n型号：%2\n\n控制轴数:%3\n\n输入电源：%4\n\n额定功率:%5\n\nIO信息：\n\nDI:%6 DO:%7\n\nAI:%8 AO:%9\n")
                        .arg(typeDataModel.get(m_testListView.currentIndex).type)
                        .arg(typeDataModel.get(m_testListView.currentIndex).model)
                        .arg(typeDataModel.get(m_testListView.currentIndex).axisCount)
                        .arg(typeDataModel.get(m_testListView.currentIndex).powerIn)
                        .arg(typeDataModel.get(m_testListView.currentIndex).power)
                        .arg(typeDataModel.get(m_testListView.currentIndex).DI)
                        .arg(typeDataModel.get(m_testListView.currentIndex).DO)
                        .arg(typeDataModel.get(m_testListView.currentIndex).AI)
                        .arg(typeDataModel.get(m_testListView.currentIndex).AO)
                        font.letterSpacing: 10;
                        font.pixelSize: 16;
                    }
                }
            }


        }
    }

    XmlListModel{
        id:typeDataModel;
        source: "./NavigationTreeModelFile/ServoInfo.xml";
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
        id:listViewDelegate;
        Item {
            id: wrapper;
            width: parent.width;
            height: 40;
            MouseArea{
                anchors.fill: parent;
                onClicked: wrapper.ListView.view.currentIndex=index;
            }
            Text{
                anchors.fill: parent;
                text:model;
                verticalAlignment: Text.AlignVCenter;
                font.pixelSize: 16;
            }
        }
    }

    Component{
        id:sectionHeader;
        Rectangle{
            width: parent.width;
            height:30;
            color:"lightsteelblue"
            Text{
                text:section;
                height: parent.height;
                font.bold: true;
                font.pixelSize: 16;
                verticalAlignment: Text.AlignVCenter;
            }
        }
    }

//    Column {
//        ProgressBar {
//            value: 0.5
//        }
//        ProgressBar {
//            indeterminate: true
//        }
//    }

}

