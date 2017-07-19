#include "xmlbuilder.h"
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>
#include <QDir>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QFormBuilder>
#include <QTreeWidget>
#include <QTreeWidgetItemIterator>
#include <QVBoxLayout>

XmlBuilder::XmlBuilder(QObject *parent) : QObject(parent)
{

}
XmlBuilder::~XmlBuilder()
{
    SysType config;
    foreach (config, m_sysConfig.sysTypeList)
    {
      delete config.treeItem;
    }
}

//!--------------------------public function---------------------------------------------
bool XmlBuilder::parseSysConfig(QString &filename)
{
  //先清0
  if(m_sysConfig.comList.size()>0||m_sysConfig.sysTypeList.size()>0)
  {
    m_sysConfig.comList.clear();
    SysType config;
    foreach (config, m_sysConfig.sysTypeList)
    {
      delete config.treeItem;
    }
    m_sysConfig.sysTypeList.clear();
  }
  //开始解析
  QFile file(filename);
  if(!file.open(QIODevice::ReadOnly))
  {
    QMessageBox::information(0,tr("File Open"),tr("File open error!"));
    return false;
  }
  QXmlStreamReader reader;
  QString elementName;
  QXmlStreamAttributes xmlAttributes;
  reader.setDevice(&file);
  reader.readNext();
  while (!reader.atEnd())
  {
    if(reader.isStartElement())
    {
      elementName=reader.name().toString();
      if(elementName=="Com")
      {
        Com com;
        xmlAttributes=reader.attributes();
        com.id=xmlAttributes.value("id").toInt();
        com.comName=xmlAttributes.value("name").toString();
        m_sysConfig.comList.append(com);
      }
      else if(elementName=="Type")
      {
        readType(reader);
      }

    }
    reader.readNext();
  }
  if(reader.hasError())
  {
    QMessageBox::information(0,tr("XML File"),tr("XML file read error!"));
    return false;
  }
  return true;
}
bool XmlBuilder::parseUserConfig(QString &filename)
{
  QFile file(filename);
  if(!file.open(QIODevice::ReadOnly))
  {
    QMessageBox::information(0,tr("File Open"),tr("File open error!"));
    return false;
  }
  QXmlStreamReader reader;
  QString elementName;
  QString elementText;
  reader.setDevice(&file);
  reader.readNext();
  while (!reader.atEnd())
  {
    if(reader.isStartElement())
    {
      elementName=reader.name().toString();
      if(elementName=="Com")
      {
        reader.readNext();
        while(!reader.atEnd())
        {
          elementName=reader.name().toString();
          if(reader.isEndElement()&&(elementName=="Com")) break;
          if(reader.isStartElement())
          {
            if(elementName=="ID") m_userConfig.com.id=reader.readElementText().toInt();
            else if(elementName=="Name") m_userConfig.com.comName=reader.readElementText();
            else if(elementName=="Property")
            {
              reader.readNext();
              while(!reader.atEnd())
              {
                elementName=reader.name().toString();
                if(reader.isEndElement()&&elementName=="Property") break;
                if(reader.isStartElement())
                {
                  elementText=reader.readElementText();
                  if(elementName=="IpAddress") m_userConfig.com.ipAddress=elementText;
                  else if(elementName=="IpPort") m_userConfig.com.ipPort=elementText.toInt();
                  else if(elementName=="RnStation") m_userConfig.com.rnStation=elementText.toUShort();
                }
                reader.readNext();
              }
            }
          }
          reader.readNext();
        }
      }
      else if(elementName=="Type")
      {
        reader.readNext();
        while(!reader.atEnd())
        {
          elementName=reader.name().toString();
          if(reader.isEndElement()&&elementName=="Type") break;
          if(reader.isStartElement())
          {
            if(elementName=="ID") m_userConfig.typeId=reader.readElementText().toInt();
            else if(elementName=="Name") m_userConfig.typeName=reader.readElementText();
            else if(elementName=="Model")
            {
              reader.readNext();
              while(!reader.atEnd())
              {
                elementName=reader.name().toString();
                if(reader.isEndElement()&&elementName=="Model") break;
                if(reader.isStartElement())
                {
                  elementText=reader.readElementText();
                  if(elementName=="ModelName") m_userConfig.model.modelName=elementText;
                  else if(elementName=="ModelAxisCount")m_userConfig.model.axisCount=elementText.toInt();
                  else if(elementName=="FirmWare")m_userConfig.model.version.append(elementText);
                }
                reader.readNext();
              }
            }
          }
          reader.readNext();
        }
      }

    }
    reader.readNext();
  }
  if(reader.hasError())
  {
    QMessageBox::information(0,tr("XML File"),tr("XML file read error!"));
    return false;
  }
//  qDebug()<<"parse userconfig"<<filename;
  return true;
}

