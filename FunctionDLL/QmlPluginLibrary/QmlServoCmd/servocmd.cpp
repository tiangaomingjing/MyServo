#include "servocmd.h"
#include "xmlbuilder.h"
#include "ServoDriverComDll.h"
#include "../TreeModelLibrary/treemodel.h"
#include "../TreeModelLibrary/treeitem.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QFormBuilder>
#include <QFile>
#include <QMessageBox>
typedef enum{
  ROW_ADVCMD_INDEX_BASEADDRESS=0,
  ROW_ADVCMD_INDEX_CONTROLSTART
}RowAdvCmdIndex;

typedef enum{
  COL_ADVCMD_INDEX_NAME=0,
  COL_ADVCMD_INDEX_VALUE,
  COL_ADVCMD_INDEX_TYPE,
  COL_ADVCMD_INDEX_ADDRESS
}ColumnAdvCmdIndex;

ServoCmd::ServoCmd(QObject *parent):
  QObject(parent),
  m_axisIndex(0),
  m_comType(0),
  m_comRnStation(240)
{
  m_extendTree=new QTreeWidget;
  m_cmdTree=new QTreeWidget;
}
ServoCmd::~ServoCmd()
{
  qDebug()<<"servocmd release->";
  m_extendTree->clear();
  delete m_extendTree;
  m_cmdTree->clear();
  delete m_cmdTree;
  qDebug()<<"servocmd release2->";
}

void ServoCmd::loadExtendTree(const QUrl & extendTreeFilePath)
{
  if(m_extendTree->topLevelItemCount()>0)
  {
    m_extendTree->clear();
  }
  QFormBuilder builder;
  QFile file(extendTreeFilePath.toLocalFile());

  if (!file.open(QFile::ReadOnly))
  {
    QMessageBox::information(0, tr("open error"), tr("open %1 file error ").arg(extendTreeFilePath.toLocalFile()));
    return ;
  }
  QWidget *widget=NULL;
  widget = builder.load(&file, 0);
  if (widget==NULL)
  {
    QMessageBox::information(0, tr("load error"), tr("load .xml file error "));
    return ;
  }
  file.close();

  QTreeWidget *tree=widget->findChild<QTreeWidget*>("treeWidget");//可以这样子查找
  QTreeWidgetItem *item;
  for(int i=0;i<tree->topLevelItemCount();i++)
  {
    item=tree->topLevelItem(i)->clone();
    m_extendTree->addTopLevelItem(item);
  }
  QTreeWidgetItem *head;
  head=tree->headerItem()->clone();
  m_extendTree->setHeaderItem(head);

  delete widget;
}

unsigned short ServoCmd::getOffsetAddress(int axis)
{
  //---------------从ExtensionTree中获得Flash的偏移地址--------------
  unsigned short offsetAddr=0;
  QTreeWidget *funcExtion=m_extendTree;
  QTreeWidgetItem *aitem;
  QTreeWidgetItem *itemChild;

  for(int i=0;i<funcExtion->topLevelItemCount();i++)
  {
    aitem=funcExtion->topLevelItem(i);
    if("offsetAddress"==aitem->text(COL_FUNC_EXTENSION_NAME))
    {
      itemChild=aitem->child(axis);
      offsetAddr=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
      break;
    }
  }
  return offsetAddr;
}

int ServoCmd::getDspTotalNumber()
{
  unsigned short num=0;
  QTreeWidget *funcExtion=m_extendTree;
  QTreeWidgetItem *aitem;
  QTreeWidgetItem *itemChild;
  aitem=funcExtion->topLevelItem(1);
  itemChild=aitem->child(0);
  num=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
  return num;
}

int ServoCmd::getDspAxis()
{
  unsigned short num=0;
  QTreeWidget *funcExtion=m_extendTree;
  QTreeWidgetItem *aitem;
  QTreeWidgetItem *itemChild;
  aitem=funcExtion->topLevelItem(1);
  itemChild=aitem->child(0);
  num=itemChild->child(0)->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
  return num;
}

