#include "fpgadialogsettingrnnet.h"
#include "ui_fpgadialogsettingrnnet.h"

#include <QTreeWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>
#include <QtMath>
#include <QMessageBox>
#include "ServoDriverComDll.h"
#include "inputvalidator.h"
#include "mainwindow.h"
#include "highlighttext.h"
#define NETNAME0 "CommonNet"

FpgaDialogSettingRnNet::FpgaDialogSettingRnNet(QTreeWidget *tree, MainWindow *mainWindow, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::FpgaDialogSettingRnNet),
  mp_mainWindow(mainWindow)
{
  ui->setupUi(this);
  ui->tableView->hide();
  ui->treeWidget->installEventFilter(this);
  m_hightlightText=new HighlightText(1,this);

  setTreeWidgetStyleSheet();
  QTreeWidgetItem *item;
  for (int i = 0; i < tree->topLevelItemCount(); i++)
  {
    item = tree->topLevelItem(i)->clone();
    ui->treeWidget->addTopLevelItem(item);
  }

  //增加控制映射参数
  //NETNAME0->0
  //DSP0->0
  //DSP1->1...
  m_prmMap.clear();
  m_prmMap.insert(NETNAME0,0);
  for(int i=0;i<mp_mainWindow->getUserConfig()->model.axisCount;i++)
  {
    m_prmMap.insert(tr("DSP%1").arg(QString::number(i)),i);
  }

  connect(ui->treeWidget,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(onExpandRefresh(QTreeWidgetItem*)));
  connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetItemClicked(QTreeWidgetItem*,int)));
  connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onTreeItemChanged(QTreeWidgetItem*,int)));
  connect(ui->treeWidget,SIGNAL(itemSelectionChanged()),this,SLOT(onItemSelectionChanged()));
  connect(ui->rbtn_dec,SIGNAL(clicked(bool)),this,SLOT(onDecHexRadioClicked()));
  connect(ui->rbtn_hex,SIGNAL(clicked(bool)),this,SLOT(onDecHexRadioClicked()));

}

