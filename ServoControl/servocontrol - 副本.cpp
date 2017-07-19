#include "servocontrol.h"
#include "xmlbuilder.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QTreeWidgetItemIterator>
#include <QMessageBox>
#define TEST_DEBUG 0

enum CMD_MODE{
  MODE_WRITE=0,
  MODE_READ=1
};

ServoControl::ServoControl(QObject *parent) : QObject(parent)
{

}

ServoControl::~ServoControl()
{

}

bool ServoControl::checkedCmdReturnValue(COM_ERROR retvalue)
{
  bool result=true;
  switch (retvalue)
  {
  case COM_OK:
    result=true;
    break;
  case COM_ARM_OUT_TIME:
    result=false;
//    QMessageBox::information(0,tr("connect"),tr("error:%1\nARM out of time").arg(retvalue));
    break;
  case COM_NET_ERROR:
    result=false;
//    QMessageBox::information(0,tr("connect"),tr("com connect error:%1\ncom offline ").arg(retvalue));
    break;
  case COM_NOT_FINISH:
    result=true;
    break;
  default:
    result=true;
    break;
  }
  return result;
}
//单个轴的树读进来
void ServoControl::updateFlashTreeWidget(QTreeWidget *tree, int axisIndex, COM_TYPE comtype,short comStation)
{
  QTreeWidgetItemIterator it(tree);
  QTreeWidgetItem *item;
  int16 value16 = 0;
  int32 value32 = 0;
  int64 value64 = 0;
  int16 offsetValue = 0;
  QString itemType;
  COM_ERROR result;
  while (*it)
  {
    item=(*it);
    offsetValue = item->text(COL_ADDRESS).toInt();
    if (offsetValue != -1)
    {
      itemType = item->text(COL_TYPE);
      if (itemType.contains("64"))
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Read64BitByAdr(axisIndex, (int16)offsetValue, &value64, (int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
          item->setText(COL_VALUE, QString::number(value64));
      }
      else if (itemType.contains("32"))
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Read32BitByAdr(axisIndex, (int16)offsetValue, &value32, (int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
          item->setText(COL_VALUE, QString::number(value32));
      }
      else
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Read16BitByAdr(axisIndex, (int16)offsetValue, &value16,(int16) comtype,comStation);
        if(checkedCmdReturnValue(result))
          item->setText(COL_VALUE, QString::number(value16));
      }
    }
    it++;
  }
}


/**********************************************************************
 * @parameter treeSrc所有轴的树读进来
 *
*/
void ServoControl::updateAllFlashTreeWidget(QTreeWidget *treeSrc, COM_TYPE comtype,short station)
{
  QList<QTreeWidget *>treeList;
  QTreeWidget *tree;
  QTreeWidgetItem *treeItem;
  for(int i=0;i<treeSrc->topLevelItemCount();i++)
  {
    tree=new QTreeWidget();
    treeItem=treeSrc->topLevelItem(i)->clone();
    tree->addTopLevelItem(treeItem);
    updateFlashTreeWidget(tree, i, comtype,station);
    treeList.append(tree);
  }
  treeSrc->clear();
  for(int i=0;i<treeList.count();i++)
  {
    treeItem=treeList.at(i)->topLevelItem(0)->clone();
    treeSrc->addTopLevelItem(treeItem);
    treeList.at(i)->clear();
  }
  treeList.clear();
}

void ServoControl::updateFlashTreeWidgetByExpanded(QTreeWidgetItem *item ,int axisIndex, COM_TYPE comtype,short station)
{
  int16 value16 = 0;
  int32 value32 = 0;
  int64 value64 = 0;
  int16 offsetValue = 0;
  QString itemType;
  COM_ERROR result;
  QTreeWidgetItem *itemChild;

  qDebug() << QString("item :%2 has child :%1").arg(item->childCount()).arg(item->text(COL_NAME));
  for (int i = 0; i < item->childCount();i++)
  {
    qDebug() << item->child(i)->text(0) << ":" << item->child(i)->text(3);
    itemChild=item->child(i);
    offsetValue = itemChild->text(COL_ADDRESS).toInt();
    if (offsetValue != -1)
    {
      itemType = itemChild->text(COL_TYPE);
      if (itemType.contains("64"))
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Read64BitByAdr(axisIndex, (int16)offsetValue, &value64, (int16)comtype,station);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            itemChild->setText(COL_VALUE, QString::number((Uint64)value64));
          else
            itemChild->setText(COL_VALUE, QString::number(value64));
        }
      }
      else if (itemType.contains("32"))
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Read32BitByAdr(axisIndex, (int16)offsetValue, &value32, (int16)comtype,station);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            itemChild->setText(COL_VALUE, QString::number((Uint32)value32));
          else
            itemChild->setText(COL_VALUE, QString::number(value32));
        }
      }
      else
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Read16BitByAdr(axisIndex, (int16)offsetValue, &value16,(int16) comtype,station);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            itemChild->setText(COL_VALUE, QString::number((Uint16)value16));
          else
            itemChild->setText(COL_VALUE, QString::number(value16));
        }
      }
    }
  }
  item->treeWidget()->resizeColumnToContents(COL_NAME);
  item->treeWidget()->resizeColumnToContents(COL_VALUE);
  item->treeWidget()->resizeColumnToContents(COL_TYPE);

}