void ServoCmd::loadCmdTree(const QUrl & cmdTreeFilePath)
{
  qDebug()<<"load cmdtree file: "<<cmdTreeFilePath.toLocalFile();
  if(m_cmdTree->topLevelItemCount()>0)
  {
    m_cmdTree->clear();
  }
  QFormBuilder builder;
  QFile file(cmdTreeFilePath.toLocalFile());

  if (!file.open(QFile::ReadOnly))
  {
    QMessageBox::information(0, tr("open error"), tr("open %1 file error ").arg(cmdTreeFilePath.toLocalFile()));
    return ;
  }
  QWidget *widget=NULL;
  widget = builder.load(&file, 0);
  if (widget==NULL)
  {
    QMessageBox::information(0, tr("load error"), tr("load .xml file error "));
    return ;
  }
  file.close();

  QTreeWidget *tree=widget->findChild<QTreeWidget*>("treeWidget");//可以这样子查找
  QTreeWidgetItem *item;
  for(int i=0;i<tree->topLevelItemCount();i++)
  {
    item=tree->topLevelItem(i)->clone();
    m_cmdTree->addTopLevelItem(item);
  }
  QTreeWidgetItem *head;
  head=tree->headerItem()->clone();
  m_cmdTree->setHeaderItem(head);

  delete widget;

  //将数据存到map中
  m_cmdTreeDataMaps.clear();
  QTreeWidgetItem *topItem;
  for(int i=0;i<m_cmdTree->topLevelItemCount();i++)
  {
    topItem=m_cmdTree->topLevelItem(i);
    fillCmdTreeDataMaps(topItem);
//    qDebug()<<topItem->text(0)<<" copy to struct data";
  }
  //观察其内容输出
  /*QMapIterator<QString ,CmdTreeData> i(m_cmdTreeDataMaps);
  const CmdTreeData *data;
  while (i.hasNext())
  {
    i.next();
    data=&(i.value());
    qDebug()<<"---------\\n";
    qDebug()<<QObject::tr("name:%1,unit:%2,cmd:%3,length:%4,setIndex:%5,getIndex:%6,kgain:%7,id:%8")\
            .arg(i.key()).arg(data->unit).arg(data->cmd).arg(data->length).arg(data->setIndex)\
            .arg(data->getIndex).arg(data->kgain).arg(data->id);
  }*/
}
void ServoCmd::writeFunctionTreeModel2Flash(TreeModel *model)
{
  TreeItem *rootItem=model->invisibleRootItem();
  for(int i=0;i<model->childrenCount();i++)
  {
    writeFunctionTreeItem2Flash(rootItem->child(i));
  }
}

void ServoCmd::writeFunctionValue2Flash(const QUrl & treeFilePath,const QString &wname, double value)
{
  qDebug()<<"load function file: "<<treeFilePath.toLocalFile();
  QTreeWidget *srcTree=NULL;

  QFormBuilder builder;
  QFile file(treeFilePath.toLocalFile());

  if (!file.open(QFile::ReadOnly))
  {
    QMessageBox::information(0, tr("open error"), tr("open %1 file error ").arg(treeFilePath.toLocalFile()));
    return ;
  }
  QWidget *widget=NULL;
  widget = builder.load(&file, 0);
  if (widget==NULL)
  {
    QMessageBox::information(0, tr("load error"), tr("load .xml file error "));
    return ;
  }
  file.close();

  srcTree=widget->findChild<QTreeWidget*>("treeWidget");//查找到树

  double kgain=0;//从cmdTree找
  unsigned short offsetAddr=0;//从extendTree找

  //从src中找
  QString valueType;
  QString offset;

  {
    QString str;
    QTreeWidgetItemIterator it(srcTree);
    QTreeWidgetItem *item;
    while(*it)
    {
      item=(*it);
      str=item->text(COL_FUNC_NAME);
      if(str==wname)
      {
        valueType=item->text(COL_FUNC_TYPE);
        offset=item->text(COL_FUNC_OFFSET);
        break;
      }
      it++;
    }
  }

  //---------------从ExtensionTree中获得Flash的偏移地址--------------
  QTreeWidget *funcExtion=m_extendTree;
  QTreeWidgetItem *aitem;
  QTreeWidgetItem *itemChild;

  for(int i=0;i<funcExtion->topLevelItemCount();i++)
  {
    aitem=funcExtion->topLevelItem(i);
    if("offsetAddress"==aitem->text(COL_FUNC_EXTENSION_NAME))
    {
      itemChild=aitem->child(m_axisIndex);
      offsetAddr=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
      break;
    }
  }
  offsetAddr+=offset.toUInt();

  //--------------------找增益-------------
  const CmdTreeData *cmdTree=NULL;
  cmdTree=&(m_cmdTreeDataMaps.value(wname));
  if(cmdTree!=NULL)
    kgain=cmdTree->kgain;

  double setValue=0;
  setValue=value*kgain;
  if(valueType.contains("64"))
  {
    GTSD_CMD_Fram_Write64BitByAdr(m_axisIndex,(int16)offsetAddr,(int64)setValue,(int16)m_comType,(int16)m_comRnStation);
  }
  else if(valueType.contains("32"))
  {
    GTSD_CMD_Fram_Write32BitByAdr(m_axisIndex,(int16)offsetAddr,(int32)setValue,(int16)m_comType,(int16)m_comRnStation);
  }
  else
  {
    GTSD_CMD_Fram_Write16BitByAdr(m_axisIndex,(int16)offsetAddr,(int16)setValue,(int16)m_comType,(int16)m_comRnStation);
  }

  delete widget;
}