FpgaDialogSettingRnNet::~FpgaDialogSettingRnNet()
{
  delete ui;
}
void FpgaDialogSettingRnNet::onExpandRefresh(QTreeWidgetItem *item)
{
  int16 value16;
  int32 value32;
  static quint8 flagBoolIn = 0;//用于判断是不是第一次更新父类item,以免多次更新取值发生变化
  quint16 offsetValue;
  QString offsetStr;
  QString typeStr;
  QString bitStart;
  int fpgaIndex=0;

  //找到控制变量

  fpgaIndex=getParameterDSPNumberByTopLevelItem(item);

  for (int i = 0; i < item->childCount(); i++)
  {
    offsetStr = item->child(i)->text(TREE_COLUMN_ADDR);
    if (offsetStr != "-1")
    {
      //qDebug() << item->child(i)->text(0);
      //更新数值 :分位控制与没有位控制，判断bitStart是不是-1,如果是发送其父类得到其值，再将值用或与方法写到每一位去
      bitStart = item->child(i)->text(TREE_COLUMN_BITSTART);
      if (bitStart == "-1")//是Bool类型
      {
        QString resultStr;
        quint32 bitStart, bitWidth, opera;
        typeStr = item->text(TREE_COLUMN_TYPE);
        offsetStr = item->text(TREE_COLUMN_ADDR);
        offsetValue = offsetStr.toShort(0, 16);//item的偏移地址
        bitStart = item->child(i)->text(TREE_COLUMN_BITSTART).toUInt();
        bitWidth = item->child(i)->text(TREE_COLUMN_BITWIDTH).toUInt();
        opera = operationValue(bitStart, bitWidth);
        if (typeStr.contains("32"))//item类型是32位
        {
          quint32 temp = 0;
          if (flagBoolIn==0)
          {
            QString str;
            GTSD_CMD_Get32bitFPGAByAddr(fpgaIndex, offsetValue, &value32, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            flagBoolIn = 1;
            str = uiTreeWidgetItemUpdateFormat(value32, 8,true);
            item->setText(TREE_COLUMN_VALUE, str);
          }
          temp = value32&opera;
          temp >>= bitStart;
          resultStr = uiTreeWidgetItemUpdateFormat(temp, 4,true);
        }
        else if (typeStr.contains("16"))//item类型是16位
        {
          quint16 temp;
          if (flagBoolIn == 0)
          {
            QString str;
            GTSD_CMD_Get16bitFPGAByAddr(fpgaIndex, offsetValue, &value16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
            flagBoolIn = 1;
            str = uiTreeWidgetItemUpdateFormat(value16, 4,true);
            item->setText(TREE_COLUMN_VALUE, str);
          }
          temp = value16&opera;
          temp >>= bitStart;
          resultStr = uiTreeWidgetItemUpdateFormat(temp, 2,true);
        }
        item->child(i)->setText(TREE_COLUMN_VALUE, resultStr);
      }
      else//非Bool类型
      {
        offsetValue = offsetStr.toShort(0,16);
        QString result;
        if (typeStr.contains("32"))
        {
          GTSD_CMD_Get32bitFPGAByAddr(fpgaIndex, offsetValue, &value32, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
          result = uiTreeWidgetItemUpdateFormat(value32, 8,true);
        }
        else if (typeStr.contains("16"))
        {
          GTSD_CMD_Get16bitFPGAByAddr(fpgaIndex, offsetValue, &value16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
          result = uiTreeWidgetItemUpdateFormat(value16, 4,true);
        }
        item->child(i)->setText(TREE_COLUMN_VALUE, result);
      }
    }
  }
  flagBoolIn = 0;
  for (int i = 0; i < item->columnCount(); i++)
  {
    ui->treeWidget->resizeColumnToContents(i);
  }
}

void FpgaDialogSettingRnNet::onDecHexRadioClicked()
{
  disconnect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onTreeItemChanged(QTreeWidgetItem*,int)));
  QTreeWidgetItemIterator it(ui->treeWidget);
  while (*it)
  {
    QString srcString;
    QString valueType;
    quint32 value = 0;
    quint8 byteNum = 2;
    QString addrOft;
    addrOft=(*it)->text(TREE_COLUMN_ADDR);
    if(addrOft!="-1")
    {
      srcString = (*it)->text(TREE_COLUMN_VALUE);
      if (srcString.contains("0x"))//存在0x
      {
        value = srcString.toUInt(0,16);
      }
      else
      {
        value = srcString.toUInt();
      }

      if (ui->rbtn_dec->isChecked())
      {
        (*it)->setText(TREE_COLUMN_VALUE, QString::number(value, 10));
      }
      else
      {
        valueType = (*it)->text(TREE_COLUMN_TYPE);
        if (valueType.contains("32"))
          byteNum = 8;
        else if (valueType.contains("16"))
          byteNum = 4;
        else
        {
          byteNum = 2;
        }

        QString str = QString("%1").arg(value, byteNum, 16, QLatin1Char('0')).toUpper();
        (*it)->setText(TREE_COLUMN_VALUE, "0x"+str);
      }
    }
    //qDebug()<<tr("%1 value:%2,byteNum:%3").arg((*it)->text(0)).arg(value).arg(byteNum);
    ++it;
  }
  ui->treeWidget->resizeColumnToContents(TREE_COLUMN_VALUE);
  connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onTreeItemChanged(QTreeWidgetItem*,int)));
}

void FpgaDialogSettingRnNet::onTreeWidgetItemClicked(QTreeWidgetItem* item, int col)
{
  //if (item->childCount() > 0) return;
  if ((TREE_COLUMN_VALUE == col)&&(("R/W"==item->text(TREE_COLUMN_OPERATION))||("W"==item->text(TREE_COLUMN_OPERATION))))    //“参数值”列允许编辑
  {
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->treeWidget->editItem(item, TREE_COLUMN_VALUE);
    m_hightlightText->storePrevItem(item);//储存
  }
  else //当双击的不是“参数值”列的时候将flags中的可编辑属性去掉
  {
    item->setFlags(item->flags()&~(Qt::ItemIsEditable));
  }
}

void FpgaDialogSettingRnNet::onTreeItemChanged(QTreeWidgetItem *item, int column)
{
  qDebug()<<"item changed "<<item->text(column);
  m_hightlightText->hightlightText();
}
void FpgaDialogSettingRnNet::onItemSelectionChanged()
{
  qDebug()<<"item select changed ";
  m_hightlightText->hightlightText();
}