bool XmlBuilder::parseFileRecent(QString &filename)
{
//  qDebug()<<filename;
  QFile file(filename);
  if(!file.open(QIODevice::ReadOnly))
  {
    QMessageBox::information(0,tr("File Open"),tr("%1\nFile open error!").arg(filename));
    return false;
  }
  QXmlStreamReader reader;
  QString elementName;
  QString elementText;
  reader.setDevice(&file);
  reader.readNext();
  while (!reader.atEnd())
  {
    if(reader.isStartElement())
    {
      elementName=reader.name().toString();
      if(elementName=="FirstBit")
      {
        elementText=reader.readElementText();
        if(elementText=="1")
          m_fileRecent.firstBit=true;
        else
          m_fileRecent.firstBit=false;
      }
      else if(elementName=="File")
      {
        reader.readNext();
        while(!reader.atEnd())
        {
          elementName=reader.name().toString();
          if((elementName=="File")&&reader.isEndElement()) break;
          if((elementName=="FilePath")&&reader.isStartElement())
          {
            elementText=reader.readElementText();
            m_fileRecent.filePathList.append(elementText);
          }
          reader.readNext();
        }
      }

    }
    reader.readNext();
  }
  if(reader.hasError())
  {
    QMessageBox::information(0,tr("XML File"),tr("XML file read error!"));
    return false;
  }
  return true;
}
void XmlBuilder::saveUserConfigXml(QString &fileName ,FileRecent*recFile,UserConfig *userCfg)
{
  //---------save list.xml---------------------------
  QString filePath=CUSTOM_FILE_PATH+"configlist.xml";

  if (fileName.isNull()) return;
  QFile fileXML(filePath);
  if (!fileXML.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QMessageBox::information(0,tr("XML File"),tr("XML file open error!"));
    return;
  }

  QXmlStreamWriter writer(&fileXML);
  writer.setAutoFormatting(true);
  writer.writeStartDocument("1.0");
  writer.writeStartElement("CfgFileRecent");
  writer.writeTextElement("FirstBit", "0");
  writer.writeStartElement("File");

  recFile->filePathList.prepend(fileName);
  while(recFile->filePathList.size()>FILELIST_MAXSIZE)
    recFile->filePathList.removeLast();
  foreach (QString path, recFile->filePathList)
  {
    writer.writeTextElement("FilePath", path);
  }
//  for(int i=0;i<FILELIST_MAXSIZE;i++)
//  {
//    writer.writeTextElement("FilePath", "0");
//  }

  writer.writeEndElement();
  writer.writeEndElement();
  writer.writeEndDocument();
  fileXML.close();

  //-------------save configuser.xml-------------------
  fileXML.setFileName(fileName);
  if (!fileXML.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QMessageBox::information(0,tr("XML File"),tr("XML file open error!"));
    return;
  }
  writer.setDevice(&fileXML);
  writer.setAutoFormatting(true);
  writer.writeStartDocument("1.0");
  writer.writeStartElement("ConfigDefalut");
    writer.writeStartElement("Com");
      writer.writeTextElement("ID", QString::number(userCfg->com.id));
      writer.writeTextElement("Name", userCfg->com.comName);
      writer.writeStartElement("Property");
        writer.writeTextElement("IpAddress", userCfg->com.ipAddress);
        writer.writeTextElement("IpPort", QString::number(userCfg->com.ipPort));
        writer.writeTextElement("RnStation", tr("%1").arg(QString::number(userCfg->com.rnStation)));
      writer.writeEndElement();
    writer.writeEndElement();

    writer.writeStartElement("Type");
      writer.writeTextElement("ID", QString::number(userCfg->typeId));
      writer.writeTextElement("Name", userCfg->typeName);
      writer.writeStartElement("Model");
        writer.writeTextElement("ModelName", userCfg->model.modelName);
        writer.writeTextElement("ModelAxisCount", QString::number(userCfg->model.axisCount));
        writer.writeTextElement("FirmWare", userCfg->model.version.at(0));
      writer.writeEndElement();
    writer.writeEndElement();

  writer.writeEndElement();
  writer.writeEndDocument();
  fileXML.close();

  //-------------------删除多余的文件---------------------------
  QDir dir(CUSTOM_USER_FILE_PATH);
  QFileInfoList fileList;
  QStringList filter;
  filter<<"*.xml";
  dir.setNameFilters(filter);
  dir.setSorting(QDir::Time);
  fileList=dir.entryInfoList();
  if(fileList.size()>FILELIST_MAXSIZE)
  {
    for(int i=FILELIST_MAXSIZE;i<fileList.size();i++)
      dir.remove(fileList.at(i).filePath());
  }
}