double ServoCmd::readFunctionValueFromFlash(const QUrl &treeFilePath, const QString &wname)
{
  int16 value16 = 0;
  int32 value32 = 0;
  int64 value64 = 0;

  double retValue=0.0;

  double kgain=0;//从cmdTree找
  unsigned short offsetAddr=0;//从extendTree找

  //从src中找
  QString valueType;
  QString offset;

  qDebug()<<"load function file: "<<treeFilePath.toLocalFile();
  QTreeWidget *srcTree=NULL;

  QFormBuilder builder;
  QFile file(treeFilePath.toLocalFile());

  if (!file.open(QFile::ReadOnly))
  {
    QMessageBox::information(0, tr("open error"), tr("open %1 file error ").arg(treeFilePath.toLocalFile()));
    return -1;
  }
  QWidget *widget=NULL;
  widget = builder.load(&file, 0);
  if (widget==NULL)
  {
    QMessageBox::information(0, tr("load error"), tr("load .xml file error "));
    return -1;
  }
  file.close();

  srcTree=widget->findChild<QTreeWidget*>("treeWidget");//查找到树

  {
    QString str;
    QTreeWidgetItemIterator it(srcTree);
    QTreeWidgetItem *item;
    while(*it)
    {
      item=(*it);
      str=item->text(COL_FUNC_NAME);
      if(str==wname)
      {
        valueType=item->text(COL_FUNC_TYPE);
        offset=item->text(COL_FUNC_OFFSET);
        break;
      }
      it++;
    }
  }

  //---------------从ExtensionTree中获得Flash的偏移地址--------------
  QTreeWidget *funcExtion=m_extendTree;
  QTreeWidgetItem *aitem;
  QTreeWidgetItem *itemChild;

  for(int i=0;i<funcExtion->topLevelItemCount();i++)
  {
    aitem=funcExtion->topLevelItem(i);
    if("offsetAddress"==aitem->text(COL_FUNC_EXTENSION_NAME))
    {
      itemChild=aitem->child(m_axisIndex);
      offsetAddr=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
      break;
    }
  }
  offsetAddr+=offset.toUInt();

  //--------------------找增益-------------
  const CmdTreeData *cmdTree=NULL;
  cmdTree=&(m_cmdTreeDataMaps.value(wname));
  if(cmdTree!=NULL)
    kgain=cmdTree->kgain;


  if(valueType.contains("64"))
  {
    GTSD_CMD_Fram_Read64BitByAdr(m_axisIndex,(int16)offsetAddr,&value64,(int16)m_comType,(int16)m_comRnStation);
    retValue=value64;

  }
  else if(valueType.contains("32"))
  {
    GTSD_CMD_Fram_Read32BitByAdr(m_axisIndex,(int16)offsetAddr,&value32,(int16)m_comType,(int16)m_comRnStation);
    retValue=value32;
  }
  else
  {
    GTSD_CMD_Fram_Read16BitByAdr(m_axisIndex,(int16)offsetAddr,&value16,(int16)m_comType,(int16)m_comRnStation);
    retValue=value16;
  }
  retValue=retValue/kgain;
  delete widget;

  return retValue;
}

void ServoCmd::updateFunctionTreeModelFromFlash(TreeModel *srcModel)
{
  TreeItem *rootItem=srcModel->invisibleRootItem();
  for(int i=0;i<srcModel->childrenCount();i++)
  {
    updateFunctionTreeItemFromFlash(rootItem->child(i));
  }
  srcModel->updataTreeUi();
}