void ServoControl::updateRamTreeWidgetByExpanded(QTreeWidgetItem *item, int axisIndex, const QMap<QString,QVariant>&map,COM_TYPE comtype,short station)
{
  int16 value16 = 0;
  int32 value32 = 0;
  int64 value64 = 0;
  int16 offsetValue = 0;
  QString itemType;
  QString itemName;
  int arg=0;
  COM_ERROR result;
  QTreeWidgetItem *itemChild;

  qDebug() << QString("item :%1 has child :%2").arg(item->text(COL_NAME)).arg(item->childCount());
  for (int i = 0; i < item->childCount();i++)
  {
    qDebug() << "item:"<<item->child(i)->text(0) << "..." <<"offset:"<< item->child(i)->text(3);
    itemChild=item->child(i);
    offsetValue = itemChild->text(COL_ADDRESS).toInt();
    if (offsetValue != -1)
    {
      itemName=itemChild->text(COL_NAME);
      itemType = itemChild->text(COL_TYPE);
      QStringList strList=itemName.split(".");
      arg=map.value(strList.at(0)).toInt();

#if TEST_DEBUG
      foreach (QString str, strList) {
        qDebug()<<str;
      }
      qDebug()<<"itemName"<<itemName;
      qDebug()<<"arg:"<<arg;
#endif

      if (itemType.contains("64"))
      {
        result=(COM_ERROR)GTSD_CMD_Read64BitByAdr(axisIndex, (int16)offsetValue, &value64,(void*)(&arg), (int16)comtype,station);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            itemChild->setText(COL_VALUE, QString::number((Uint64)value64));
          else
            itemChild->setText(COL_VALUE, QString::number(value64));
        }
      }
      else if (itemType.contains("32"))
      {
        result=(COM_ERROR)GTSD_CMD_Read32BitByAdr(axisIndex, (int16)offsetValue, &value32, (void*)(&arg),(int16)comtype,station);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            itemChild->setText(COL_VALUE, QString::number((Uint32)value32));
          else
            itemChild->setText(COL_VALUE, QString::number(value32));
        }
      }
      else
      {
        result=(COM_ERROR)GTSD_CMD_Read16BitByAdr(axisIndex, (int16)offsetValue, &value16,(void*)(&arg),(int16) comtype,station);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            itemChild->setText(COL_VALUE, QString::number((Uint16)value16));
          else
            itemChild->setText(COL_VALUE, QString::number(value16));
        }
      }
    }
  }
  item->treeWidget()->resizeColumnToContents(COL_NAME);
  item->treeWidget()->resizeColumnToContents(COL_VALUE);
  item->treeWidget()->resizeColumnToContents(COL_TYPE);
}

