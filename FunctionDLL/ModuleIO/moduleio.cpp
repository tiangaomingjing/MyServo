#include "moduleio.h"
#include "ui_moduleio.h"
#include "iowidget.h"
#include "MainGTSD/mainwindow.h"

#include <QTreeWidget>
#include <QStackedWidget>
#include <QDebug>
#include <QMessageBox>

#define TREE_DIGITAL_INDEX 0
#define TREE_ANALOG_INDEX 1
#define TREE_DIGITAL_OUT_INDEX 0
#define TREE_DIGITAL_IN_INDEX 1
#define TREE_DIGITAL_OUT_POLARITY_INDEX 1
#define TREE_DIGITAL_OUT_ENABLE_INDEX 0
#define IO_DESCRIPTION_FILENAME "PrmFuncIO.xml"

enum Tree_IO_Column_Index{
  TREE_IO_NAME_COL=0,
  TREE_IO_VALUE_COL,
  TREE_IO_MEMTYPE_COL,
  TREE_IO_ADDR_COL,
  TREE_IO_BASEADDR_COL,
  TREE_IO_BITSTART_COL,
  TREE_IO_BITWIDTH_COL,
  TREE_IO_CONTROLTYPE_COL,
  TREE_IO_IOTYPE_COL,
  TREE_IO_IOTYPENAME_COL,
  TREE_IO_PHYPIN_COL,
  TREE_IO_NOTE_COL,
  TREE_IO_TOTAL_COL
};

ModuleIO::ModuleIO(QWidget *parent) :
  AbstractFuncWidget(parent),
  ui(new Ui::ModuleIO)
{
  ui->setupUi(this);
  setUiWidgetStyle();
}

ModuleIO::~ModuleIO()
{
  delete ui;
  for(int i=0;i<m_ioWidgetList.count();i++)
    delete m_ioWidgetList[i];
  m_ioWidgetList.clear();
  qDebug()<<"moduleio release ->";
}
void ModuleIO::onWriteFuncTreetoServoRam()
{

}
void ModuleIO::onWriteFuncTreetoServoFlash()
{
  QString iodesFileName=m_qmlFilePath+IO_DESCRIPTION_FILENAME;
  for(int i=0;i<m_ioWidgetList.count();i++)
  {
    m_ioWidgetList.at(i)->setLineEditUiStatus(IOWidget::LINEEDIT_STA_NORMAL);
    m_ioWidgetList.at(i)->clearLineEnterFlag();
  }
  writeIODescription(iodesFileName);
}

void ModuleIO::onActionReadFuncValueFromRam()
{
  if(mp_mainWindow->getComOpenState())
  {
    qDebug()<<"update iowidget ";
    bool retEnable=false;
    IOWidget *io;
    QTreeWidgetItem *itemDout;
    QTreeWidgetItem *itemIn;
    QTreeWidgetItem *item;
    itemDout=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_OUT_INDEX);
    itemIn=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_IN_INDEX);
    quint32 value;

    for(int i=0;i<m_ioWidgetList.count();i++)
    {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
      if(mp_mainWindow->getComOpenState()==false)
        break;
      io=m_ioWidgetList[i];
      if(io->ioType()==IOWidget::IO_OUTPUT)
      {
        item=itemDout->child(io->index());
        retEnable=getBit(item->child(TREE_DIGITAL_OUT_ENABLE_INDEX),value);
        io->setUiEnabledStatus(retEnable);
        qDebug()<<"update out io:"<<io->index();
      }
      else if(io->ioType()==IOWidget::IO_INPUT)
      {
        item=itemIn->child(io->index());
        //输入目前没有极性选项
        retEnable=getBit(item->child(TREE_DIGITAL_OUT_ENABLE_INDEX),value);
        io->setUiEnabledStatus(retEnable);
        qDebug()<<"update in io:"<<io->index();
      }
    }
  }
}
void ModuleIO::updateUiWhenNavigationTreeClicked()
{
  onActionReadFuncValueFromRam();
}
void ModuleIO::onItemExpanded(QTreeWidgetItem *item)
{
  Q_UNUSED(item)
}
void ModuleIO::onTreeItemClickedEdit(QTreeWidgetItem *item, int column)
{
  Q_UNUSED(item)
  Q_UNUSED(column)
}
void ModuleIO::onItemSelecttionChangedHighlight()
{

}