bool ServoCmd::loadAdvanceFastAccessCommandTree(const QUrl &treePath)
{
  bool ret=true;

  qDebug()<<"load function file: "<<treePath.toLocalFile();
  QTreeWidget *srcTree=NULL;

  QFormBuilder builder;
  QFile file(treePath.toLocalFile());

  if (!file.open(QFile::ReadOnly))
  {
    QMessageBox::information(0, tr("open error"), tr("open %1 file error ").arg(treePath.toLocalFile()));
    ret=false ;
  }
  QWidget *widget=NULL;
  widget = builder.load(&file, 0);
  if (widget==NULL)
  {
    QMessageBox::information(0, tr("load error"), tr("load .xml file error "));
    ret=false ;
  }
  file.close();

  srcTree=widget->findChild<QTreeWidget*>("treeWidget");//查找到树

  m_advFastCmdDataMaps.clear();
  m_addressOffsetVectors.clear();
  //存储轴的偏移地址
  QTreeWidgetItem *treeItem;
  treeItem=srcTree->topLevelItem(ROW_ADVCMD_INDEX_BASEADDRESS);//第一个字节点存储的是偏移地址信息
  for(int i=0;i<treeItem->childCount();i++)
  {
    m_addressOffsetVectors.append(treeItem->child(i)->text(COL_ADVCMD_INDEX_VALUE).toUInt());
  }
  AdvFastCmdData data;
  for(int i=ROW_ADVCMD_INDEX_CONTROLSTART;i<srcTree->topLevelItemCount();i++)
  {
    treeItem=srcTree->topLevelItem(i);
    data.value=treeItem->text(COL_ADVCMD_INDEX_VALUE);
    data.valueType=treeItem->text(COL_ADVCMD_INDEX_TYPE);
    data.address=treeItem->text(COL_ADVCMD_INDEX_ADDRESS).toUInt();
    m_advFastCmdDataMaps.insert(treeItem->text(COL_ADVCMD_INDEX_NAME),data);
  }
  delete widget;
  //test
  /*for(int i=0;i<m_addressOffsetVectors.count();i++)
  {
    qDebug()<<tr("%1 m_addressOffsetVectors:%2").arg(i).arg(m_addressOffsetVectors.at(i));
  }
  QMapIterator<QString,AdvFastCmdData>it(m_advFastCmdDataMaps);
  while(it.hasNext())
  {
    it.next();
    data=it.value();
    qDebug()<<tr("key:%1 value:V-%2,T-%3,A-%4").arg(it.key()).arg(data.value).arg(data.valueType).arg(data.address);
  }*/

  return ret;
}

quint64 ServoCmd::writeAdvanceFlash(const QString &cmdName, qint64 value)
{
  quint64 retValue=0;
  int32 value32=-1;
  int64 value64=-1;
  int16 value16=-1;
  if(m_advFastCmdDataMaps.contains(cmdName)==false)
    return 0;
  const AdvFastCmdData *data=&(m_advFastCmdDataMaps.value(cmdName));
  QString valueType=data->valueType;
  quint16 offsetValue=data->address;
  offsetValue+=m_addressOffsetVectors.at(m_axisIndex);//加多一个基于轴号的偏移地址
  if(valueType.contains("64"))
  {
    int ret=GTSD_CMD_Fram_Write64BitByAdr(m_axisIndex,(int16)offsetValue,(int64)value,(int16)m_comType,(int16)m_comRnStation);
    if(ret!=0)
      ret=GTSD_CMD_Fram_Write64BitByAdr(m_axisIndex,(int16)offsetValue,(int64)value,(int16)m_comType,(int16)m_comRnStation);
    if(ret==0)//成功写入
    {
      GTSD_CMD_Fram_Read64BitByAdr(m_axisIndex, (int16)offsetValue, &value64, (int16)m_comType,(int16)m_comRnStation);//读回来
      retValue=value64;
    }
  }
  else if(valueType.contains("32"))
  {
    int ret=GTSD_CMD_Fram_Write32BitByAdr(m_axisIndex,(int16)offsetValue,(int32)value,(int16)m_comType,(int16)m_comRnStation);
    if(ret!=0)
      ret=GTSD_CMD_Fram_Write32BitByAdr(m_axisIndex,(int16)offsetValue,(int32)value,(int16)m_comType,(int16)m_comRnStation);
    if(ret==0)//成功写入
    {
      GTSD_CMD_Fram_Read32BitByAdr(m_axisIndex, (int16)offsetValue, &value32, (int16)m_comType,(int16)m_comRnStation);//读回来
      retValue=value32;
    }
  }
  else
  {
    int ret=GTSD_CMD_Fram_Write16BitByAdr(m_axisIndex,(int16)offsetValue,(int16)value,(int16)m_comType,(int16)m_comRnStation);
    if(ret!=0)
      ret=GTSD_CMD_Fram_Write16BitByAdr(m_axisIndex,(int16)offsetValue,(int16)value,(int16)m_comType,(int16)m_comRnStation);
    if(ret==0)//成功写入
    {
      GTSD_CMD_Fram_Read16BitByAdr(m_axisIndex, (int16)offsetValue, &value16, (int16)m_comType,(int16)m_comRnStation);//读回来
      retValue=value16;
    }
  }
  return retValue;
}
quint64 ServoCmd::readAdvanceFlash(const QString &cmdName)
{
  quint64 retValue=0;
  int32 value32=-1;
  int64 value64=-1;
  int16 value16=-1;
  const AdvFastCmdData *data=&(m_advFastCmdDataMaps.value(cmdName));
  QString valueType=data->valueType;
  quint16 offsetValue=data->address;
  offsetValue+=m_addressOffsetVectors.at(m_axisIndex);//加多一个基于轴号的偏移地址
  if(valueType.contains("64"))
  {
    GTSD_CMD_Fram_Read64BitByAdr(m_axisIndex, (int16)offsetValue, &value64, (int16)m_comType,(int16)m_comRnStation);//读回来
    retValue=value64;
  }
  else if(valueType.contains("32"))
  {
    GTSD_CMD_Fram_Read32BitByAdr(m_axisIndex, (int16)offsetValue, &value32, (int16)m_comType,(int16)m_comRnStation);//读回来
    retValue=value32;
  }
  else
  {
    GTSD_CMD_Fram_Read16BitByAdr(m_axisIndex, (int16)offsetValue, &value16, (int16)m_comType,(int16)m_comRnStation);//读回来
    retValue=value16;
  }
  return retValue;
}
double ServoCmd::getKgian(const QString &cmdName)
{
  //获取cmd相关信息
  double ret=-1;
  const CmdTreeData *data;
  data=&(m_cmdTreeDataMaps.value(cmdName));
  if(data!=NULL){
    ret=data->kgain;
  }
  return ret;
}
/**
 * @brief ServoCmd::readCommand
 * @param cmdReadName
 * @return "NULL" error
 */