COM_ERROR ServoControl::updateFunctionValueFromRam(QTreeWidgetItem *item,QTreeWidget *cmdTree, int axisIndex, COM_TYPE comtype,short comStation)
{
  COM_ERROR servoResult=COM_OK;
  QString name=item->text(COL_FUNC_NAME);
  QString cmdName;
  QTreeWidgetItemIterator it(cmdTree);
  QTreeWidgetItem *itemTemp;
  while (*it)
  {
    itemTemp=(*it);
    cmdName=itemTemp->text(COL_CMD_FUNC_NAME);
    if(0==cmdName.compare(name))
    {
      int id;
      int getIndex;
      double kgain;
      ushort cmd;
      ushort length;
      QString type;
      /**填王彬结构体，然后调用它的指令下发
       *
       * cmd 是指其指令号
       * GENERALFUNCTION 中的data[]数组长度为:length
       * data[0],data[1]不用填，他下面已经写了
       * data[3]当xml表中的id不为-1时，data[3]=id
       * GTSD_CMD_ProcessorGeneralFunc(axisIndex,&func,comtype)中func写下去，再由下面修改，其返回结果在func中
       * 返回结果从getIndex位开始获得
       * 读命令返回的最终结果要/kgain,如果是设置写命令要*kgain
      */
      GENERALFUNCTION func;
      COM_ERROR result;
      id=itemTemp->text(COL_CMD_FUNC_ID).toInt();
      getIndex=itemTemp->text(COL_CMD_FUNC_GETINDEX).toInt();
      kgain=itemTemp->text(COL_CMD_FUNC_KGAIN).toDouble();
      cmd=itemTemp->text(COL_CMD_FUNC_CMD).toUShort();
      length=itemTemp->text(COL_CMD_FUNC_LENGTH).toUShort();
      type=itemTemp->text(COL_CMD_FUNC_TYPE);
      func.data=new short[length];
//      qDebug()<<cmdName<<"id"<<id<<"getindex"<<getIndex<<"length"<<length<<"cmd"<<cmd<<"type"<<type;
      if(id!=-1)
      {
        func.data[3]=id;
      }
      func.cmd=cmd;
      func.mode=MODE_READ;
      func.dataLenth=length;
      result=(COM_ERROR)GTSD_CMD_ProcessorGeneralFunc(axisIndex,&func,comtype,comStation);
      servoResult=result;
      double ret;
      if(checkedCmdReturnValue(result))
      {
        if(type.contains("32"))//两个16位,低在前
        {
          if(type.contains("U"))
          {
            Uint32 value;
            Uint32 temp;
            temp=func.data[getIndex+1];
            temp=temp&0x0000ffff;
            temp=(temp<<16)&0xffff0000;
            value=temp+(func.data[getIndex]&0x0000ffff);
            ret=value+0.0;
            ret=ret/kgain;
//            value=value/kgain+0.5;
            item->setText(COL_FUNC_VALUE,QString::number(ret,'f',3));

          }
          else
          {
            int32 value;
            int32 temp;
            temp=func.data[getIndex+1];
            temp=temp&0x0000ffff;
            temp=(temp<<16)&0xffff0000;
            value=temp+(func.data[getIndex]&0x0000ffff);
            ret=value+0.0;
            ret=ret/kgain;
//            value=value/kgain+0.5;
            item->setText(COL_FUNC_VALUE,QString::number(ret,'f',3));
          }
        }
        else//16位
        {
          if(type.contains("U"))
          {
            Uint16 value;
            value=func.data[getIndex];
            ret=value+0.0;
            ret=ret/kgain;
//            value=value/kgain+0.5;
            item->setText(COL_FUNC_VALUE,QString::number(ret,'f',3));
          }
          else
          {
            int16 value;
            value=func.data[getIndex];
            ret=value+0.0;
            ret=ret/kgain;
            item->setText(COL_FUNC_VALUE,QString::number(ret,'f',3));
          }
        }
//        qDebug()<<cmdName<<"value :"<<ret;
        delete []func.data;
        break;
      }
      else
      {
        delete []func.data;
//        QMessageBox::information(0,tr("read ram error"),tr("read ram error\nplease check your servo's status"));
        break;
      }
    }
    it++;
  }
  return servoResult;
}