//!--------------------------private function-------------------------------------
void XmlBuilder::readType(QXmlStreamReader &reader)
{
  QString elementName;
  QXmlStreamAttributes xmlAttributes;
  SysType sysType;
  xmlAttributes=reader.attributes();
  sysType.typeId=xmlAttributes.value("id").toString().toInt();
  sysType.typeName=xmlAttributes.value("name").toString();

  reader.readNext();

  while(!reader.atEnd())
  {
    elementName=reader.name().toString();
    if(reader.isEndElement()&&(elementName=="Type")) break;
    if(reader.isStartElement())
    {
      if(elementName=="Model")
      {
        readMode(sysType,reader);
      }
      else if(elementName=="Tree")
      {
        readTree(sysType,reader);
      }
    }
    reader.readNext();
  }
  m_sysConfig.sysTypeList.append(sysType);
}
void XmlBuilder::readMode(SysType &sysType, QXmlStreamReader &reader)
{
  QString elementName;
  QXmlStreamAttributes xmlAttributes;
  Model model;
  xmlAttributes=reader.attributes();
  model.modelName=xmlAttributes.value("name").toString();
  model.axisCount=xmlAttributes.value("axisCount").toInt();
  while(!reader.atEnd())
  {
    elementName=reader.name().toString();
    if(reader.isEndElement()&&(elementName=="Model")) break;
    if(reader.isStartElement())
    {
      if(elementName=="FirmWare")
      {
        xmlAttributes=reader.attributes();
        model.version.append(xmlAttributes.value("name").toString());
      }
    }
    reader.readNext();
  }
  sysType.modelList.append(model);
}
void XmlBuilder::readTree(SysType &sysType, QXmlStreamReader &reader)
{
  QTreeWidgetItem *itemRoot,*itemChild;
  QString elementName;
  QXmlStreamAttributes xmlAttributes;
  sysType.treeItem=new QTreeWidgetItem();
  while(!reader.atEnd())
  {
    elementName=reader.name().toString();
    if(elementName=="Tree"&&reader.isEndElement()) break;
    if(reader.isStartElement())
    {
      if(elementName=="Root")
      {
        itemRoot=new QTreeWidgetItem(sysType.treeItem);
        xmlAttributes=reader.attributes();
        itemRoot->setText(0,xmlAttributes.value("name").toString());

        while(!reader.atEnd())
        {
          elementName=reader.name().toString();
          if((elementName=="Root")&&reader.isEndElement()) break;
          if(reader.isStartElement())
          {
            if(elementName=="Child")
            {
              itemChild = new QTreeWidgetItem(itemRoot);
              xmlAttributes=reader.attributes();
              itemChild->setText(0,xmlAttributes.value("name").toString());
              itemChild->setText(1,xmlAttributes.value("classname").toString());
              itemChild->setText(2,xmlAttributes.value("PrmFuncFile").toString());
            }
          }
          reader.readNext();
        }
      }
    }
    reader.readNext();
  }
}