QTreeWidget *ModuleIO::getUiTree()
{
  return ui->treeWidget;
}
QStackedWidget *ModuleIO::getUiStackedWidget()
{
  return ui->stackedWidget;
}

void ModuleIO::setActionReadFuncValueFromFlashEnable()
{
  m_actReadFuncValueFromFlash->setEnabled(false);
}
void ModuleIO::onEnterKeyPress()
{

}
void ModuleIO::onIOEnableClicked(bool checked)
{
  if(mp_mainWindow->getComOpenState()!=true)
    return;
  IOWidget *io=qobject_cast<IOWidget *>(sender());
  qDebug()<<"index: "<<io->index();
  qDebug()<<"enable: "<<checked;

  bool ret=false;
  QString value;

  if(io->ioType()==IOWidget::IO_OUTPUT)
  {
    QTreeWidgetItem *itemDout;
    QTreeWidgetItem *item;
    itemDout=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_OUT_INDEX);
    item=itemDout->child(io->index());
//    getBit(item->child(TREE_DIGITAL_OUT_ENABLE_INDEX),value);
    ret=setBit(item->child(TREE_DIGITAL_OUT_ENABLE_INDEX),checked);
    if(ret)
      value="1";
    else
      value="0";
    item->child(TREE_DIGITAL_OUT_ENABLE_INDEX)->setText(TREE_IO_VALUE_COL,value);
    io->setUiEnabledStatus(ret);
  }
}

void ModuleIO::onIOComboBoxActived(int index)
{
//  IOWidget *io=qobject_cast<IOWidget *>(sender());
//  qDebug()<<"index: "<<io->index();
//  qDebug()<<"combobox index: "<<index;
//  if(io->ioType()==IOWidget::IO_OUTPUT)
//  {

//  }
  qDebug()<<"----------------combobox index: "<<index<<" --------------------";
}

void ModuleIO::onIOPolarityChanged(bool checked)
{
  if(mp_mainWindow->getComOpenState()!=true)
    return;
  IOWidget *io=qobject_cast<IOWidget *>(sender());
  qDebug()<<"index: "<<io->index();
  qDebug()<<"polarity checked: "<<checked;
  bool ret=false;
  QString value;
  if(io->ioType()==IOWidget::IO_OUTPUT)
  {
    QTreeWidgetItem *itemDout;
    QTreeWidgetItem *item;
    itemDout=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_OUT_INDEX);
    item=itemDout->child(io->index());
    ret=setBit(item->child(TREE_DIGITAL_OUT_POLARITY_INDEX),checked);
    if(ret)
      value="1";
    else
      value="0";
    item->child(TREE_DIGITAL_OUT_POLARITY_INDEX)->setText(TREE_IO_VALUE_COL,value);
    io->setUiPolarityStatus(ret);//更新界面极性
  }
}

void ModuleIO::onIONoteSelectionChanged()
{
  IOWidget *io=qobject_cast<IOWidget *>(sender());
  QString description;
  qDebug()<<"index: "<<io->index();
  QTreeWidgetItem *doTreeItem;
  QTreeWidgetItem *diTreeItem;

  doTreeItem=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_OUT_INDEX);
  diTreeItem=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_IN_INDEX);

  if(io->ioType()==IOWidget::IO_OUTPUT)
    description=doTreeItem->child(io->index())->text(TREE_IO_NOTE_COL);
  else
    description=diTreeItem->child(io->index())->text(TREE_IO_NOTE_COL);
  io->setNoteDescription(description);
}

