import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtClass 1.0

import "./components/CfgMotor"

Rectangle{
    id:root;
    color: "#F0F0F0";
    QmlFactory{
        id:factory;
        property QTreeWidgetProxy dataTree: null;
        onInitialFactory:{
            dataTree=factory.createQTreeWidgetProxy(treeSource,driveMotor);
        }
    }

    ColumnLayout{
        anchors.fill: parent;
        anchors.margins: 10;
        spacing: 15;
        RowLayout{
            spacing: 10;
            Current{
                id:cur;
                dataTree: factory.dataTree;
                Layout.fillHeight:true;
                Layout.fillWidth: true;
                Layout.minimumWidth: 300;
            }
            Image {
                id: motor;
                width: 200;
                height: 100;
                Layout.maximumWidth: 250;
                Layout.maximumHeight: 140;
                source: "file:///"+iconPath+"motor_qml.png";
            }
        }
        Velocity{
            id:vel;
            dataTree: factory.dataTree;
            Layout.fillHeight:true;
            Layout.fillWidth: true;
            Layout.minimumHeight: 80;
        }
        Impedance{
            id:imp;
            dataTree: factory.dataTree;
            Layout.fillHeight:true;
            Layout.fillWidth: true;
            Layout.minimumHeight: 80;
        }
        MechanicalParameter{
            id:mecprm;
            dataTree: factory.dataTree;
            Layout.fillHeight:true;
            Layout.fillWidth: true;
            Layout.minimumHeight: 80;
        }
        ForceParameter{
            id:forprm;
            dataTree: factory.dataTree;
            Layout.fillHeight:true;
            Layout.fillWidth: true;
            Layout.minimumHeight: 80;
        }
    }

    Connections{
        target: driveMotor;
        onItemValueChanged:{
            cur.irat_1.text=factory.dataTree.textTopLevel(0,1);
            cur.imax_1.text=factory.dataTree.textTopLevel(1,1);
            vel.sct_1.text=factory.dataTree.textTopLevel(2,1);
            vel.srat_1.text=factory.dataTree.textTopLevel(3,1);
            vel.nos_1.text=factory.dataTree.textTopLevel(4,1);
            forprm.tqr_1.text=factory.dataTree.textTopLevel(5,1);
            mecprm.ppn_1.text=factory.dataTree.textTopLevel(6,1);
            forprm.vmax_1.text=factory.dataTree.textTopLevel(7,1);
            forprm.phim_1.text=factory.dataTree.textTopLevel(8,1);
            imp.ldm_1.text=factory.dataTree.textTopLevel(9,1);
            imp.lqm_1.text=factory.dataTree.textTopLevel(10,1);
            imp.rm_1.text=factory.dataTree.textTopLevel(11,1);
            mecprm.jm_1.text=factory.dataTree.textTopLevel(12,1);
            mecprm.jrat_1.text=factory.dataTree.textTopLevel(13,1);
            mecprm.fm_1.text=factory.dataTree.textTopLevel(14,1);

            cur.irat_1.textColor="black";
            cur.imax_1.textColor="black";
            vel.sct_1.textColor="black";
            vel.srat_1.textColor="black";
            vel.nos_1.textColor="black";
            forprm.tqr_1.textColor="black";
            mecprm.ppn_1.textColor="black";
            forprm.vmax_1.textColor="black";
            forprm.phim_1.textColor="black";
            imp.ldm_1.textColor="black";
            imp.lqm_1.textColor="black";
            imp.rm_1.textColor="black";
            mecprm.jm_1.textColor="black";
            mecprm.jrat_1.textColor="black";
            mecprm.fm_1.textColor="black";

        }
    }
}