void XmlBuilder::writeStructXmlFile(QTreeWidgetItem *item, QXmlStreamWriter &writer)
{
  QTreeWidgetItem *itemChild;
  for (int i = 0; i<item->childCount();i++)
  {
    itemChild = item->child(i);
    QString name = itemChild->text(COL_NAME);
    QString value = itemChild->text(COL_VALUE);
    QString type = itemChild->text(COL_TYPE);
    QString offset = itemChild->text(COL_ADDRESS);
    QString isbits = itemChild->text(COL_ISBIT);
    QString bitstart = itemChild->text(COL_BITSTART);
    QString bitwidth = itemChild->text(COL_BITWIDTH);
    QString axisnum = itemChild->text(COL_AXISNUM);
    QString haschild = itemChild->text(COL_HASCHILD);
    QString childnum = itemChild->text(COL_CHILDNUM);
    QString parent = itemChild->text(COL_PARENT);
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
    writer.writeTextElement("string",offset);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",isbits);
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
    writer.writeTextElement("string",axisnum);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",haschild);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",childnum);
    writer.writeEndElement();

    writer.writeStartElement("property");
    writer.writeAttribute("name","text");
    writer.writeTextElement("string",parent);
    writer.writeEndElement();

    writeStructXmlFile(itemChild, writer);
    writer.writeEndElement();
  }
}

//void XmlBuilder::fullAxisTreeWidgetNormalization(QTreeWidget *srcTreeWidget)
//{
//  QTreeWidget *treeWidget=new QTreeWidget();
//  QTreeWidgetItem *item;
//  int nameIndex,valueIndex,typeIndex,addrOfstIndex;
//  for(int i=0;i<srcTreeWidget->topLevelItemCount();i++)
//  {
//    item=new QTreeWidgetItem();
//    item=srcTreeWidget->topLevelItem(i)->clone();
//    treeWidget->addTopLevelItem(item);
//  }

//  QStringList header;
//  header<<NORMAL_NAME<<NORMAL_VALUE<<NORMAL_TYPE<<NORMAL_OFFSET;
//  for(int i=0;i<srcTreeWidget->headerItem()->columnCount();i++)
//  {
//    if(NORMAL_NAME==srcTreeWidget->headerItem()->text(i))
//      nameIndex=i;
//    else if(NORMAL_VALUE==srcTreeWidget->headerItem()->text(i))
//      valueIndex=i;
//    else if(NORMAL_TYPE==srcTreeWidget->headerItem()->text(i))
//      typeIndex=i;
//    else if(NORMAL_OFFSET==srcTreeWidget->headerItem()->text(i))
//      addrOfstIndex=i;
//    srcTreeWidget->headerItem()->setText(i,header.at(i));
//  }
//  qDebug()<<NORMAL_NAME<<nameIndex<<NORMAL_VALUE<<valueIndex<<NORMAL_TYPE<<typeIndex<<NORMAL_OFFSET<<addrOfstIndex;