void ModuleIO::onIONoteEnterPressed()
{
  IOWidget *io=qobject_cast<IOWidget *>(sender());
  qDebug()<<"index: "<<io->index();
  QTreeWidgetItem *item;
  QTreeWidgetItem *itemChild;
  if(io->ioType()==IOWidget::IO_OUTPUT)
    item=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_OUT_INDEX);
  else//输入IO
    item=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_IN_INDEX);
  itemChild=item->child(io->index());
  itemChild->setText(TREE_IO_NOTE_COL,io->noteDescription());
}

void ModuleIO::onMainWindowTimeOut(int time)
{
  if(time%2==0)
    onActionReadFuncValueFromRam();
}

void ModuleIO::setActiveNow(bool actived)
{
  if(actived)
    connect(mp_mainWindow,SIGNAL(timeOut(int)),this,SLOT(onMainWindowTimeOut(int)));
  else
    disconnect(mp_mainWindow,SIGNAL(timeOut(int)),this,SLOT(onMainWindowTimeOut(int)));
}

void ModuleIO::setUiWidgetStyle()
{
  QString str="QTabWidget::pane { /* The tab widget frame */\
      border: 2px solid #DBDBDB;\
      background:#F0F0F0;\
      border-top-right-radius: 10px;\
      border-bottom-right-radius: 10px;\
      border-bottom-left-radius: 10px;\
  }\
  QTabWidget::tab-bar {\
      left: 0px; /* move to the right by 5px */\
  }\
  QTabBar::tab {\
      background:#F0F0F0;\
      border-top: 2px solid #DBDBDB;\
      border-left: 2px solid #DBDBDB;\
      border-right: 2px solid #DBDBDB;\
      border-top-left-radius: 4px;\
      border-top-right-radius: 4px;\
      min-width: 16ex;\
      min-height: 10ex;\
      padding: 2px;\
  }\
  QTabBar::tab:selected{\
      background: #F0F0F0;\
  }\
  QTabBar::tab:selected {\
      border-color: #DDDDE0;\
      border-bottom-color: #DDDDE0; /* same as pane color */\
  }\
  QTabBar::tab:!selected {\
      margin-top: 6px; /* make non-selected tabs look smaller */\
      border-color: #DDDDE0;\
      background:#DBDBDB;\
  }\
  QTabBar::tab:hover {\
                background: #F0F0F0;\
            }";
  ui->tabWidget->setStyleSheet(str);
//  ui->scrollArea->setStyleSheet(QStringLiteral("background-color: transparent"));
//  ui->scrollArea_2->setStyleSheet(QStringLiteral("background-color: transparent"));
}
void ModuleIO::connectionSignalSlotHandler()
{
  connect(mp_mainWindow,SIGNAL(timeOut(int)),this,SLOT(onMainWindowTimeOut(int)));
}

