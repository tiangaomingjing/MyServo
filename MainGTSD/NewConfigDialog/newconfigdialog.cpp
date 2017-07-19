#include "newconfigdialog.h"
#include "ui_newconfigdialog.h"
#include "globaldef.h"
#include <QMessageBox>
#define RECORD_VERSION 0x80
#define RECORD_COM_ETHERNET 0x01
#define RECORD_COM_RNNET 0x02
#define RECORD_COM_TCPIP 0x04


NewConfigDialog::NewConfigDialog(QWidget *parent) : QWidget(parent),
  ui(new Ui::NewConfigDialog()),
  m_recordClick(0)
{
  ui->setupUi(this);
  ui->widget_ip->hide();
  drawPicture();
}

NewConfigDialog::NewConfigDialog(SysConfig *srcConfig, QWidget *parent) : QWidget(parent),
  ui(new Ui::NewConfigDialog()),
  mp_sysConfig(srcConfig),
  m_recordClick(0)
{
  ui->setupUi(this);
  connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeItemClicked(QTreeWidgetItem*,int)));
  connect(ui->btn_cancle,SIGNAL(clicked(bool)),this,SLOT(onBtnCancleClicked()));
  connect(ui->btn_apply,SIGNAL(clicked(bool)),this,SLOT(onBtnApplyClicked()));
  ui->widget_ip->hide();
  ui->widget_rnnetPort->hide();
  drawPicture();
  ui->btn_apply->setText(tr("Apply"));
  ui->btn_cancle->setText(tr("cancle"));
  m_userConfig.model.version.clear();

  ui->treeWidget->setColumnCount(2);
  ui->treeWidget->hideColumn(1);
  ui->treeWidget->setHeaderLabel(QString(tr("configuration")));
  QTreeWidgetItem *itemRoot;
  QTreeWidgetItem *item;
  QTreeWidgetItem *itemModel;
  QTreeWidgetItem *itemFirm;
  QTreeWidgetItem *itemType;

  if(srcConfig->comList.size()>0)
  {
    itemRoot=new QTreeWidgetItem(ui->treeWidget->invisibleRootItem());
    itemRoot->setText(0,tr("Com"));
    itemRoot->setIcon(0,QIcon(ICON_FILE_PATH+ICON_ETHERNETPORT));
    ui->treeWidget->addTopLevelItem(itemRoot);
    for(int i=0;i<srcConfig->comList.size();i++)
    {
      item=new QTreeWidgetItem(itemRoot);
      item->setText(0,srcConfig->comList.at(i).comName);
      item->setText(1,QString::number(srcConfig->comList.at(i).id));
    }
  }
  if(srcConfig->sysTypeList.size()>0)
  {
    itemRoot=new QTreeWidgetItem(ui->treeWidget->invisibleRootItem());
    itemRoot->setText(0,tr("Type"));
    itemRoot->setIcon(0,QIcon(ICON_FILE_PATH+ICON_CONFIGTREE));
    ui->treeWidget->addTopLevelItem(itemRoot);
    for(int i=0;i<srcConfig->sysTypeList.size();i++)
    {
      itemType=new QTreeWidgetItem(itemRoot);
      itemType->setText(0,srcConfig->sysTypeList.at(i).typeName);
      itemType->setText(1,QString::number(srcConfig->sysTypeList.at(i).typeId));
      qDebug()<<"typeIDDDDDDDDDDDDDDDD"<<srcConfig->sysTypeList.at(i).typeId;
      for(int j=0;j<srcConfig->sysTypeList.at(i).modelList.size();j++)
      {
        itemModel=new QTreeWidgetItem(itemType);
        itemModel->setText(0,srcConfig->sysTypeList.at(i).modelList.at(j).modelName);
        itemModel->setText(1,QString::number(srcConfig->sysTypeList.at(i).modelList.at(j).axisCount));
        for(int k=0;k<srcConfig->sysTypeList.at(i).modelList.at(j).version.size();k++)
        {
          itemFirm=new QTreeWidgetItem(itemModel);
          itemFirm->setText(0,srcConfig->sysTypeList.at(i).modelList.at(j).version.at(k));
          itemFirm->setIcon(0,QIcon(ICON_FILE_PATH+ICON_FIRMWARE));
        }
      }
    }
  }