//  {
//    QTreeWidgetItemIterator it_src(srcTreeWidget);
//    QTreeWidgetItemIterator it_copy(treeWidget);
//    while((*it_src))
//    {
//      (*it_src)->setText(COL_NAME,(*it_copy)->text(nameIndex));
//      (*it_src)->setText(COL_VALUE,(*it_copy)->text(valueIndex));
//      (*it_src)->setText(COL_TYPE,(*it_copy)->text(typeIndex));
//      (*it_src)->setText(COL_ADDRESS,(*it_copy)->text(addrOfstIndex));
//      it_src++;
//      it_copy++;
//    }
//  }
//  srcTreeWidget->resizeColumnToContents(COL_NAME);
//  srcTreeWidget->resizeColumnToContents(COL_VALUE);
//  srcTreeWidget->resizeColumnToContents(COL_TYPE);
//  srcTreeWidget->resizeColumnToContents(COL_ADDRESS);
//  srcTreeWidget->expandAll();

//  treeWidget->clear();
//  delete treeWidget;
//}
void XmlBuilder::ramFlashTreeWidgetNormalization(QTreeWidget *srcTreeWidget)
{
  QTreeWidget *treeWidget=new QTreeWidget();
  QTreeWidgetItem *item;
  int nameIndex,valueIndex,typeIndex,addrOfstIndex,isbitIndex,bitstartIndex;
  int bitwidthIndex,axisnumIndex,haschildIndex,childnumIndex,parentIndex;
  for(int i=0;i<srcTreeWidget->topLevelItemCount();i++)
  {
//    item=new QTreeWidgetItem();
    item=srcTreeWidget->topLevelItem(i)->clone();
    treeWidget->addTopLevelItem(item);
  }

  QStringList header;
  header<<NORMAL_NAME<<NORMAL_VALUE<<NORMAL_TYPE<<NORMAL_OFFSET\
        <<NORMAL_ISBITS<<NORMAL_BITSTART<<NORMAL_BITWIDTH\
        <<NORMAL_AXISNUM<<NORMAL_HASCHILD<<NORMAL_CHILDNUM<<NORMAL_PARENT;

  for(int i=0;i<srcTreeWidget->headerItem()->columnCount();i++)
  {
    if(NORMAL_NAME==srcTreeWidget->headerItem()->text(i))
      nameIndex=i;
    else if(NORMAL_VALUE==srcTreeWidget->headerItem()->text(i))
      valueIndex=i;
    else if(NORMAL_TYPE==srcTreeWidget->headerItem()->text(i))
      typeIndex=i;
    else if(NORMAL_OFFSET==srcTreeWidget->headerItem()->text(i))
      addrOfstIndex=i;
    else if(NORMAL_ISBITS==srcTreeWidget->headerItem()->text(i))
      isbitIndex=i;
    else if(NORMAL_BITSTART==srcTreeWidget->headerItem()->text(i))
      bitstartIndex=i;
    else if(NORMAL_BITWIDTH==srcTreeWidget->headerItem()->text(i))
      bitwidthIndex=i;
    else if(NORMAL_AXISNUM==srcTreeWidget->headerItem()->text(i))
      axisnumIndex=i;
    else if(NORMAL_HASCHILD==srcTreeWidget->headerItem()->text(i))
      haschildIndex=i;
    else if(NORMAL_CHILDNUM==srcTreeWidget->headerItem()->text(i))
      childnumIndex=i;
    else if(NORMAL_PARENT==srcTreeWidget->headerItem()->text(i))
      parentIndex=i;

    srcTreeWidget->headerItem()->setText(i,header.at(i));
  }
  qDebug()<<NORMAL_NAME<<nameIndex<<NORMAL_VALUE<<valueIndex<<NORMAL_TYPE<<typeIndex<<NORMAL_OFFSET<<addrOfstIndex;

  {
    QTreeWidgetItemIterator it_src(srcTreeWidget);
    QTreeWidgetItemIterator it_copy(treeWidget);
    while((*it_src))
    {
      (*it_src)->setText(COL_NAME,(*it_copy)->text(nameIndex));
      (*it_src)->setText(COL_VALUE,(*it_copy)->text(valueIndex));
      (*it_src)->setText(COL_TYPE,(*it_copy)->text(typeIndex));
      (*it_src)->setText(COL_ADDRESS,(*it_copy)->text(addrOfstIndex));
      (*it_src)->setText(COL_ISBIT,(*it_copy)->text(isbitIndex));
      (*it_src)->setText(COL_BITSTART,(*it_copy)->text(bitstartIndex));
      (*it_src)->setText(COL_BITWIDTH,(*it_copy)->text(bitwidthIndex));
      (*it_src)->setText(COL_AXISNUM,(*it_copy)->text(axisnumIndex));
      (*it_src)->setText(COL_HASCHILD,(*it_copy)->text(haschildIndex));
      (*it_src)->setText(COL_CHILDNUM,(*it_copy)->text(childnumIndex));
      (*it_src)->setText(COL_PARENT,(*it_copy)->text(parentIndex));
      item=(*it_src);
      if(item->childCount()>0)
        item->setText(COL_VALUE,tr("  NONE  "));
      it_src++;
      it_copy++;
    }
  }
  treeWidget->clear();
  delete treeWidget;
}