void ModuleIO::createUiByQml()
{
  // ui dout
  qDebug()<<this->objectName()<<" createUiByQml";
  QString version=mp_mainWindow->getUserConfig()->model.version.at(0);
  QString modelName=mp_mainWindow->getUserConfig()->model.modelName;
  QString typeName=mp_mainWindow->getUserConfig()->typeName;
  m_qmlFilePath=SYSCONFIG_FILE_PATH+typeName+"/"+modelName+"/"+version+"/";
//  QString iodesFileName=m_qmlFilePath+IO_DESCRIPTION_FILENAME;
//  qDebug()<<"io description file name:"<<iodesFileName;
//  readIODescription(iodesFileName);

  QTreeWidgetItem *doTreeItem;
  QTreeWidgetItem *diTreeItem;
  QTreeWidgetItem *childItem;
  IOWidget *ioWidget=NULL;
  doTreeItem=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_OUT_INDEX);
  diTreeItem=ui->treeWidget->topLevelItem(TREE_DIGITAL_INDEX)->child(TREE_DIGITAL_IN_INDEX);
  QStringList listDoutComboBox;
  listDoutComboBox<<"0 motor brake"\
                  <<"1 general io";
  int iotypeId;
  for(int i=0;i<doTreeItem->childCount();i++)
  {
    childItem=doTreeItem->child(i);
    ioWidget=new IOWidget(IOWidget::IO_OUTPUT,i,0);
    ioWidget->setComboBoxContents(listDoutComboBox);
    iotypeId=childItem->text(TREE_IO_IOTYPE_COL).toInt();
    ioWidget->setComboBoxCurrentIndex(iotypeId);
    if(iotypeId==1)//通用IO目前还没有极性控制
      ioWidget->setPolarityEnable(false);

    ioWidget->setPinDescription(childItem->text(TREE_IO_PHYPIN_COL));
    ioWidget->setNoteDescription(childItem->text(TREE_IO_NOTE_COL));
//    ioWidget->setEnabled(false);
    ui->vlayout_scloutput->addWidget(ioWidget);
    m_ioWidgetList.append(ioWidget);
    qDebug()<<"creat iowidget :"<<i;
    connect(ioWidget,SIGNAL(ioComboBoxActived(int)),this,SLOT(onIOComboBoxActived(int)));
    connect(ioWidget,SIGNAL(ioEnableChanged(bool)),this,SLOT(onIOEnableClicked(bool)));
    connect(ioWidget,SIGNAL(ioPolarityChanged(bool)),this,SLOT(onIOPolarityChanged(bool)));
    connect(ioWidget,SIGNAL(ioNoteDescriptionSelectChanged()),this,SLOT(onIONoteSelectionChanged()));
    connect(ioWidget,SIGNAL(ioNoteEnterPressed()),this,SLOT(onIONoteEnterPressed()));
  }
  for(int i=0;i<diTreeItem->childCount();i++)
  {
    childItem=diTreeItem->child(i);
    ioWidget=new IOWidget(IOWidget::IO_INPUT,i,0);
    ioWidget->setComboBoxContents(listDoutComboBox);
    iotypeId=childItem->text(TREE_IO_IOTYPE_COL).toInt();
    ioWidget->setComboBoxCurrentIndex(iotypeId);
    if(iotypeId==1)//通用IO目前还没有极性控制
      ioWidget->setPolarityEnable(false);

    ioWidget->setPinDescription(childItem->text(TREE_IO_PHYPIN_COL));
    ioWidget->setNoteDescription(childItem->text(TREE_IO_NOTE_COL));
//    ioWidget->setEnabled(false);
    ui->vlayout_sclinput->addWidget(ioWidget);
    m_ioWidgetList.append(ioWidget);
    qDebug()<<"creat iowidget :"<<i;
    connect(ioWidget,SIGNAL(ioComboBoxActived(int)),this,SLOT(onIOComboBoxActived(int)));
    connect(ioWidget,SIGNAL(ioEnableChanged(bool)),this,SLOT(onIOEnableClicked(bool)));
    connect(ioWidget,SIGNAL(ioPolarityChanged(bool)),this,SLOT(onIOPolarityChanged(bool)));
    connect(ioWidget,SIGNAL(ioNoteDescriptionSelectChanged()),this,SLOT(onIONoteSelectionChanged()));
    connect(ioWidget,SIGNAL(ioNoteEnterPressed()),this,SLOT(onIONoteEnterPressed()));
  }
}