QString ServoCmd::readCommand(const QString cmdReadName)
{
  QString retString="NULL";
//  QString name=cmdReadName;
//  QString cmdName;
//  QTreeWidgetItemIterator it(m_cmdTree);
//  QTreeWidgetItem *itemTemp;
  int getIndex=0;
  double kgain;
  int id;
  ushort cmd;
  ushort length;
  QString dataType;
  GENERALFUNCTION funcRead;
  double ret;
  //获取cmd相关信息
  const CmdTreeData *data;
  if(!m_cmdTreeDataMaps.contains(cmdReadName))//如果cmdtree中没有这个名字，则返回 0
    return retString;
  data=&(m_cmdTreeDataMaps.value(cmdReadName));
  id=data->id;
  getIndex=data->getIndex;
  kgain=data->kgain;
  cmd=data->cmd;
  length=data->length;
  dataType=data->type;

  funcRead.data=new short[length];
  if(id!=-1)
    funcRead.data[3]=id;
  funcRead.cmd=cmd;
  funcRead.mode=1;//1为读 0：写
  funcRead.dataLenth=length;
  GTSD_CMD_ProcessorGeneralFunc(m_axisIndex,&funcRead,m_comType,(int16)m_comRnStation);
  if(dataType.contains("32"))//32位
  {
    if(dataType.contains("U"))
    {
      Uint32 value;
      Uint32 temp;
      temp=funcRead.data[getIndex+1];//高位在前
      temp=temp&0x0000ffff;
      temp=(temp<<16)&0xffff0000;
      value=temp+(funcRead.data[getIndex]&0x0000ffff);
      ret=value+0.0;
    }
    else
    {
      int32 value;
      int32 temp;
      temp=funcRead.data[getIndex+1];
      temp=temp&0x0000ffff;
      temp=(temp<<16)&0xffff0000;
      value=temp+(funcRead.data[getIndex]&0x0000ffff);
      ret=value+0.0;
    }
  }
  else//16位
  {
    if(dataType.contains("U"))
    {
      Uint16 value;
      value=funcRead.data[getIndex];
      ret=value+0.0;
    }
    else
    {
      int16 value;
      value=funcRead.data[getIndex];
      ret=value+0.0;
    }
  }
  ret=ret/kgain;
  retString=QString::number(ret,'f',3);
  delete []funcRead.data;

  /*while(*it)
  {
    itemTemp=(*it);
    cmdName=itemTemp->text(COL_CMD_FUNC_NAME);
    if(cmdName==name)
    {
      id=itemTemp->text(COL_CMD_FUNC_ID).toInt();
      getIndex=itemTemp->text(COL_CMD_FUNC_GETINDEX).toInt();
      kgain=itemTemp->text(COL_CMD_FUNC_KGAIN).toDouble();
      cmd=itemTemp->text(COL_CMD_FUNC_CMD).toUShort();
      length=itemTemp->text(COL_CMD_FUNC_LENGTH).toUShort();
      dataType=itemTemp->text(COL_CMD_FUNC_TYPE);
      funcRead.data=new short[length];
      if(id!=-1)
        funcRead.data[3]=id;
      funcRead.cmd=cmd;
      funcRead.mode=1;//1为读 0：写
      funcRead.dataLenth=length;
      GTSD_CMD_ProcessorGeneralFunc(m_axisIndex,&funcRead,m_comType);
      if(dataType.contains("32"))//32位
      {
        if(dataType.contains("U"))
        {
          Uint32 value;
          Uint32 temp;
          temp=funcRead.data[getIndex+1];
          temp=temp&0x0000ffff;
          temp=(temp<<16)&0xffff0000;
          value=temp+(funcRead.data[getIndex]&0x0000ffff);
          ret=value+0.0;
        }
        else
        {
          int32 value;
          int32 temp;
          temp=funcRead.data[getIndex+1];
          temp=temp&0x0000ffff;
          temp=(temp<<16)&0xffff0000;
          value=temp+(funcRead.data[getIndex]&0x0000ffff);
          ret=value+0.0;
        }
      }
      else//16位
      {
        if(dataType.contains("U"))
        {
          Uint16 value;
          value=funcRead.data[getIndex];
          ret=value+0.0;
        }
        else
        {
          int16 value;
          value=funcRead.data[getIndex];
          ret=value+0.0;
        }
      }
      ret=ret/kgain;
      retString=QString::number(ret,'f',3);
      delete []funcRead.data;

      break;
    }
    it++;
  }*/
  return retString;
}