COM_ERROR ServoControl::updateFunctionValueFromRamAllTree(QTreeWidget *tree,QTreeWidget *cmdTree, int axisIndex, COM_TYPE comtype,short comStation)
{
  QTreeWidgetItemIterator it(tree);
  QTreeWidgetItem *item;
  COM_ERROR ret=COM_OK;
  while (*it)
  {
    item=(*it);
    ret=updateFunctionValueFromRam(item,cmdTree, axisIndex, comtype,comStation);
    if(ret==COM_NET_ERROR||ret==COM_ARM_OUT_TIME) //更新没成功
      break;
    it++;
  }
  return ret;
}

void ServoControl::updateFunctionValueFromFlash(QTreeWidgetItem *item, QTreeWidget *cmdTree, quint16 offsetAddr, int axisIndex, COM_TYPE comtype,short comStation)
{
  int16 value16 = 0;
  int32 value32 = 0;
  int64 value64 = 0;
  double ret;
  QString name=item->text(COL_FUNC_NAME);
  Uint16 offset=item->text(COL_FUNC_OFFSET).toUInt()+offsetAddr;
  QString type=item->text(COL_FUNC_TYPE);
  COM_ERROR result;
  QString cmdName;
  QTreeWidgetItemIterator it(cmdTree);
  QTreeWidgetItem *itemTemp;
  while (*it)
  {
    itemTemp=(*it);
    cmdName=itemTemp->text(COL_CMD_FUNC_NAME);
    if(0==cmdName.compare(name))
    {
      double kgain;
      kgain=itemTemp->text(COL_CMD_FUNC_KGAIN).toDouble();
      if(type.contains("64"))
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Read64BitByAdr(axisIndex, (int16)offset, &value64, (int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          ret=value64+0.0;
          ret=ret/kgain;
          item->setText(COL_VALUE, QString::number(ret,'f',3));
        }
      }
      else if(type.contains("32"))
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Read32BitByAdr(axisIndex, (int16)offset, &value32, (int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          ret=value32+0.0;
          ret=ret/kgain;
          item->setText(COL_VALUE, QString::number(ret,'f',3));
        }
      }
      else
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Read16BitByAdr(axisIndex, (int16)offset, &value16, (int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          ret=value16+0.0;
          ret=ret/kgain;
          item->setText(COL_VALUE, QString::number(ret,'f',3));
        }
      }
      break;
    }
    it++;
  }
}

void ServoControl::updateFunctionValueFromFlashAllTree(QTreeWidget *tree, QTreeWidget *cmdTree, quint16 offsetAddr, int axisIndex, COM_TYPE comtype,short comStation)
{
  QTreeWidgetItemIterator it(tree);
  QTreeWidgetItem *item;
  while (*it)
  {
    item=(*it);
    updateFunctionValueFromFlash(item,cmdTree, offsetAddr,axisIndex, comtype,comStation);
    it++;
  }
}