void FpgaDialogSettingRnNet::onExecuteEnterKey()
{
  QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
  if (("R/W"==currentItem->text(TREE_COLUMN_OPERATION))||("W"==currentItem->text(TREE_COLUMN_OPERATION)))
  {
    quint32 bitwidthTemp;
    bitwidthTemp = currentItem->text(TREE_COLUMN_BITWIDTH).toUInt();
    InputValidator::NumberType numberType;
    if(ui->rbtn_dec->isChecked())
      numberType=InputValidator::DecType;
    else
      numberType=InputValidator::HexType;
    InputValidator iv(currentItem->text(TREE_COLUMN_VALUE),bitwidthTemp,true,numberType);
    if(iv.validate()==false)
    {
      QMessageBox::information(this, tr("Input Error"), "input value over the maximum value \nor the hex value format error ");
      return;
    }
    qDebug() << "key enter itemIsEditable--------------";
    QString typeStr;
    QString bitStart;
    int16 value16;
    int32 value32;
    int16 addrOfstValue;
    int fpgaIndex =getParameterDSPNumberByTopLevelItem(currentItem);
    typeStr = currentItem->text(TREE_COLUMN_TYPE);
    bitStart=currentItem->text(TREE_COLUMN_BITSTART);
    if (bitStart!="-1")//如果是位
    {
      //先判断父类是哪种类型
      QTreeWidgetItem *parent = currentItem->parent();
      quint32 temp;
      quint32 srcValue;
      quint32 bitStart;
      quint32 bitWidth;
      typeStr = parent->text(TREE_COLUMN_TYPE);
      addrOfstValue = parent->text(TREE_COLUMN_ADDR).toShort(0, 16);
      bitStart = currentItem->text(TREE_COLUMN_BITSTART).toUInt();
      bitWidth = currentItem->text(TREE_COLUMN_BITWIDTH).toUInt();
      temp = operationValue(bitStart,bitWidth);
      srcValue = getValueFromItemText(currentItem, TREE_COLUMN_VALUE);//当前编辑的值
      srcValue <<= bitStart;
      if (typeStr.indexOf("16")>-1)
      {
        GTSD_CMD_Get16bitFPGAByAddr(fpgaIndex, addrOfstValue, &value16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
        quint16 result16;
        //如宽度是3位_ _ _ _|x x x _(srcValue=_ _ _ _| 1 0 1_)
        //temp :0 0 0 0 | 1 1 1 0  ~temp 1 1 1 1|0 0 0 1
        //(value16&(~temp|srcValue)) | srcValue   先清零 ，再置1
        result16 = (value16&(~temp|srcValue)) | srcValue;
        GTSD_CMD_Set16bitFPGAByAddr(fpgaIndex, addrOfstValue, result16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
        onExpandRefresh(parent);
      }
      else if (typeStr.indexOf("32")>-1)
      {
        GTSD_CMD_Get32bitFPGAByAddr(fpgaIndex, addrOfstValue, &value32, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
        quint32 result32;
        result32 = (value32 &((~temp) | srcValue)) | srcValue;
        GTSD_CMD_Set16bitFPGAByAddr(fpgaIndex, addrOfstValue, result32, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
        onExpandRefresh(parent);
      }
    }
    else
    {
      addrOfstValue = currentItem->text(TREE_COLUMN_ADDR).toShort(0, 16);
      if (typeStr.indexOf("32")>-1)
      {
        value32 = getValueFromItemText(currentItem, TREE_COLUMN_VALUE);
        GTSD_CMD_Set32bitFPGAByAddr(fpgaIndex, addrOfstValue, value32,mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
        if (currentItem->childCount()>0)
        {
          onExpandRefresh(currentItem);
        }
        else
        {
          //更新自己
          QString str;
          GTSD_CMD_Get32bitFPGAByAddr(fpgaIndex, addrOfstValue, &value32, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
          str = uiTreeWidgetItemUpdateFormat(value32, 8, true);
          currentItem->setText(TREE_COLUMN_VALUE, str);
        }
      }
      else if (typeStr.indexOf("16")>-1)
      {
        value16 = getValueFromItemText(currentItem, TREE_COLUMN_VALUE);
        GTSD_CMD_Set16bitFPGAByAddr(fpgaIndex, addrOfstValue, value16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
        if (currentItem->childCount() > 0)
        {
          onExpandRefresh(currentItem);
        }
        else
        {
          //更新自己
          QString str;
          GTSD_CMD_Get16bitFPGAByAddr(fpgaIndex, addrOfstValue, &value16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
          str = uiTreeWidgetItemUpdateFormat(value16, 4, true);
          currentItem->setText(TREE_COLUMN_VALUE, str);
        }
      }
    }
    m_hightlightText->resetTextColor();
  }

}

void FpgaDialogSettingRnNet::setTreeWidgetStyleSheet()
{
  QString style="QTreeView {\
      show-decoration-selected: 1;\
  }\
  QTreeView::item {\
       border: 1px solid #d9d9d9;\
      border-left-color: transparent;\
      border-top-color: transparent;\
  }\
   QTreeView::item:hover {\
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);\
      border: 1px solid #bfcde4;\
  }\
  QTreeView::item:selected {\
      border: 1px solid #567dbc;\
  }\
  QTreeView::item:selected:active{\
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);\
  }\
  QTreeView::item:selected:!active {\
      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf);\
  }";
  ui->treeWidget->setStyleSheet(style);
}
quint32 FpgaDialogSettingRnNet::operationValue(quint32 bitstart, quint32 bitwidth)
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

/**
 * @brief   NETNAME0->0  DSP0->0  DSP1->1...
 * @param item
 * @return
 */
quint16 FpgaDialogSettingRnNet::getParameterDSPNumberByTopLevelItem(QTreeWidgetItem *item)
{
  quint16 ret;
  QTreeWidgetItem *top=getTopLevelItem(item);
  QString topStr=top->text(TREE_COLUMN_NAME);
  if(m_prmMap.contains(topStr))
    ret=m_prmMap.value(topStr);
  qDebug() <<topStr <<ret;
  return ret;
}
QTreeWidgetItem * FpgaDialogSettingRnNet::getTopLevelItem(QTreeWidgetItem *item)
{
  QTreeWidgetItem* top=item;
  while(top->parent()!=NULL)
  {
    top=top->parent();
  }
  return top;
}
QString FpgaDialogSettingRnNet::uiTreeWidgetItemUpdateFormat(const quint32 value, quint8 hexWidth, bool hasOX)
{
  QString resultStr;

  if (ui->rbtn_dec->isChecked())
  {
//    if(hexWidth<8)//16 bit
//    {
//      quint16 tempValue=value;
//      resultStr = QString::number(tempValue);
//    }
//    else//32 bit
//    {
//      resultStr = QString::number(value);
//    }
    resultStr = QString::number(value);
  }
  else if (ui->rbtn_hex->isChecked())
  {
    if(hexWidth<8)
    {
      quint16 tempValue=value;
      if (hasOX)
        resultStr = tr("0x") + QString("%1").arg(tempValue, hexWidth, 16, QLatin1Char('0')).toUpper();
      else
        resultStr = QString("%1").arg(tempValue);
    }
    else
    {
      if (hasOX)
        resultStr = tr("0x") + QString("%1").arg(value, hexWidth, 16, QLatin1Char('0')).toUpper();
      else
        resultStr = QString("%1").arg(value);
    }
  }
  qDebug()<<"format str:"<<resultStr;
  return resultStr;
}

bool FpgaDialogSettingRnNet::eventFilter(QObject *watchObj, QEvent *eve)
{
  if (watchObj == ui->treeWidget)
  {
    if (eve->type() == QEvent::KeyPress)
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>(eve);
      if ((keyEvent->key() == Qt::Key_Return) || keyEvent->key() == Qt::Key_Enter)
      {
        QTimer::singleShot(100, this, SLOT(onExecuteEnterKey()));
        return true;
      }
//      switch(keyEvent->key())
//      {
//      case Qt::Key_0:
//      case Qt::Key_1:
//      case Qt::Key_2:
//      case Qt::Key_3:
//      case Qt::Key_4:
//      case Qt::Key_5:
//      case Qt::Key_6:
//      case Qt::Key_7:
//      case Qt::Key_8:
//      case Qt::Key_9:
//      case Qt::Key_A:
//      case Qt::Key_B:
//      case Qt::Key_C:
//      case Qt::Key_D:
//      case Qt::Key_E:
//      case Qt::Key_F:
//        qDebug()<<keyEvent->key();
//        qDebug()<<"tree current name:"<<ui->treeWidget->currentItem()->text(0);
//        return true;
//        break;
//      }
    }
    //if (eve->type() == QEvent::MouseButtonPress)
    //{
    //	QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(eve);
    //	if (mouseEvent->button()==Qt::LeftButton)
    //	{
    //		qDebug() << "double click---------";
    //		return true;
    //	}
    //}
  }
  return QWidget::eventFilter(watchObj, eve);
}
qint32 FpgaDialogSettingRnNet::getValueFromItemText(QTreeWidgetItem *item, int col)
{
  qint32 value;
  if (ui->rbtn_dec->isChecked())
  {
    value = item->text(col).toLong();
  }
  else
  {
    value = item->text(col).toLong(0, 16);
  }
  return value;
}