/**
 * @brief ServoCmd::writeCommand
 * @param cmdWriteName
 * @param value
 * @return error:"NULL"
 */
QString ServoCmd::writeCommand(const QString cmdWriteName, double value)
{
  QString retString="NULL";
//  QString name=cmdWriteName;
//  QString cmdName;
//  QTreeWidgetItemIterator it(m_cmdTree);
//  QTreeWidgetItem *itemTemp;
  int setIndex=0;
  double kgain;
  int id;
  ushort cmd;
  ushort length;
  QString dataType;
  GENERALFUNCTION funcWrite;
  double setValue=value;

  //获取cmd相关信息
  const CmdTreeData *data;
  if(!m_cmdTreeDataMaps.contains(cmdWriteName))
    return retString;
  data=&(m_cmdTreeDataMaps.value(cmdWriteName));
  id=data->id;
  setIndex=data->setIndex;
  kgain=data->kgain;
  cmd=data->cmd;
  length=data->length;
  dataType=data->type;

  funcWrite.data=new short[length];
  if(id!=-1)
    funcWrite.data[3]=id;
  funcWrite.cmd=cmd;
  funcWrite.mode=0;//1为读 0：写
  funcWrite.dataLenth=length;

  if(dataType.contains("32"))
  {
    if(dataType.contains("U"))
    {
      setValue=setValue*kgain;

      Uint32 value;
      value=(Uint32)setValue;
      funcWrite.data[setIndex]=value&0x0000ffff;
      funcWrite.data[setIndex+1]=value&0xffff0000;
    }
    else
    {
      setValue=setValue*kgain;
      int32 value;
      value=(int32)setValue;
      funcWrite.data[setIndex]=value&0x0000ffff;
      funcWrite.data[setIndex+1]=value&0xffff0000;
    }
  }
  else//16位
  {
    if(dataType.contains("U"))
    {
      setValue=setValue*kgain;
      Uint16 value;
      value=(Uint16)setValue;
      funcWrite.data[setIndex]=value;
    }
    else
    {
      setValue=setValue*kgain;

      int16 value;
      value=(int16)setValue;
      funcWrite.data[setIndex]=value;
    }
  }

  GTSD_CMD_ProcessorGeneralFunc(m_axisIndex,&funcWrite,m_comType,(int16)m_comRnStation);
  delete []funcWrite.data;

  retString=readCommand(cmdWriteName);

  /*while(*it)
  {
    itemTemp=(*it);
    cmdName=itemTemp->text(COL_CMD_FUNC_NAME);
    if(cmdName==name)
    {
      id=itemTemp->text(COL_CMD_FUNC_ID).toInt();
      setIndex=itemTemp->text(COL_CMD_FUNC_SETINDEX).toInt();
      kgain=itemTemp->text(COL_CMD_FUNC_KGAIN).toDouble();
      cmd=itemTemp->text(COL_CMD_FUNC_CMD).toUShort();
      length=itemTemp->text(COL_CMD_FUNC_LENGTH).toUShort();
      dataType=itemTemp->text(COL_CMD_FUNC_TYPE);

      funcWrite.data=new short[length];
      if(id!=-1)
        funcWrite.data[3]=id;
      funcWrite.cmd=cmd;
      funcWrite.mode=0;
      funcWrite.dataLenth=length;

      if(dataType.contains("32"))
      {
        if(dataType.contains("U"))
        {
          setValue=setValue*kgain;

          Uint32 value;
          value=(Uint32)setValue;
          funcWrite.data[setIndex]=value&0x0000ffff;
          funcWrite.data[setIndex+1]=value&0xffff0000;
        }
        else
        {
          setValue=setValue*kgain;
          int32 value;
          value=(int32)setValue;
          funcWrite.data[setIndex]=value&0x0000ffff;
          funcWrite.data[setIndex+1]=value&0xffff0000;
        }
      }
      else//16位
      {
        if(dataType.contains("U"))
        {
          setValue=setValue*kgain;
          Uint16 value;
          value=(Uint16)setValue;
          funcWrite.data[setIndex]=value;
        }
        else
        {
          setValue=setValue*kgain;

          int16 value;
          value=(int16)setValue;
          funcWrite.data[setIndex]=value;
        }
      }

      GTSD_CMD_ProcessorGeneralFunc(m_axisIndex,&funcWrite,m_comType);
      delete []funcWrite.data;

      retString=readCommand(cmdWriteName);

      break;
    }
    it++;
  }*/
  return retString;
}