//写单个树结点
void ServoControl::writeServoFlash(QTreeWidgetItem *item, int axisIndex, COM_TYPE comtype,short comStation)
{
  int16 value16 = 0;
  int32 value32 = 0;
  int64 value64 = 0;
  int64 value = 0;
  int16 offsetValue = 0;
  QString itemType;
  int16 result=0;//0代表成功返回

  offsetValue = item->text(COL_ADDRESS).toInt();
  if (offsetValue != -1)
  {
    value=item->text(COL_VALUE).toLongLong();
    itemType = item->text(COL_TYPE);
    if (itemType.contains("64"))
    {
      result=GTSD_CMD_Fram_Write64BitByAdr(axisIndex,(int16)offsetValue,(int64)value,(int16)comtype,comStation);
      if(result!=0)
        result=GTSD_CMD_Fram_Write64BitByAdr(axisIndex,(int16)offsetValue,(int64)value,(int16)comtype,comStation);
      if(result==0)
      {
//        qDebug()<<"write value to servo flash";
        result=GTSD_CMD_Fram_Read64BitByAdr(axisIndex, (int16)offsetValue, &value64, (int16)comtype,comStation);
        if(result)
        {
          if(itemType.contains("U"))
            item->setText(COL_VALUE, QString::number((Uint64)value64));
          else
            item->setText(COL_VALUE, QString::number(value64));
        }
      }
    }
    else if (itemType.contains("32"))
    {
      result=(COM_ERROR)GTSD_CMD_Fram_Write32BitByAdr(axisIndex,(int16)offsetValue,(int32)value,(int16)comtype,comStation);
      if(checkedCmdReturnValue(result))
      {
//        qDebug()<<"write value to servo flash";
        result=(COM_ERROR)GTSD_CMD_Fram_Read32BitByAdr(axisIndex, (int16)offsetValue, &value32, (int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            item->setText(COL_VALUE, QString::number((Uint32)value32));
          else
            item->setText(COL_VALUE, QString::number(value32));
        }
      }
    }
    else
    {
      result=(COM_ERROR)GTSD_CMD_Fram_Write16BitByAdr(axisIndex,(int16)offsetValue,(int16)value,(int16)comtype,comStation);
      if(checkedCmdReturnValue(result))
      {
//        qDebug()<<"write value to servo flash";
        result=(COM_ERROR)GTSD_CMD_Fram_Read16BitByAdr(axisIndex, (int16)offsetValue, &value16, (int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            item->setText(COL_VALUE, QString::number((Uint16)value16));
          else
            item->setText(COL_VALUE, QString::number(value16));
        }
      }
    }
  }
}

void ServoControl::writeServoRam(QTreeWidgetItem *item, int axisIndex,const QMap<QString,QVariant>&map, COM_TYPE comtype,short comStation)
{
  int16 value16 = 0;
  int32 value32 = 0;
  int64 value64 = 0;
  int64 value = 0;
  int16 offsetValue = 0;
  QString itemType;
  QString itemName;
  int arg=0;
  COM_ERROR result;

  offsetValue = item->text(COL_ADDRESS).toInt();
  if (offsetValue != -1)
  {
    itemName=item->text(COL_NAME);
    itemType = item->text(COL_TYPE);
    QStringList strList=itemName.split(".");
    arg=map.value(strList.at(0)).toInt();
    value=item->text(COL_VALUE).toLongLong();

#if TEST_DEBUG
    foreach (QString str, strList) {
      qDebug()<<str;
    }
    qDebug()<<"itemName"<<itemName;
    qDebug()<<"arg:"<<arg;
#endif


    if (itemType.contains("64"))
    {
      result=(COM_ERROR)GTSD_CMD_Write64BitByAdr(axisIndex,(int16)offsetValue,(int64)value,(void*)&arg,(int16)comtype,comStation);
      if(checkedCmdReturnValue(result))
      {
        result=(COM_ERROR)GTSD_CMD_Read64BitByAdr(axisIndex, (int16)offsetValue, &value64,(void*)&arg, (int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            item->setText(COL_VALUE, QString::number((Uint64)value64));
          else
            item->setText(COL_VALUE, QString::number(value64));
        }
        else
          item->setText(COL_VALUE, "0");
      }
      else
        item->setText(COL_VALUE, "0");
    }

    else if (itemType.contains("32"))
    {
      result=(COM_ERROR)GTSD_CMD_Write32BitByAdr(axisIndex,(int16)offsetValue,(int32)value,(void*)&arg,(int16)comtype,comStation);
      if(checkedCmdReturnValue(result))
      {
//        qDebug()<<"write value to servo ram";
        result=(COM_ERROR)GTSD_CMD_Read32BitByAdr(axisIndex, (int16)offsetValue, &value32, (void*)&arg,(int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            item->setText(COL_VALUE, QString::number((Uint32)value32));
          else
            item->setText(COL_VALUE, QString::number(value32));

        }
        else
          item->setText(COL_VALUE, "0");
      }
      else
        item->setText(COL_VALUE, "0");
    }
    else
    {
      result=(COM_ERROR)GTSD_CMD_Write16BitByAdr(axisIndex,(int16)offsetValue,(int16)value,(void*)&arg,(int16)comtype,comStation);
      if(checkedCmdReturnValue(result))
      {
//        qDebug()<<"write value to servo ram";
        result=(COM_ERROR)GTSD_CMD_Read16BitByAdr(axisIndex, (int16)offsetValue, &value16, (void*)&arg,(int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          if(itemType.contains("U"))
            item->setText(COL_VALUE, QString::number((Uint16)value16));
          else
            item->setText(COL_VALUE, QString::number(value16));
        }
        else
          item->setText(COL_VALUE, "0");
      }
      else
        item->setText(COL_VALUE, "0");
    }
  }
}

void ServoControl::writeServoFlashByAxisTree(QTreeWidget *tree, int axisIndex, COM_TYPE comtype,short comStation)
{
  QTreeWidgetItemIterator it(tree);
  QTreeWidgetItem *item;
  while (*it)
  {
    item=(*it);
    writeServoFlash(item, axisIndex, comtype,comStation);
    it++;
  }
}

void ServoControl::writeServoFlashByAllAxisTree(QTreeWidget *alltree, COM_TYPE comtype,short comStation)
{
  QTreeWidget *tree;
  QTreeWidgetItem *treeItem;
  for(int i=0;i<alltree->topLevelItemCount();i++)
  {
    tree=new QTreeWidget();
    treeItem=alltree->topLevelItem(i)->clone();
    tree->addTopLevelItem(treeItem);
    writeServoFlashByAxisTree(tree,i,comtype,comStation);
    tree->clear();
    delete tree;
    tree=NULL;
  }
}
void ServoControl::writeFunctionValue2Ram(QTreeWidgetItem *item, QTreeWidget *cmdTree, int axisIndex, COM_TYPE comtype,short comStation)
{
  QString name=item->text(COL_FUNC_NAME);
  QString cmdName;
  QTreeWidgetItemIterator it(cmdTree);
  QTreeWidgetItem *itemTemp;
  while (*it)
  {
    itemTemp=(*it);
    cmdName=itemTemp->text(COL_CMD_FUNC_NAME);
    if(0==cmdName.compare(name))
    {
      int id;
      int setIndex;
      double kgain;
      ushort cmd;
      ushort length;
      QString type;
      /**填王彬结构体，然后调用它的指令下发
       *
       * cmd 是指其指令号
       * GENERALFUNCTION 中的data[]数组长度为:length
       * data[0],data[1]不用填，他下面已经写了
       * data[3]当xml表中的id不为-1时，data[3]=id
       * GTSD_CMD_ProcessorGeneralFunc(axisIndex,&func,comtype)中func写下去，再由下面修改，其返回结果在func中
       * 返回结果从getIndex位开始获得
       * 读命令返回的最终结果要/kgain,如果是设置写命令要*kgain
      */
      GENERALFUNCTION func;
      COM_ERROR result;
      id=itemTemp->text(COL_CMD_FUNC_ID).toInt();
      setIndex=itemTemp->text(COL_CMD_FUNC_SETINDEX).toInt();
      kgain=itemTemp->text(COL_CMD_FUNC_KGAIN).toDouble();
      cmd=itemTemp->text(COL_CMD_FUNC_CMD).toUShort();
      length=itemTemp->text(COL_CMD_FUNC_LENGTH).toUShort();
      type=itemTemp->text(COL_CMD_FUNC_TYPE);
      func.data=new short[length];
      qDebug()<<cmdName<<"id"<<id<<"setindex"<<setIndex<<"length"<<length<<"cmd"<<cmd<<"type"<<type;
      if(id!=-1)
      {
        func.data[3]=id;
      }
      func.cmd=cmd;
      func.mode=MODE_WRITE;
      func.dataLenth=length;
      double setValue;
      if(type.contains("32"))
      {
        if(type.contains("U"))
        {
          setValue=item->text(COL_FUNC_VALUE).toDouble();
          setValue=setValue*kgain;

          Uint32 value;
//          value=item->text(COL_FUNC_VALUE).toULong();
//          value=value*kgain;
          value=(Uint32)setValue;
          func.data[setIndex]=value&0x0000ffff;
          func.data[setIndex+1]=value&0xffff0000;
        }
        else
        {
          setValue=item->text(COL_FUNC_VALUE).toDouble();
          setValue=setValue*kgain;

          int32 value;
//          value=item->text(COL_FUNC_VALUE).toLong();
//          value=value*kgain;
          value=(int32)setValue;
          func.data[setIndex]=value&0x0000ffff;
          func.data[setIndex+1]=value&0xffff0000;
        }
      }
      else//16位
      {
        if(type.contains("U"))
        {
          setValue=item->text(COL_FUNC_VALUE).toDouble();
          setValue=setValue*kgain;

          Uint16 value;
//          value=item->text(COL_FUNC_VALUE).toUShort();
//          value=value*kgain;
          value=(Uint16)setValue;
          func.data[setIndex]=value;
        }
        else
        {
          setValue=item->text(COL_FUNC_VALUE).toDouble();
          setValue=setValue*kgain;

          int16 value;
//          value=item->text(COL_FUNC_VALUE).toShort();
//          value=value*kgain;
          value=(int16)setValue;
          func.data[setIndex]=value;
        }
      }
      result=(COM_ERROR)GTSD_CMD_ProcessorGeneralFunc(axisIndex,&func,comtype,comStation);
      if(checkedCmdReturnValue(result))
      {
        updateFunctionValueFromRam(item,cmdTree,axisIndex,comtype,comStation);
      }
      else
        QMessageBox::information(0,tr("write error"),tr("write error\nplease check your servo's status"));
      delete []func.data;
      break;
    }
    it++;
  }
}

void ServoControl::writeFunctionValue2RamAllTree(QTreeWidget *tree, QTreeWidget *cmdTree, int axisIndex, COM_TYPE comtype,short comStation)
{
  QTreeWidgetItemIterator it(tree);
  QTreeWidgetItem *item;
  QColor black(Qt::black);
  while (*it)
  {
    item=(*it);
    writeFunctionValue2Ram(item, cmdTree, axisIndex,comtype,comStation);
    item->setTextColor(COL_FUNC_VALUE,black);
    it++;
  }
}
void ServoControl::writeFunctionValue2Flash(QTreeWidgetItem *item,QTreeWidget *cmdTree,quint16 offsetAddr,int axisIndex,COM_TYPE comtype,short comStation)
{
  int16 value16 = 0;
  int32 value32 = 0;
  int64 value64 = 0;
  QString name=item->text(COL_FUNC_NAME);
//  int64 value=item->text(COL_FUNC_VALUE).toLongLong();
  double value=item->text(COL_FUNC_VALUE).toDouble();
  Uint16 offset=item->text(COL_FUNC_OFFSET).toUInt()+offsetAddr;
  QString type=item->text(COL_FUNC_TYPE);
  COM_ERROR result;
  QString cmdName;
  QTreeWidgetItemIterator it(cmdTree);
  QTreeWidgetItem *itemTemp;
  double ret;
  while (*it)
  {
    itemTemp=(*it);
    cmdName=itemTemp->text(COL_CMD_FUNC_NAME);
    if(0==cmdName.compare(name))
    {
      double kgain;
      kgain=itemTemp->text(COL_CMD_FUNC_KGAIN).toDouble();
      value=value*kgain;
      if(type.contains("64"))
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Write64BitByAdr(axisIndex,(int16)offset,(int64)value,(int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          result=(COM_ERROR)GTSD_CMD_Fram_Read64BitByAdr(axisIndex, (int16)offset, &value64, (int16)comtype,comStation);
          if(checkedCmdReturnValue(result))
          {
            ret=value64+0.0;
            ret=ret/kgain;
//            value64=value64/kgain+0.5;
            item->setText(COL_VALUE, QString::number(ret,'f',3));
          }
        }
      }
      else if(type.contains("32"))
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Write32BitByAdr(axisIndex,(int16)offset,(int32)value,(int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          result=(COM_ERROR)GTSD_CMD_Fram_Read32BitByAdr(axisIndex, (int16)offset, &value32, (int16)comtype,comStation);
          if(checkedCmdReturnValue(result))
          {
            ret=value32+0.0;
            ret=ret/kgain;
            item->setText(COL_VALUE, QString::number(ret,'f',3));
          }
        }
      }
      else
      {
        result=(COM_ERROR)GTSD_CMD_Fram_Write16BitByAdr(axisIndex,(int16)offset,(int16)value,(int16)comtype,comStation);
        if(checkedCmdReturnValue(result))
        {
          result=(COM_ERROR)GTSD_CMD_Fram_Read16BitByAdr(axisIndex, (int16)offset, &value16, (int16)comtype,comStation);
          if(checkedCmdReturnValue(result))
          {
            ret=value16+0.0;
            ret=ret/kgain;
            item->setText(COL_VALUE, QString::number(ret,'f',3));
          }
        }
      }
      break;
    }
    it++;
  }
}

void ServoControl::writeFunctionValue2FlashAllTree(QTreeWidget *uiTree,QTreeWidget *cmdTree,quint16 offsetAddr,int axisIndex,COM_TYPE comtype,short comStation)
{
  QTreeWidgetItemIterator it(uiTree);
  QTreeWidgetItem *item;
  QColor black(Qt::black);
  while (*it)
  {
    item=(*it);
    writeFunctionValue2Flash(item, cmdTree, offsetAddr, axisIndex,comtype,comStation);
    item->setTextColor(COL_FUNC_VALUE,black);
    it++;
  }
}

void ServoControl::clearAlarm(short axisIndex, COM_TYPE comType,short comStation)
{
  GTSD_CMD_ClrAlarm(axisIndex, comType,comStation);
  qDebug()<<"clear alarm :"<<axisIndex<<"comtype:"<<comType;
}

qint16 ServoControl::readDeviceVersion(int axis, quint16 &version, qint16 comType,short comStation)
{
  return GTSD_CMD_ReadProcessorVersion(axis,version,comType,comStation);
}
qint16 ServoControl::readDeviceFirmwareVersion(int axis, quint16 &fversion, qint16 comType,short comStation)
{
  return GTSD_CMD_ReadFirmwareVersion(axis,fversion,comType,comStation);
}
bool ServoControl::checkServoIsReady(int axis,int16 com_type , short comStation )
{
  SERVO_STATE state;
  GTSD_CMD_GetServoState(axis,&state,com_type,comStation);
  return state.serv_ready;
}

qint16 ServoControl::servoCurrentUserTaskMode(int axis, int16 com_type, short comStation)
{
  SERVO_MODE mode;
  GTSD_CMD_GetServoTaskMode(axis,&mode,com_type,comStation);
  return mode.usr_mode;
}