//  ui->treeWidget->expandToDepth(1);
  ui->treeWidget->expandAll();
  ui->treeWidget->resizeColumnToContents(0);
  setIpInputFormat();
}
NewConfigDialog::~NewConfigDialog()
{

}
//!------------------private slots--------------------------------
void NewConfigDialog::onTreeItemClicked(QTreeWidgetItem *item, int column)
{
  if(!(item->childCount()>0))
  {
    if(item->parent()->text(0)=="Com")
    {
      QString comName=item->text(0);
      int comtype=item->text(1).toInt();
      switch (comtype)
      {
      case 0:
        ui->widget_ip->hide();
        ui->widget_rnnetPort->hide();
        break;
      case 1:
        ui->widget_ip->hide();
        ui->widget_rnnetPort->show();
        break;
      case 2:
        ui->widget_ip->show();
        ui->widget_rnnetPort->hide();
        break;
      default:
        ui->widget_ip->hide();
        ui->widget_rnnetPort->hide();
        break;
      }
      m_recordClick|=(1<<comtype);
      m_recordClick&=((1<<comtype)|0x80);
//      if(comName=="TCP/IP")
//      {
//        ui->widget_ip->show();
//        m_recordClick|=(RECORD_COM_TCPIP&0xff);
//      }
//      else
//      {
//        ui->widget_ip->hide();
//        m_recordClick&=(~(RECORD_COM_TCPIP&0xff));
//      }
      m_userConfig.com.comName=comName;
      m_userConfig.com.id=item->text(1).toInt();
      ui->label_com->setText(tr("Com:%1").arg(comName));

    }
    else
    {
      m_recordClick|=(RECORD_VERSION&0xff);
      m_userConfig.model.version.clear();
      m_userConfig.model.version.append(item->text(0));
      m_userConfig.model.modelName=item->parent()->text(0);
      m_userConfig.model.axisCount=item->parent()->text(1).toInt();
      m_userConfig.typeName=item->parent()->parent()->text(0);
      m_userConfig.typeId=item->parent()->parent()->text(1).toInt();
      ui->label_type->setText(tr("Type:%1\nID:%2\nModel:%3\naxisCount:%4\n%5").arg(m_userConfig.typeName)\
                              .arg(m_userConfig.typeId).arg(m_userConfig.model.modelName)\
                              .arg(m_userConfig.model.axisCount).arg(m_userConfig.model.version.at(0)));
    }
  }
  QString str;
  str.sprintf("%.2x",m_recordClick);
  qDebug()<<"column"<<column<<"record"<<str;
}

void NewConfigDialog::onBtnCancleClicked()
{
  this->hide();
}
void NewConfigDialog::onBtnApplyClicked()
{
  if((m_recordClick&RECORD_VERSION)==0)//没有选择固件版本
  {
    QMessageBox::information(this,tr("information"),tr("please select firmware version"));
    return;
  }
  else if(((m_recordClick&RECORD_COM_ETHERNET)==0)\
          &&((m_recordClick&RECORD_COM_RNNET)==0)\
          &&((m_recordClick&RECORD_COM_TCPIP)==0))//没有选择端口
  {
    QMessageBox::information(this,tr("information"),tr("please select com type"));
    return;
  }
  else
  {
    m_userConfig.com.ipAddress=ui->lineEdit_Addr->text();
    m_userConfig.com.ipPort=ui->lineEdit_Port->text().toInt();
    qDebug()<<ui->lineEdit_Addr->text();
    m_userConfig.com.rnStation=ui->lineEdit_rnNetPort->text().toInt();
    this->hide();
    emit newConfig(&m_userConfig);
  }

}

//!-------------------private  function----------------------------
void NewConfigDialog::drawPicture()
{
  ui->label_source->setPixmap(QPixmap(ICON_FILE_PATH+ICON_PIC_DIALOG_1));
  ui->label_target->setPixmap(QPixmap(ICON_FILE_PATH+ICON_PIC_DIALOG_2));
  ui->label_compic->setPixmap(QPixmap(ICON_FILE_PATH+ICON_ETHERNETPORT));
  ui->label_typepic->setPixmap(QPixmap(ICON_FILE_PATH+ICON_CONFIGTREE));
}
void NewConfigDialog::setIpInputFormat()
{
  //Ip地址输入检测
  QRegExp rx("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
  QRegExpValidator *v=new QRegExpValidator(rx,ui->lineEdit_Addr);
  ui->lineEdit_Addr->setValidator(v);
//  ui->lineEdit_Addr->setInputMask("000.000.000.000;0");//只要加上;0保证有默认值即可使得正则和mask同时生效。

  //端口号输入检测
  QRegExp regExp("^([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
  QRegExpValidator *v2=new QRegExpValidator(regExp,ui->lineEdit_Port);
  ui->lineEdit_Port->setValidator(v2);

  //站号输入检测
  QIntValidator *v3=new QIntValidator(0,255,ui->lineEdit_rnNetPort);
  ui->lineEdit_rnNetPort->setValidator(v3);

  ui->lineEdit_Addr->setText("192.168.0.0");
  ui->lineEdit_Port->setText("8086");
  ui->lineEdit_rnNetPort->setText("240");
}