bool ModuleIO::writeIODescription(const QString &fileName)
{
  //json 序列化
  /*QVariantList doDesList;
  for(int i=0;i<m_ioDescriptionList.doDescription.count();i++)
  {
    doDesList<<m_ioDescriptionList.doDescription.at(i);
  }

  QVariantList diDesList;

  QVariantMap digital;
  digital.insert("di",diDesList);
  digital.insert("do",doDesList);

  QVariantMap analog;

  QVariantMap root;
  root.insert("digital",digital);
  root.insert("analog",analog);

  QJsonDocument jsonDocument = QJsonDocument::fromVariant(root);
  QString json=jsonDocument.toJson();
  if (!jsonDocument.isNull())
  {
    qDebug() << json;
    QFile outFile;
    outFile.setFileName(fileName);
    if(!(outFile.open(QFile::WriteOnly|QIODevice::Truncate))){
      QMessageBox::information(0,tr("open"),tr("error open file:%1").arg(fileName));
      return false;
    }
    QTextStream out(&outFile);
    out<<json;
    outFile.close();
    return true;
  }
  else
    return false;*/

  QFile fileXML(fileName);
  if (!fileXML.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QMessageBox::information(0,tr("XML File"),tr("XML file save error:%!").arg(fileName));
    return false;
  }

  QXmlStreamWriter writer(&fileXML);
  writer.setAutoFormatting(true);
  writer.writeStartDocument("1.0");
  writer.writeStartElement("ui");                 //1
  writer.writeAttribute("version","4.0");

  writer.writeTextElement("class", "Form");
  writer.writeStartElement("widget");             //2<widget
    writer.writeAttribute("class","QWidget");
    writer.writeAttribute("name","Form");

    writer.writeStartElement("property");          //3
      writer.writeAttribute("name","geometry");
      writer.writeStartElement("rect");
      writer.writeTextElement("x", "0");
      writer.writeTextElement("y", "0");
      writer.writeTextElement("width", "400");
      writer.writeTextElement("height", "400");
      writer.writeEndElement();
    writer.writeEndElement();                     //-3

    writer.writeStartElement("property");
    writer.writeAttribute("name","windowTitle");
      writer.writeTextElement("string", "Form");
    writer.writeEndElement();

    writer.writeStartElement("layout");             //<layout
    writer.writeAttribute("class","QVBoxLayout");
    writer.writeAttribute("name","verticalLayout");
      writer.writeStartElement("item");             //<item>

      writer.writeStartElement("widget");           //<widget
      writer.writeAttribute("class","QTreeWidget");
      writer.writeAttribute("name","treeWidget");

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "memberName");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "value");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "memberType");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "address");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "baseAdd");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "bitStart");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "bitWidth");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "controlType");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "ioType");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "ioTypeName");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "physicalPin");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "note");
      writer.writeEndElement();
      writer.writeEndElement();

      //开始写树结构
      QTreeWidgetItem *item;
      int itemCount=ui->treeWidget->topLevelItemCount();
      for(int i=0;i<itemCount;i++)
      {
        item=ui->treeWidget->topLevelItem(i);
        qDebug()<<"topitem:"<<item->text(0);
        QString name = item->text(TREE_IO_NAME_COL);
        QString value = item->text(TREE_IO_VALUE_COL);
        QString type = item->text(TREE_IO_MEMTYPE_COL);
        QString address = item->text(TREE_IO_ADDR_COL);
        QString base = item->text(TREE_IO_BASEADDR_COL);
        QString bitstart = item->text(TREE_IO_BITSTART_COL);
        QString bitwidth = item->text(TREE_IO_BITWIDTH_COL);
        QString controlType = item->text(TREE_IO_CONTROLTYPE_COL);
        QString ioType = item->text(TREE_IO_IOTYPE_COL);
        QString ioTypeName = item->text(TREE_IO_IOTYPENAME_COL);
        QString phyPin = item->text(TREE_IO_PHYPIN_COL);
        QString note = item->text(TREE_IO_NOTE_COL);
        writer.writeStartElement("item");

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",name);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",value);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",type);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",address);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",base);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",bitstart);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",bitwidth);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",controlType);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",ioType);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",ioTypeName);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",phyPin);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",note);
        writer.writeEndElement();
        writeStructXmlFile(item,writer);

        writer.writeEndElement();//end item
      }
      writer.writeEndElement();                     //</widget>
      writer.writeEndElement();                     //</item>
    writer.writeEndElement();                       //</layout>
  writer.writeEndElement();                       //</widget>
  writer.writeStartElement("resources");
  writer.writeEndElement();
  writer.writeStartElement("connections");
  writer.writeEndElement();
  writer.writeEndElement();                       //-1
  writer.writeEndDocument();
  fileXML.close();
  return true;
}
void ModuleIO::writeStructXmlFile(QTreeWidgetItem *item, QXmlStreamWriter &writer)
{
  QTreeWidgetItem *itemChild;

  for (int i = 0; i<item->childCount();i++)
  {
    itemChild = item->child(i);
    qDebug()<<"write: "<<itemChild->text(0);
    QString name = itemChild->text(TREE_IO_NAME_COL);
    QString value = itemChild->text(TREE_IO_VALUE_COL);
    QString type = itemChild->text(TREE_IO_MEMTYPE_COL);
    QString address = itemChild->text(TREE_IO_ADDR_COL);
    QString base = itemChild->text(TREE_IO_BASEADDR_COL);
    QString bitstart = itemChild->text(TREE_IO_BITSTART_COL);
    QString bitwidth = itemChild->text(TREE_IO_BITWIDTH_COL);
    QString controlType = itemChild->text(TREE_IO_CONTROLTYPE_COL);
    QString ioType = itemChild->text(TREE_IO_IOTYPE_COL);
    QString ioTypeName = itemChild->text(TREE_IO_IOTYPENAME_COL);
    QString phyPin = itemChild->text(TREE_IO_PHYPIN_COL);
    QString note = itemChild->text(TREE_IO_NOTE_COL);
    writer.writeStartElement("item");

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",name);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",value);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",type);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",address);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",base);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",bitstart);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",bitwidth);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",controlType);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",ioType);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",ioTypeName);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",phyPin);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",note);
    writer.writeEndElement();
    writeStructXmlFile(itemChild,writer);

    writer.writeEndElement();
  }
}
bool ModuleIO::getBit(QTreeWidgetItem *item, quint32 &value)
{
  QString memberType;
  qint16 address;
  qint16 base;
  quint16 bitstart;
  quint16 bitwidth;
  QString controlType;

  qint16 comId=mp_mainWindow->getUserConfig()->com.id;
  qint16 rnStation=mp_mainWindow->getUserConfig()->com.rnStation;

  quint16 value16;
  quint32 value32;

  memberType=item->text(TREE_IO_MEMTYPE_COL);
  address=item->text(TREE_IO_ADDR_COL).toShort(0,16);
  base=item->text(TREE_IO_BASEADDR_COL).toShort();
  bitstart=item->text(TREE_IO_BITSTART_COL).toUShort();
  bitwidth=item->text(TREE_IO_BITWIDTH_COL).toUShort();
  controlType=item->text(TREE_IO_CONTROLTYPE_COL);

  if(memberType.contains("16"))
  {
    if(0!=GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16 *)&value16,comId,rnStation))
      GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16*)&value16,comId,rnStation);
    value=value16;
  }
  else if(memberType.contains("32"))
  {
    if(0!=GTSD_CMD_Get32bitFPGAByAddr(base,address,(int32*)&value32,comId,rnStation))
      GTSD_CMD_Get32bitFPGAByAddr(base,address,(int32*)&value32,comId,rnStation);
    value=value32;
  }
  else
  {
    if(0!=GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16 *)&value16,comId,rnStation))
      GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16*)&value16,comId,rnStation);
    value=value16;
  }

  quint32 operation=operationValue(bitstart,bitwidth);

  bool ret=(bool)(operation&value);