void XmlBuilder::functionTreeWidgetNormalization(QTreeWidget *srcTreeWidget)
{
  QTreeWidget *treeWidget=new QTreeWidget();
  QTreeWidgetItem *item;
  int nameIndex,valueIndex,typeIndex,unitIndex,introIndex,upIndex,downIndex,framOffsetIndex;
  for(int i=0;i<srcTreeWidget->topLevelItemCount();i++)
  {
//    item=new QTreeWidgetItem();
    item=srcTreeWidget->topLevelItem(i)->clone();
    treeWidget->addTopLevelItem(item);
  }

  QStringList header;
  header<<NORMAL_NAME<<NORMAL_VALUE<<NORMAL_TYPE<<NORMAL_UINT<<NORMAL_DOWNLIMIT<<NORMAL_UPLIMIT<<NORMAL_INTRODUCTION<<NORMAL_FRAMOFFSET;
  for(int i=0;i<srcTreeWidget->headerItem()->columnCount();i++)
  {
    if(NORMAL_NAME==srcTreeWidget->headerItem()->text(i))
      nameIndex=i;
    else if(NORMAL_VALUE==srcTreeWidget->headerItem()->text(i))
      valueIndex=i;
    else if(NORMAL_TYPE==srcTreeWidget->headerItem()->text(i))
      typeIndex=i;
    else if(NORMAL_UINT==srcTreeWidget->headerItem()->text(i))
      unitIndex=i;
    else if(NORMAL_DOWNLIMIT==srcTreeWidget->headerItem()->text(i))
      downIndex=i;
    else if(NORMAL_UPLIMIT==srcTreeWidget->headerItem()->text(i))
      upIndex=i;
    else if(NORMAL_INTRODUCTION==srcTreeWidget->headerItem()->text(i))
      introIndex=i;
    else if(NORMAL_FRAMOFFSET==srcTreeWidget->headerItem()->text(i))
      framOffsetIndex=i;
    srcTreeWidget->headerItem()->setText(i,header.at(i));
  }
//  qDebug()<<NORMAL_NAME<<nameIndex<<NORMAL_VALUE<<valueIndex<<NORMAL_TYPE;

  {
    QTreeWidgetItemIterator it_src(srcTreeWidget);
    QTreeWidgetItemIterator it_copy(treeWidget);
    while((*it_src))
    {
      (*it_src)->setText(COL_FUNC_NAME,(*it_copy)->text(nameIndex));
      (*it_src)->setText(COL_FUNC_VALUE,(*it_copy)->text(valueIndex));
      (*it_src)->setText(COL_FUNC_TYPE,(*it_copy)->text(typeIndex));
      (*it_src)->setText(COL_FUNC_UINT,(*it_copy)->text(unitIndex));
      (*it_src)->setText(COL_FUNC_DOWNLIMIT,(*it_copy)->text(downIndex));
      (*it_src)->setText(COL_FUNC_UPLIMIT,(*it_copy)->text(upIndex));
      (*it_src)->setText(COL_FUNC_INTRODUCTION,(*it_copy)->text(introIndex));
      (*it_src)->setText(COL_FUNC_OFFSET,(*it_copy)->text(framOffsetIndex));
      it_src++;
      it_copy++;
    }
  }
  treeWidget->clear();
  delete treeWidget;
}