bool ServoCmd::setServoTaskMode(int mode)
{
  return GTSD_CMD_SetServoTaskMode(m_axisIndex,mode,m_comType,(int16)m_comRnStation);
}
bool ServoCmd::checkServoIsReady()
{
  SERVO_STATE state;
  GTSD_CMD_GetServoState(m_axisIndex,&state,m_comType,(int16)m_comRnStation);
  return state.serv_ready;
}
bool ServoCmd::setServoOn(bool on)
{
  short ret=0;
  if(on)
    ret=GTSD_CMD_SetServoOn(m_axisIndex,m_comType,(int16)m_comRnStation);
  else
    ret=GTSD_CMD_SetServoOff(m_axisIndex,m_comType,(int16)m_comRnStation);
  return ret;
}
int ServoCmd::setPosAdjRef(double value)
{
  return GTSD_CMD_SetPosAdjRef(m_axisIndex,value,m_comType,(int16)m_comRnStation);
}
int ServoCmd::setSpeedRef(double value)
{
  return GTSD_CMD_SetSpdRef(m_axisIndex,value,m_comType,(int16)m_comRnStation);
}

int ServoCmd::connectServo(bool connected)
{
  if(connected)
    return GTSD_CMD_Open(connectProgressCallBack,(void*) 0,m_comType);
  else
    return GTSD_CMD_Close(m_comType);
}

int ServoCmd::resetDsp(int axis)
{
  return GTSD_CMD_ResetSystem(axis,m_comType,(int16)m_comRnStation);
}
bool ServoCmd::checkRestDspFinish(int axis)
{
  bool flagFinish=false;
  GTSD_CMD_CheckResetFinish(axis,flagFinish,m_comType,(int16)m_comRnStation);
  return flagFinish;
}

