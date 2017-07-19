import QtQuick 2.5
import QtClass 1.0
Item
{
    id:root;
    property QLineEdit root_edit: null;
    property int number: 0;
    Timer
    {
        id:countUp;
        interval: 1000;
        repeat: true;
        triggeredOnStart: true;
        onTriggered:
        {
            root.number+=1;
            if(root.number==65536) root.number=0;
//            console.log(root.number);
            factory.updateUiData();
        }
    }
//    QPushButton
//    {
//        id:btnClearAlm;
//        minimumHeight: 32;
//        text:"清报警";
//        onClicked:
//        {
//            console.log("button");
//            mainWindowDevice.clearAlarm();
//        }
//    }

    QmlFactory
    {
        id:factory;
        property QLineEdit edit_state: null;
        property QLineEdit edit_vdc: null;
        property QLineEdit edit_cmdid: null;
        property QLineEdit edit_currid: null;
        property QLineEdit edit_usrmode: null;

        property IconLabel servoIcon: null;
        property IconLabel flagIcon: null;
        property QLabel label_flag: null;

        property Led led_OC: null;
        property Led led_OV: null;
        property Led led_UV: null;
        property Led led_BrkPh: null;
        property Led led_ResErr: null;
        property Led led_OL: null;
        property Led led_OT: null;
        property Led led_IOerr: null;

        property Led led_REG: null;
        property Led led_PS: null;
        property Led led_OS: null;
        property Led led_OPRE: null;
        property Led led_DIR: null;
        property Led led_SOC: null;
        property Led led_OBPH: null;
        property Led led_rsvd: null;

        property QFormLayoutProxy formLayoutProxy1: null;
        property QVBoxLayoutProxy vlayoutProxy1: null;
        property QVBoxLayoutProxy vlayoutProxy2: null;
        property QVBoxLayoutProxy vlayoutProxy3: null;
        property QHBoxLayoutProxy hlayoutProxy0: null;


        property var ledonfileName: null;
        property var ledofffileName:null;
        property var servoIconFileName:null;

        property var stateArray: ["OFF","INIT","ON"];
        property var idArray: ["0 IDLE","1 AOA","2 MIPA","3 MPSA","4 COLD","5 CCLD","6 VCLD","7 PSOCLD"];
        property var modeArray: ["0 IDLE_SEQ","1 AOA_SEQ","2 MIPA_SEQ","3 MPSA_SEQ",
                                 "4 COLD_SEQ","5 CCLD_SEQ","6 VCLD_SEQ","7 OPEN_VCTL_SEQ",
                                 "8 CLOSED_VCTL_SEQ","9 FIX_PCTL_SEQ","10 TRA_PCTL_SEQ"];
        persons:
        [
            Person{id:person1;name:"Lucy";},
            Person{id:person2;name:"Jack";},
            Person{id:person3;name:"Andriod";}
        ]
        property QSpacerItemProxy spaceItem1: null;
        property QSpacerItemProxy spaceItem2: null;
        property QSpacerItemProxy spaceItem3: null;
        property QSpacerItemProxy spaceItem4: null;

        property QSpacerItemProxy spaceItemV1: null;
        property QSpacerItemProxy spaceItemV2: null;
        property QSpacerItemProxy spaceItemV3: null;

        property QTreeWidgetProxy dataTree: null;

        onInitialFactory:
        {
            ledonfileName=iconPath+"led_on.png";
            ledofffileName=iconPath+"led_off.png";
            servoIconFileName=iconPath+"servo_device.png";

//            stateArray=new Array("OFF","INIT","ON");
//            idArray=new Array("0 IDLE","1 AOA","2 MIPA","3 MPSA","4 COLD","5 CCLD","6 VCLD","7 PSOCLD");
//            modeArray=new Array("0 IDLE_SEQ","1 AOA_SEQ","2 MIPA_SEQ","3 MPSA_SEQ",
//                                "4 COLD_SEQ","5 CCLD_SEQ","6 VCLD_SEQ","7 OPEN_VCTL_SEQ",
//                                "8 CLOSED_VCTL_SEQ","9 FIX_PCTL_SEQ","10 TRA_PCTL_SEQ");

            spaceItem1=factory.createQSpacerItemProxy();
            spaceItem2=factory.createQSpacerItemProxy();
            spaceItem3=factory.createQSpacerItemProxy();
            spaceItem4=factory.createQSpacerItemProxy();

            spaceItemV1=factory.createQSpacerItemProxy(1);
            spaceItemV2=factory.createQSpacerItemProxy(1);
            spaceItemV3=factory.createQSpacerItemProxy(1);

            hlayoutProxy0=factory.createQHBoxLayoutProxy(mainPage);
            hlayoutProxy0.addSpacerItem(spaceItem1);
            //第一列
            flagIcon=factory.createIconLabel(mainPage);
            flagIcon.alignment=4;
            flagIcon.loadIconFromFile(ledonfileName);
            label_flag=factory.createQLabel(mainPage);
            label_flag.text="Axis_"+mainWindowDevice.getAxisNum()+" OK...";
            label_flag.alignment=4;
            servoIcon=factory.createIconLabel(mainPage);
            servoIcon.alignment=4;
            servoIcon.loadIconFromFile(servoIconFileName);

            edit_state=factory.createQLineEdit(mainPage);
            edit_vdc=factory.createQLineEdit(mainPage);
            edit_cmdid=factory.createQLineEdit(mainPage);
            edit_currid=factory.createQLineEdit(mainPage);
            edit_usrmode=factory.createQLineEdit(mainPage);
            edit_state.enabled=false;
            edit_vdc.enabled=false;
            edit_cmdid.enabled=false;
            edit_currid.enabled=false;
            edit_usrmode.enabled=false;

            formLayoutProxy1=factory.createQFormLayoutProxy();
            formLayoutProxy1.addRow("state",edit_state);
            formLayoutProxy1.addRow("vdc",edit_vdc);
            formLayoutProxy1.addRow("cmd_id",edit_cmdid);
            formLayoutProxy1.addRow("curr_id",edit_currid);
            formLayoutProxy1.addRow("usr_mode",edit_usrmode);

            vlayoutProxy1=factory.createQVBoxLayoutProxy();
            vlayoutProxy1.addSpacerItem(spaceItemV1);
            vlayoutProxy1.addWidget(flagIcon);
            vlayoutProxy1.addWidget(label_flag);
            vlayoutProxy1.addWidget(servoIcon);
//            vlayoutProxy1.addWidget(btnClearAlm);
            vlayoutProxy1.addSpacerItem(spaceItemV2);
            vlayoutProxy1.addLayout(formLayoutProxy1.fromlayout);
            vlayoutProxy1.addSpacerItem(spaceItemV3);
            hlayoutProxy0.addLayout(vlayoutProxy1.vboxLayout);
            hlayoutProxy0.addSpacerItem(spaceItem2);

            //第二列
            vlayoutProxy2=factory.createQVBoxLayoutProxy();
            led_OC=factory.createLed(mainPage);
            led_OC.ledOnFileName=ledonfileName;
            led_OC.ledOffFileName=ledofffileName;
            led_OC.ledName="过流";
            led_OC.state=false;
            led_OV=factory.createLed(mainPage);
            led_OV.ledOnFileName=ledonfileName;
            led_OV.ledOffFileName=ledofffileName;
            led_OV.ledName="过压";
            led_OV.state=false;
            led_UV=factory.createLed(mainPage);
            led_UV.ledOnFileName=ledonfileName;
            led_UV.ledOffFileName=ledofffileName;
            led_UV.ledName="欠压";
            led_UV.state=false;
            led_BrkPh=factory.createLed(mainPage);
            led_BrkPh.ledOnFileName=ledonfileName;
            led_BrkPh.ledOffFileName=ledofffileName;
            led_BrkPh.ledName="输入断线";
            led_BrkPh.state=false;
            led_ResErr=factory.createLed(mainPage);
            led_ResErr.ledOnFileName=ledonfileName;
            led_ResErr.ledOffFileName=ledofffileName;
            led_ResErr.ledName="编码器故障";
            led_ResErr.state=false;
            led_OL=factory.createLed(mainPage);
            led_OL.ledOnFileName=ledonfileName;
            led_OL.ledOffFileName=ledofffileName;
            led_OL.ledName="过载";
            led_OL.state=false;
            led_OT=factory.createLed(mainPage);
            led_OT.ledOnFileName=ledonfileName;
            led_OT.ledOffFileName=ledofffileName;
            led_OT.ledName="过温";
            led_OT.state=false;
            led_IOerr=factory.createLed(mainPage);
            led_IOerr.ledOnFileName=ledonfileName;
            led_IOerr.ledOffFileName=ledofffileName;
            led_IOerr.ledName="IO故障";
            led_IOerr.state=false;

            vlayoutProxy2.addWidget(led_OC);
            vlayoutProxy2.addWidget(led_OV);
            vlayoutProxy2.addWidget(led_UV);
            vlayoutProxy2.addWidget(led_BrkPh);
            vlayoutProxy2.addWidget(led_ResErr);
            vlayoutProxy2.addWidget(led_OL);
            vlayoutProxy2.addWidget(led_OT);
            vlayoutProxy2.addWidget(led_IOerr);
            hlayoutProxy0.addLayout(vlayoutProxy2.vboxLayout);
            hlayoutProxy0.addSpacerItem(spaceItem3);

            //第三列
            vlayoutProxy3=factory.createQVBoxLayoutProxy();
            led_REG=factory.createLed(mainPage);
            led_REG.ledOnFileName=ledonfileName;
            led_REG.ledOffFileName=ledofffileName;
            led_REG.ledName="寄存器故障";
            led_REG.state=false;
            led_PS=factory.createLed(mainPage);
            led_PS.ledOnFileName=ledonfileName;
            led_PS.ledOffFileName=ledofffileName;
            led_PS.ledName="功率模块故障";
            led_PS.state=false;
            led_OS=factory.createLed(mainPage);
            led_OS.ledOnFileName=ledonfileName;
            led_OS.ledOffFileName=ledofffileName;
            led_OS.ledName="过速";
            led_OS.state=false;
            led_OPRE=factory.createLed(mainPage);
            led_OPRE.ledOnFileName=ledonfileName;
            led_OPRE.ledOffFileName=ledofffileName;
            led_OPRE.ledName="过压力";
            led_OPRE.state=false;
            led_DIR=factory.createLed(mainPage);
            led_DIR.ledOnFileName=ledonfileName;
            led_DIR.ledOffFileName=ledofffileName;
            led_DIR.ledName="方向错误";
            led_DIR.state=false;
            led_SOC=factory.createLed(mainPage);
            led_SOC.ledOnFileName=ledonfileName;
            led_SOC.ledOffFileName=ledofffileName;
            led_SOC.ledName="瞬时过流";
            led_SOC.state=false;
            led_OBPH=factory.createLed(mainPage);
            led_OBPH.ledOnFileName=ledonfileName;
            led_OBPH.ledOffFileName=ledofffileName;
            led_OBPH.ledName="输出断线";
            led_OBPH.state=false;
            led_rsvd=factory.createLed(mainPage);
            led_rsvd.ledOnFileName=ledonfileName;
            led_rsvd.ledOffFileName=ledofffileName;
            led_rsvd.ledName="保留位";
            led_rsvd.state=true;

            vlayoutProxy3.addWidget(led_REG);
            vlayoutProxy3.addWidget(led_PS);
            vlayoutProxy3.addWidget(led_OS);
            vlayoutProxy3.addWidget(led_OPRE);
            vlayoutProxy3.addWidget(led_DIR);
            vlayoutProxy3.addWidget(led_SOC);
            vlayoutProxy3.addWidget(led_OBPH);
            vlayoutProxy3.addWidget(led_rsvd);
            hlayoutProxy0.addLayout(vlayoutProxy3.vboxLayout);
            hlayoutProxy0.addSpacerItem(spaceItem4);

            dataTree=factory.createQTreeWidgetProxy(treeSource,mainWindowDevice);
            countUp.interval=600+10*mainWindowDevice.getAxisNum();
            countUp.start();

        }
        function message(msg)
        {
            console.log(msg+" person length:"+persons.length+",p1 name:"+persons[0].name);
        }
        function updateUiData()
        {
            var index=parseInt(factory.dataTree.textTopLevel(0,1))-1;
            var strText;
            if(index<=3)
            {
                strText=stateArray[index];
                factory.edit_state.text=String(strText);
            }

            index=parseInt(factory.dataTree.textTopLevel(4,1));
            if(index<=7)
            {
                strText=idArray[index];
                factory.edit_cmdid.text=String(strText);
            }

            index=parseInt(factory.dataTree.textTopLevel(5,1));
            if(index<=10)
            {
                strText=idArray[index];
                factory.edit_currid.text=String(strText);
            }

            index=parseInt(factory.dataTree.textTopLevel(6,1));
            strText=modeArray[index];
            factory.edit_usrmode.text=String(strText);

//            factory.edit_state.text=factory.dataTree.textTopLevel(0,1);
            factory.edit_vdc.text=factory.dataTree.textTopLevel(1,1);
//            factory.edit_cmdid.text=factory.dataTree.textTopLevel(4,1);
//            factory.edit_currid.text=factory.dataTree.textTopLevel(5,1);
//            factory.edit_usrmode.text=factory.dataTree.textTopLevel(6,1);

            led_OC.state=!Boolean(parseInt(factory.dataTree.textChild(3,0,1)));
            led_OV.state=!Boolean(parseInt(factory.dataTree.textChild(3,1,1)));
            led_UV.state=!Boolean(parseInt(factory.dataTree.textChild(3,2,1)));
            led_BrkPh.state=!Boolean(parseInt(factory.dataTree.textChild(3,3,1)));
            led_ResErr.state=!Boolean(parseInt(factory.dataTree.textChild(3,4,1)));
            led_OL.state=!Boolean(parseInt(factory.dataTree.textChild(3,5,1)));
            led_OT.state=!Boolean(parseInt(factory.dataTree.textChild(3,6,1)));
            led_IOerr.state=!Boolean(parseInt(factory.dataTree.textChild(3,7,1)));
            led_REG.state=!Boolean(parseInt(factory.dataTree.textChild(3,8,1)));
            led_PS.state=!Boolean(parseInt(factory.dataTree.textChild(3,9,1)));
            led_OS.state=!Boolean(parseInt(factory.dataTree.textChild(3,10,1)));
            led_OPRE.state=!Boolean(parseInt(factory.dataTree.textChild(3,11,1)));
            led_DIR.state=!Boolean(parseInt(factory.dataTree.textChild(3,12,1)));
            led_SOC.state=!Boolean(parseInt(factory.dataTree.textChild(3,13,1)));
            led_OBPH.state=!Boolean(parseInt(factory.dataTree.textChild(3,14,1)));
//            for(var i=0;i<15;i++) console.log("child text:"+factory.dataTree.textChild(3,i,1));
            var flagState=Boolean(parseInt(factory.dataTree.textTopLevel(2,1)));
            if(flagState)
            {
                flagIcon.loadIconFromFile(factory.ledofffileName);
                label_flag.text="Axis_"+mainWindowDevice.getAxisNum()+" ERROR !";
            }
            else
            {
                flagIcon.loadIconFromFile(factory.ledonfileName);
                label_flag.text="Axis_"+mainWindowDevice.getAxisNum()+" OK...";
            }

        }

    }
    Person
    {
        id:root_person;
        name:"Joy";
    }
}