QTreeWidget * XmlBuilder::readTreeWidgetFromFile(QString &filename)
{
  QFormBuilder builder;
  QFile file(filename);

  if (!file.open(QFile::ReadOnly))
  {
    QMessageBox::information(0, tr("open error"), tr("open %1 file error ").arg(filename));
    return NULL;
  }
  QWidget *widget=NULL;
  QTreeWidget *treeWidget=NULL;

  widget = builder.load(&file, 0);
  if (widget==NULL)
  {
    QMessageBox::information(0, tr("load error"), tr("load .xml file error "));
    file.close();
    return NULL;
  }
  file.close();

//  QTreeWidget *tree=widget->findChild<QTreeWidget*>("treeWidget");//可以这样子查找
//  QList<QTreeWidget *>trees=widget->findChildren<QTreeWidget*>("treeWidget");

  QObjectList list = widget->children();
  foreach(QObject *obj, list)
  {
//    qDebug() << obj->objectName();
    if ("treeWidget" == obj->objectName())
    {
      QTreeWidget *tree = static_cast<QTreeWidget *>(obj);
      treeWidget=new QTreeWidget();
      QTreeWidgetItem *item;
      for(int i=0;i<tree->topLevelItemCount();i++)
      {
//        item=new QTreeWidgetItem();
        item=tree->topLevelItem(i)->clone();
        treeWidget->addTopLevelItem(item);
      }
//      QTreeWidgetItem *head=new QTreeWidgetItem();
      QTreeWidgetItem *head;
      head=tree->headerItem()->clone();
      treeWidget->setHeaderItem(head);
      break;
    }
  }
  delete widget;

  if(treeWidget!=NULL)
    return treeWidget;
  else
    return NULL;
}

void XmlBuilder::saveFlashAllAxisTreeFile(QString &fileName, QTreeWidget *treeWidget)
{
  QFile fileXML(fileName);
  if (!fileXML.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QMessageBox::information(0,tr("XML File"),tr("XML file save error!"));
    return;
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
      writer.writeTextElement("string", "addressOffset");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "isBits");
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
      writer.writeTextElement("string", "axisNum");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "hasChild");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "childNum");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "parent");
      writer.writeEndElement();
      writer.writeEndElement();

      //开始写树结构
      QTreeWidgetItem *item;
      int itemCount=treeWidget->topLevelItemCount();
      for(int i=0;i<itemCount;i++)
      {
        item=treeWidget->topLevelItem(i);
        QString name = item->text(COL_NAME);
        QString value = item->text(COL_VALUE);
        QString type = item->text(COL_TYPE);
        QString offset = item->text(COL_ADDRESS);
        QString isbits = item->text(COL_ISBIT);
        QString bitstart = item->text(COL_BITSTART);
        QString bitwidth = item->text(COL_BITWIDTH);
        QString axisnum = item->text(COL_AXISNUM);
        QString haschild = item->text(COL_HASCHILD);
        QString childnum = item->text(COL_CHILDNUM);
        QString parent = item->text(COL_PARENT);
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
        writer.writeTextElement("string",offset);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",isbits);
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
        writer.writeTextElement("string",axisnum);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",haschild);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",childnum);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",parent);
        writer.writeEndElement();
        writeStructXmlFile(item,writer);
        writer.writeEndElement();
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
}
void XmlBuilder::saveFunctionTreeFile(QString &fileName,QTreeWidget *treeWidget)
{
  QFile fileXML(fileName);
  if (!fileXML.open(QIODevice::WriteOnly))
  {
    QMessageBox::information(0,tr("XML File"),tr("XML file save error!\n%1").arg(fileName));
    return;
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
      writer.writeTextElement("string", "unit");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "downlimit");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "uplimit");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "introduction");
      writer.writeEndElement();
      writer.writeEndElement();

      writer.writeStartElement("column");
      writer.writeStartElement("property");
      writer.writeAttribute("name","text");
      writer.writeTextElement("string", "framAddr");
      writer.writeEndElement();
      writer.writeEndElement();

      //开始写树结构
      QTreeWidgetItem *item;
      int itemCount=treeWidget->topLevelItemCount();
      for(int i=0;i<itemCount;i++)
      {
        item=treeWidget->topLevelItem(i);
        QString name = item->text(COL_FUNC_NAME);
        QString value = item->text(COL_FUNC_VALUE);
        QString type = item->text(COL_FUNC_TYPE);
        QString unit = item->text(COL_FUNC_UINT);
        QString downlimit = item->text(COL_FUNC_DOWNLIMIT);
        QString uplimit = item->text(COL_FUNC_UPLIMIT);
        QString introduction = item->text(COL_FUNC_INTRODUCTION);
        QString framAddr = item->text(COL_FUNC_OFFSET);
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
        writer.writeTextElement("string",unit);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",downlimit);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",uplimit);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",introduction);
        writer.writeEndElement();

        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string",framAddr);
        writer.writeEndElement();

        writer.writeEndElement();
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
}