void ServoCmd::delayMs(int ms)
{
  QTime dieTime = QTime::currentTime().addMSecs(ms);
  while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
quint16 ServoCmd::readDeviceVersion()
{
  quint16 version;
  GTSD_CMD_ReadProcessorVersion(0,version,m_comType,(int16)m_comRnStation);
  return version;
}

void ServoCmd::writeFunctionTreeItem2Flash(TreeItem *item)
{
  TreeItem *childItem;
  for(int i=0;i<item->childCount();i++)
  {
    childItem=item->child(i);
    if(childItem->childCount()>0)
    {
      writeFunctionTreeItem2Flash(item->child(i));
    }
    else//叶子节点
    {
      //-------开始读---------
      double kgain=0;
      unsigned short offsetAddr=0;
      double setValue=0;
      QString name=childItem->data(COL_FUNC_NAME).toString();
      QString valueType=childItem->data(COL_FUNC_TYPE).toString();
      QString offset=childItem->data(COL_FUNC_OFFSET).toString();
      setValue=childItem->data(COL_FUNC_VALUE).toDouble();
      qDebug()<<tr("write data to servo flash: %1").arg(name);

      //---------------从ExtensionTree中获得Flash的偏移地址--------------
      QTreeWidget *funcExtion=m_extendTree;
      QTreeWidgetItem *aitem;
      QTreeWidgetItem *itemChild;

      for(int i=0;i<funcExtion->topLevelItemCount();i++)
      {
        aitem=funcExtion->topLevelItem(i);
        if("offsetAddress"==aitem->text(COL_FUNC_EXTENSION_NAME))
        {
          itemChild=aitem->child(m_axisIndex);
          offsetAddr=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
          break;
        }
      }
      offsetAddr+=offset.toUInt();

      //--------------------找增益-------------
      const CmdTreeData *cmdTree;
      cmdTree=&(m_cmdTreeDataMaps.value(name));
      kgain=cmdTree->kgain;

      setValue=setValue*kgain;
      if(valueType.contains("64"))
      {
        GTSD_CMD_Fram_Write64BitByAdr(m_axisIndex,(int16)offsetAddr,(int64)setValue,(int16)m_comType,(int16)m_comRnStation);
      }
      else if(valueType.contains("32"))
      {
        GTSD_CMD_Fram_Write32BitByAdr(m_axisIndex,(int16)offsetAddr,(int32)setValue,(int16)m_comType,(int16)m_comRnStation);
      }
      else
      {
        GTSD_CMD_Fram_Write16BitByAdr(m_axisIndex,(int16)offsetAddr,(int16)setValue,(int16)m_comType,(int16)m_comRnStation);
      }
    }
  }
}

void ServoCmd::updateFunctionTreeItemFromFlash(TreeItem *item)
{
  TreeItem *childItem;
  for(int i=0;i<item->childCount();i++)
  {
    childItem=item->child(i);
    if(childItem->childCount()>0)
    {
      updateFunctionTreeItemFromFlash(item->child(i));
    }
    else//叶子节点
    {
      //-------开始读---------
      double kgain=0;
      unsigned short offsetAddr=0;
      double retValue=0;
      int16 value16 = 0;
      int32 value32 = 0;
      int64 value64 = 0;

      QString name=childItem->data(COL_FUNC_NAME).toString();
      QString valueType=childItem->data(COL_FUNC_TYPE).toString();
      QString offset=childItem->data(COL_FUNC_OFFSET).toString();

      qDebug()<<tr("read data from servo flash: %1").arg(name);

      //---------------从ExtensionTree中获得Flash的偏移地址--------------
      QTreeWidget *funcExtion=m_extendTree;
      QTreeWidgetItem *aitem;
      QTreeWidgetItem *itemChild;

      for(int i=0;i<funcExtion->topLevelItemCount();i++)
      {
        aitem=funcExtion->topLevelItem(i);
        if("offsetAddress"==aitem->text(COL_FUNC_EXTENSION_NAME))
        {
          itemChild=aitem->child(m_axisIndex);
          offsetAddr=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
          break;
        }
      }
      offsetAddr+=offset.toUInt();

      //--------------------找增益-------------
      const CmdTreeData *cmdTree=NULL;
      cmdTree=&(m_cmdTreeDataMaps.value(name));
      if(cmdTree!=NULL)
        kgain=cmdTree->kgain;

      if(valueType.contains("64"))
      {
        GTSD_CMD_Fram_Read64BitByAdr(m_axisIndex, (int16)offsetAddr, &value64, (int16)m_comType,(int16)m_comRnStation);
        retValue=value64+0.0;
        retValue=retValue/kgain;
      }
      else if(valueType.contains("32"))
      {
        GTSD_CMD_Fram_Read32BitByAdr(m_axisIndex,(int16)offsetAddr,&value32,(int16)m_comType,(int16)m_comRnStation);
        retValue=value32+0.0;
        retValue=retValue/kgain;
      }
      else
      {
        GTSD_CMD_Fram_Read16BitByAdr(m_axisIndex,(int16)offsetAddr,&value16,(int16)m_comType,(int16)m_comRnStation);
        retValue=value16+0.0;
        retValue=retValue/kgain;
      }
      qDebug()<<tr("Flash/kgain value:%1").arg(retValue);
      childItem->setData(COL_FUNC_VALUE,retValue);
    }
  }
}

void ServoCmd::fillCmdTreeDataMaps(QTreeWidgetItem *item)
{
  QTreeWidgetItem *childItem;
  for(int i=0;i<item->childCount();i++)
  {
    childItem=item->child(i);
    if(childItem->childCount()>0)
    {
      fillCmdTreeDataMaps(childItem);
    }
    else
    {
      QString name=childItem->text(COL_CMD_FUNC_NAME);
      QString type=childItem->text(COL_CMD_FUNC_TYPE);
      QString unit=childItem->text(COL_CMD_FUNC_UNIT);
      int cmd=childItem->text(COL_CMD_FUNC_CMD).toInt();
      int length=childItem->text(COL_CMD_FUNC_LENGTH).toInt();
      int setIndex=childItem->text(COL_CMD_FUNC_SETINDEX).toInt();
      int getIndex=childItem->text(COL_CMD_FUNC_GETINDEX).toInt();
      double kgain=childItem->text(COL_CMD_FUNC_KGAIN).toDouble();
      int id=childItem->text(COL_CMD_FUNC_ID).toInt();
      CmdTreeData treeData;
      treeData.type=type;
      treeData.unit=unit;
      treeData.cmd=cmd;
      treeData.length=length;
      treeData.setIndex=setIndex;
      treeData.getIndex=getIndex;
      treeData.kgain=kgain;
      treeData.id=id;
      m_cmdTreeDataMaps.insert(name,treeData);
    }
  }
}
void ServoCmd::connectProgressCallBack(void *arg, qint16 *value)
{
  Q_UNUSED(arg);
  Q_UNUSED(value);
}