//  qDebug()<<"ret "<<ret;
//  qDebug()<<"value "<<value;
  return ret;
}
quint32 ModuleIO::operationValue(quint32 bitstart, quint32 bitwidth)
{
  quint32 temp = 0;
  for (quint32 i = 0; i < bitwidth; i++)
  {
    temp <<= 1;
    temp += 1;
  }
  temp <<= bitstart;
  return temp;
}
bool ModuleIO::setBit(QTreeWidgetItem *item,bool bit)
{
  QString memberType;
  qint16 address;
  qint16 base;
  quint16 bitstart;
  quint16 bitwidth;
  QString controlType;

  qint16 comId=mp_mainWindow->getUserConfig()->com.id;
  qint16 rnStation=mp_mainWindow->getUserConfig()->com.rnStation;

  quint16 value16;
  quint32 value32;

  quint32 value=0;
  quint32 operation;

  memberType=item->text(TREE_IO_MEMTYPE_COL);
  address=item->text(TREE_IO_ADDR_COL).toShort(0,16);
  base=item->text(TREE_IO_BASEADDR_COL).toShort();
  bitstart=item->text(TREE_IO_BITSTART_COL).toUShort();
  bitwidth=item->text(TREE_IO_BITWIDTH_COL).toUShort();
  controlType=item->text(TREE_IO_CONTROLTYPE_COL);

  operation=operationValue(bitstart,bitwidth);

  //读数据 - 写  - 再读一次
  if(memberType.contains("16"))
  {
    if(0!=GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16 *)&value16,comId,rnStation))
      GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16*)&value16,comId,rnStation);
    value=value16;
    if(bit)//set 1
      value16=(quint16)(operation|value);
    else//set 0
      value16=(quint16)((~operation)&value);
    qDebug()<<"after :"<<QString::number(value16,2);
    if(0!=GTSD_CMD_Set16bitFPGAByAddr(base,address,(int16 )value16,comId,rnStation))
      GTSD_CMD_Set16bitFPGAByAddr(base,address,(int16)value16,comId,rnStation);

    if(0!=GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16 *)&value16,comId,rnStation))
      GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16*)&value16,comId,rnStation);
    value=value16;
  }
  else if(memberType.contains("32"))
  {
    if(0!=GTSD_CMD_Get32bitFPGAByAddr(base,address,(int32*)&value32,comId,rnStation))
      GTSD_CMD_Get32bitFPGAByAddr(base,address,(int32*)&value32,comId,rnStation);
    value=value32;
    if(bit)//set 1
      value32=(quint16)(operation|value);
    else//set 0
      value32=(quint16)((~operation)&value);
    qDebug()<<"after :"<<QString::number(value32,2);
    if(0!=GTSD_CMD_Set32bitFPGAByAddr(base,address,(int32 )value32,comId,rnStation))
      GTSD_CMD_Set32bitFPGAByAddr(base,address,(int16)value32,comId,rnStation);

    if(0!=GTSD_CMD_Get32bitFPGAByAddr(base,address,(int32*)&value32,comId,rnStation))
      GTSD_CMD_Get32bitFPGAByAddr(base,address,(int32*)&value32,comId,rnStation);
    value=value32;
  }
  else
  {
      if(0!=GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16 *)&value16,comId,rnStation))
        GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16*)&value16,comId,rnStation);
      value=value16;
      if(bit)//set 1
        value16=(quint16)(operation|value);
      else//set 0
        value16=(quint16)((~operation)&value);
      qDebug()<<"after :"<<QString::number(value16,2);
      if(0!=GTSD_CMD_Set16bitFPGAByAddr(base,address,(int16 )value16,comId,rnStation))
        GTSD_CMD_Set16bitFPGAByAddr(base,address,(int16)value16,comId,rnStation);

      if(0!=GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16 *)&value16,comId,rnStation))
        GTSD_CMD_Get16bitFPGAByAddr(base,address,(int16*)&value16,comId,rnStation);
      value=value16;
  }


  bool ret=(bool)(operation&value);

  qDebug()<<"ret "<<ret;
  return ret;

}
