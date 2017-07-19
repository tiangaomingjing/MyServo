#include "servogeneralcmd.h"
#include <QTreeWidgetItem>
#include <QTreeWidget>

#include "xmlbuilder.h"
#include "ServoDriverComDll.h"

#define DEBUG_TEST 0

ServoGeneralCmd::ServoGeneralCmd(QTreeWidget *cmdTree,qint16 comType,qint16 rnStation,QObject *parent):
  QObject(parent),
  m_comType(comType),
  m_comRnStation(rnStation),
  m_cmdTree(cmdTree)
{
  if(m_cmdTreeDataMaps.count()>0)
    m_cmdTreeDataMaps.clear();
  QTreeWidgetItem *topItem;
  for(int i=0;i<cmdTree->topLevelItemCount();i++)
  {
    topItem=cmdTree->topLevelItem(i);
    fillCmdTreeDataMaps(topItem);
  }
#if DEBUG_TEST
  QMapIterator<QString,CmdTreeData>i(m_cmdTreeDataMaps);
  const CmdTreeData *data;
  while(i.hasNext()){
    i.next();
    data=&(i.value());
    qDebug()<<i.key()<<" "<<data->type<<data->unit<<data->cmd<<data->length<<data->setIndex<<data->getIndex<<data->kgain<<data->id;
  }
#endif
}
ServoGeneralCmd::~ServoGeneralCmd()
{
  m_cmdTreeDataMaps.clear();
}

double ServoGeneralCmd::read(const QString &cmdReadName, qint16 axisIndex)
{
  double ret=-1;
  int getIndex=0;
  double kgain;
  int id;
  ushort cmd;
  ushort length;
  QString dataType;
  GENERALFUNCTION funcRead;

  //获取cmd相关信息
  const CmdTreeData *data;
  if(!m_cmdTreeDataMaps.contains(cmdReadName))//如果cmdtree中没有这个名字，则返回 0
    return ret;
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
  funcRead.mode=1;//1:为读    0：写
  funcRead.dataLenth=length;
  if(0!=GTSD_CMD_ProcessorGeneralFunc(axisIndex,&funcRead,m_comType,m_comRnStation))
  {
    if(0!=GTSD_CMD_ProcessorGeneralFunc(axisIndex,&funcRead,m_comType,m_comRnStation))
      return -1;
  }

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
  delete []funcRead.data;
  return ret;
}
double ServoGeneralCmd::write(const QString &cmdWriteName, double value, qint16 axisIndex)
{
  double ret=0;
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
    return -1;
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

  setValue=setValue*kgain;

  if(dataType.contains("32"))
  {
    if(dataType.contains("U"))
    {
      Uint32 value;
      value=(Uint32)setValue;
      funcWrite.data[setIndex]=value&0x0000ffff;
      funcWrite.data[setIndex+1]=value&0xffff0000;
    }
    else
    {
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
      Uint16 value;
      value=(Uint16)setValue;
      funcWrite.data[setIndex]=value;
    }
    else
    {
      int16 value;
      value=(int16)setValue;
      funcWrite.data[setIndex]=value;
    }
  }

  if(0!=GTSD_CMD_ProcessorGeneralFunc(axisIndex,&funcWrite,m_comType,(int16)m_comRnStation))
    GTSD_CMD_ProcessorGeneralFunc(axisIndex,&funcWrite,m_comType,(int16)m_comRnStation);
  delete []funcWrite.data;

  ret=read(cmdWriteName,axisIndex);
  return ret;
}

void ServoGeneralCmd::fillCmdTreeDataMaps(QTreeWidgetItem *item)
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

qint16 ServoGeneralCmd::comRnStation() const
{
    return m_comRnStation;
}

qint16 ServoGeneralCmd::comType() const
{
    return m_comType;
}

void ServoGeneralCmd::setCmdTree(QTreeWidget *cmdTree)
{
  m_cmdTree = cmdTree;
  if(m_cmdTreeDataMaps.count()>0)
    m_cmdTreeDataMaps.clear();
  QTreeWidgetItem *topItem;
  for(int i=0;i<cmdTree->topLevelItemCount();i++)
  {
    topItem=cmdTree->topLevelItem(i);
    fillCmdTreeDataMaps(topItem);
  }
}

void ServoGeneralCmd::setComRnStation(const qint16 comRnStation)
{
  m_comRnStation = comRnStation;
}

void ServoGeneralCmd::setComType(const qint16 comType)
{
  m_comType = comType;
}