void XmlBuilder::setTreeValueFromTree(QTreeWidget *dstTree, QTreeWidget *srcTree)
{
  /*QTreeWidgetItemIterator itOld(srcTree);
  QString oldName;
  QString newName;
  QString oldValue;
  QString newValue;
  while (*itOld)
  {
    oldName=(*itOld)->text(COL_NAME);
    oldName=oldName.trimmed();
    oldValue=(*itOld)->text(COL_VALUE);
    oldValue=oldValue.trimmed();

    QTreeWidgetItemIterator itNew(dstTree);
    while(*itNew)
    {
      newName=(*itNew)->text(COL_NAME);
      newName=newName.trimmed();
      newValue=(*itNew)->text(COL_VALUE);
      newValue=newValue.trimmed();

      if(0==oldName.compare(newName))
      {
        if(0!=oldValue.compare(newValue))
        {
          (*itNew)->setText(COL_VALUE,oldValue);
          (*itNew)->setTextColor(COL_VALUE,Qt::red);
        }
        break;
      }
      ++itNew;
    }
    ++itOld;
  }*/

  QTreeWidgetItemIterator itDst(dstTree);
  QString srcName;
  QString dstName;
  QString srcValue;
  QString dstValue;
  QTreeWidgetItem *dstItem;
  QTreeWidgetItem *srcItem;
  while (*itDst)
  {
    dstItem=(*itDst);
    dstName=dstItem->text(COL_NAME);
    dstName=dstName.trimmed();
    dstValue=dstItem->text(COL_VALUE);
    dstValue=dstValue.trimmed();
//    qDebug()<<"dstItem "<<dstName;
    QTreeWidgetItemIterator itSrc(srcTree);
    while (*itSrc)
    {
      srcItem=(*itSrc);
      srcName=srcItem->text(COL_NAME);
      srcName=srcName.trimmed();
      srcValue=srcItem->text(COL_VALUE);
      srcValue=srcValue.trimmed();
//      qDebug()<<"srcItem "<<srcName;
      if(0==dstName.compare(srcName))
      {
//        qDebug()<<"name compare";
        if(0!=dstValue.compare(srcValue))
        {
          dstItem->setText(COL_VALUE,srcValue);
          dstItem->setTextColor(COL_VALUE,Qt::red);
//          qDebug()<<"setvalue "<<dstItem->text(COL_NAME);
        }
        break;
      }
      itSrc++;
    }

    itDst++;
  }
}
