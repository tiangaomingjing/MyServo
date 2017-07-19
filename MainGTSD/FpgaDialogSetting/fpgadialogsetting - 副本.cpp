#include "fpgadialogsetting.h"
#include "ui_fpgadialogsetting.h"

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
#define NAME_DSP0 "DSP0"
#define NAME_DSP1 "DSP1"


//FpgaDialogSetting::FpgaDialogSetting(QWidget *parent) :
//QWidget(parent)
//{
//
//}
typedef enum{
  FPAG_TREE_INDEX_NAME,
  FPAG_TREE_INDEX_VALUE,
  FPAG_TREE_INDEX_ADDROFFSET,
  FPAG_TREE_INDEX_TYPE,
  FPAG_TREE_INDEX_BITSTART,
  FPAG_TREE_INDEX_BITWIDTH,
  FPAG_TREE_INDEX_OPERATION,
  FPAG_TREE_INDEX_COUNT

}FpgaTreeIndex;
FpgaDialogSetting::FpgaDialogSetting(QTreeWidget *tree, MainWindow *mainWindow, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::FpgaDialogSetting),
  mp_mainWindow(mainWindow)
{
	ui->setupUi(this);
	ui->treeWidget->installEventFilter(this);
	ui->radio_dec->setChecked(true);
  m_hightlightText=new HighlightText(1,this);
	
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
	connect(ui->treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onExpandRefresh(QTreeWidgetItem*)));
	connect(ui->btnExit, SIGNAL(clicked()), this, SLOT(hide()));
	connect(ui->radio_dec, SIGNAL(clicked(bool)), this, SLOT(onDecHexRadioClicked()));
	connect(ui->radio_hex, SIGNAL(clicked(bool)), this, SLOT(onDecHexRadioClicked()));
	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeWidgetItemClicked(QTreeWidgetItem*, int)));
//  connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onTreeItemChanged(QTreeWidgetItem*,int)));
  connect(ui->treeWidget,SIGNAL(itemSelectionChanged()),this,SLOT(onItemSelectionChanged()));
	//------------------------------------------------------------------------
  QTreeWidgetItem *item;
	for (int i = 0; i < tree->topLevelItemCount(); i++)
	{
		item = tree->topLevelItem(i)->clone();
		ui->treeWidget->addTopLevelItem(item);
	}
	
	QStringList headerList;
	for (int i = 0; i < tree->headerItem()->columnCount(); i++)
	{
		headerList << tree->headerItem()->text(i);
	}
	ui->treeWidget->setHeaderLabels(headerList);
	for (int i = 0; i < tree->headerItem()->columnCount(); i++)
	{
		ui->treeWidget->resizeColumnToContents(i);
		if ("AddrOfst" == tree->headerItem()->text(i))
			m_addrOfstIndex = i;
		else if ("Type" == tree->headerItem()->text(i))
			m_typeIndex = i;
		else if ("Value" == tree->headerItem()->text(i))
			m_valueIndex = i;
		else if ("BitWidth" == tree->headerItem()->text(i))
			m_bitWidthIndex = i;
		else if ("BitStart" == tree->headerItem()->text(i))
			m_startBitIndex = i;
		else if ("Operation" == tree->headerItem()->text(i))
			m_operationIndex = i;
	}
	this->setWindowModality(Qt::WindowModality::NonModal);
	ui->treeWidget->expandToDepth(0);
}

FpgaDialogSetting::~FpgaDialogSetting()
{
	delete ui;
}
///-------------------private slots functions------------------------------
void FpgaDialogSetting::onExpandRefresh(QTreeWidgetItem *item)
{
	int16 value16;
	int32 value32;
	static quint8 flagBoolIn = 0;
	int16 offsetValue;
	QString offsetStr;
	QString typeStr;

	int fpgaIndex = getTopLevelItemIndex(item);
	qDebug() << fpgaIndex;

	for (int i = 0; i < item->childCount(); i++)
	{
		offsetStr = item->child(i)->text(m_addrOfstIndex);
		if (offsetStr != "-1")
		{
			//qDebug() << item->child(i)->text(0);
			//更新数值 :分位控制与没有位控制，判断type是不是bool,如果是发送其父类得到其值，再将值用或与方法写到每一位去
			typeStr = item->child(i)->text(m_typeIndex);
			if (typeStr == "Bool")//是Bool类型
			{
				QString resultStr;
				quint32 bitStart, bitWidth, opera;
				typeStr = item->text(m_typeIndex);
				offsetStr = item->text(m_addrOfstIndex);
				offsetValue = offsetStr.toShort(0, 16);//item的偏移地址
				bitStart = item->child(i)->text(m_startBitIndex).toUInt();
				bitWidth = item->child(i)->text(m_bitWidthIndex).toUInt();
				opera = operationValue(bitStart, bitWidth);
				if (typeStr.indexOf("32")>-1)//item类型是32位
				{
//					qDebug() << "bool 32";
					
					quint32 temp = 0;
					if (flagBoolIn==0)
					{
						QString str;
            GTSD_CMD_Get32bitFPGAByAddr(fpgaIndex, offsetValue, &value32, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
						flagBoolIn = 1;
						str = uiTreeWidgetItemUpdateFormat(value32, 8,true);
						item->setText(m_valueIndex, str);
					}
					//GTSD_CMD_Get32bitFPGAByAddr(fpgaIndex, offsetValue, &value32, GTSD_COM_TYPE_NET);//有问题?????只读一次，还要更新item
					temp = value32&opera;
          temp >>= bitStart;
					resultStr = uiTreeWidgetItemUpdateFormat(temp, 4,true);
				}
				else if (typeStr.indexOf("16") > -1)//item类型是16位
				{
//					qDebug() << "bool 16";
					quint16 temp;
					if (flagBoolIn == 0)
					{
						QString str;
            GTSD_CMD_Get16bitFPGAByAddr(fpgaIndex, offsetValue, &value16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
						flagBoolIn = 1;
						str = uiTreeWidgetItemUpdateFormat(value16, 4,true);
						item->setText(m_valueIndex, str);
					}
					temp = value16&opera;
					temp >>= bitStart;
					resultStr = uiTreeWidgetItemUpdateFormat(temp, 2,true);
				}
				item->child(i)->setText(m_valueIndex, resultStr);
			}
			else//非Bool类型
			{
				offsetValue = offsetStr.toShort(0,16);
//				qDebug() << "addrOfst" << offsetValue;
				QString result;
				if (typeStr.indexOf("32")>-1)
				{
//					qDebug() << "32";
          GTSD_CMD_Get32bitFPGAByAddr(fpgaIndex, offsetValue, &value32, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
					result = uiTreeWidgetItemUpdateFormat(value32, 8,true);
				}
				else if (typeStr.indexOf("16") > -1)
				{
//					qDebug() << "16";
          GTSD_CMD_Get16bitFPGAByAddr(fpgaIndex, offsetValue, &value16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
					result = uiTreeWidgetItemUpdateFormat(value16, 4,true);
				}
				item->child(i)->setText(m_valueIndex, result);
			}
		}
	}
	flagBoolIn = 0;
	for (int i = 0; i < item->columnCount(); i++)
	{
		ui->treeWidget->resizeColumnToContents(i);
	}
}
void FpgaDialogSetting::onDecHexRadioClicked()
{
	QTreeWidgetItemIterator it(ui->treeWidget);
	while (*it)
	{
		QString srcString;
    QString valueType;
		quint32 value = 0;
		quint8 byteNum = 2;
		srcString = (*it)->text(m_valueIndex);
    if (srcString.indexOf("0x")>-1)//存在0x
		{
			value = srcString.toUInt(0,16);
		}
		else
		{
			value = srcString.toUInt();
		}
		if (ui->radio_dec->isChecked())
		{
			(*it)->setText(m_valueIndex, QString::number(value, 10));
		}
		else
		{
      valueType = (*it)->text(m_typeIndex);
      if (valueType.indexOf("32") > -1)
				byteNum = 8;
      else if (valueType.indexOf("16") > -1)
				byteNum = 4;
			else
			{
				byteNum = 2;
//				if ("16" == (*it)->text(m_bitWidthIndex))
//					byteNum = 4;
			}

			QString str = QString("%1").arg(value, byteNum, 16, QLatin1Char('0')).toUpper();
			(*it)->setText(m_valueIndex, tr("0x")+str);
		}
    //qDebug()<<tr("%1 value:%2,byteNum:%3").arg((*it)->text(0)).arg(value).arg(byteNum);
		++it;
	}
	ui->treeWidget->resizeColumnToContents(m_valueIndex);
}
void FpgaDialogSetting::onTreeWidgetItemClicked(QTreeWidgetItem* item, int col)
{
	//if (item->childCount() > 0) return;
	if ((m_valueIndex == col)&&("R/W"==item->text(m_operationIndex)))    //“参数值”列允许编辑
	{
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		ui->treeWidget->editItem(item, m_valueIndex);
    m_hightlightText->storePrevItem(item);//储存
	}
  else //当双击的不是“参数值”列的时候将flags中的可编辑属性去掉
	{
		item->setFlags(item->flags()&~(Qt::ItemIsEditable));
	}
}
void FpgaDialogSetting::onTreeItemChanged(QTreeWidgetItem *item, int column)
{
  qDebug()<<"item changed "<<item->text(column);
  m_hightlightText->hightlightText();
}
void FpgaDialogSetting::onItemSelectionChanged()
{
  qDebug()<<"item select changed ";
  m_hightlightText->hightlightText();
}
void FpgaDialogSetting::onExecuteEnterKey()
{
	QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
	if ("R/W"==currentItem->text(m_operationIndex))
	{
		quint32 bitwidthTemp;
		bitwidthTemp = currentItem->text(m_bitWidthIndex).toUInt();
    InputValidator::NumberType numberType;
    if(ui->radio_dec->isChecked())
      numberType=InputValidator::DecType;
    else
      numberType=InputValidator::HexType;
    InputValidator iv(currentItem->text(m_valueIndex),bitwidthTemp,true,numberType);
    if(iv.validate()==false)
    {
      QMessageBox::information(this, tr("Input Error"), "input value over the maximum value \nor the hex value format error ");
      return;
    }
		qDebug() << "key enter itemIsEditable--------------";
		QString typeStr;
		int16 value16;
		int32 value32;
		int16 addrOfstValue;
		int fpgaIndex = getTopLevelItemIndex(currentItem);
		typeStr = currentItem->text(m_typeIndex);
		if (typeStr=="Bool")
		{
			//先判断父类是哪种类型
			QTreeWidgetItem *parent = currentItem->parent();
			quint32 temp;
			quint32 srcValue;
			quint32 bitStart;
			quint32 bitWidth;
			typeStr = parent->text(m_typeIndex);
			addrOfstValue = parent->text(m_addrOfstIndex).toShort(0, 16);
			bitStart = currentItem->text(m_startBitIndex).toUInt();
			bitWidth = currentItem->text(m_bitWidthIndex).toUInt();
			temp = operationValue(bitStart,bitWidth);
      srcValue = getValueFromItemText(currentItem, m_valueIndex);//当前编辑的值
			srcValue <<= bitStart;
			if (typeStr.indexOf("16")>-1)
			{
        GTSD_CMD_Get16bitFPGAByAddr(fpgaIndex, addrOfstValue, &value16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
				quint16 result16;
        //如宽度是3位_ _ _ _|x x x _(srcValue=_ _ _ _| 1 0 1_)
        //temp :0 0 0 0 | 1 1 1 0  ~temp 1 1 1 1|0 0 0 1
        //(value16&(~temp|srcValue)) | srcValue先清零 ，再置1
				result16 = (value16&(~temp|srcValue)) | srcValue;
        GTSD_CMD_Set16bitFPGAByAddr(fpgaIndex, addrOfstValue, result16, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
				onExpandRefresh(parent);
			}
			else if (typeStr.indexOf("32")>-1)
			{
        GTSD_CMD_Get32bitFPGAByAddr(fpgaIndex, addrOfstValue, &value32, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
				quint32 result32;
				result32 = (value32 &((~temp) | srcValue)) | srcValue;
        GTSD_CMD_Set32bitFPGAByAddr(fpgaIndex, addrOfstValue, result32, mp_mainWindow->getUserConfig()->com.id,mp_mainWindow->getUserConfig()->com.rnStation);
				onExpandRefresh(parent);
			}
		}
		else
		{
			addrOfstValue = currentItem->text(m_addrOfstIndex).toShort(0, 16);
			if (typeStr.indexOf("32")>-1)
			{
				value32 = getValueFromItemText(currentItem, m_valueIndex);
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
					currentItem->setText(m_valueIndex, str);
				}
			}
			else if (typeStr.indexOf("16")>-1)
			{
				value16 = getValueFromItemText(currentItem, m_valueIndex);
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
					currentItem->setText(m_valueIndex, str);
				}
			}
		}
    m_hightlightText->resetTextColor();
	}
	
}

///--------------------private functions-----------------------------------
int FpgaDialogSetting::getTopLevelItemIndex(QTreeWidgetItem *item)
{
	int index = 0;
	findTopLevelItem(item);
  if (NAME_DSP0 == m_currentParentItemPtr->text(0))
		index = 0;
  else if (NAME_DSP1 == m_currentParentItemPtr->text(0))
		index = 1;
	return index;
}
void FpgaDialogSetting::findTopLevelItem(QTreeWidgetItem *item)
{
	QTreeWidgetItem *parentItem;
	parentItem = item->parent();

	if (parentItem != NULL)
		findTopLevelItem(parentItem);
	else
	{
		m_currentParentItemPtr = item;
		return;
	}
}
quint32 FpgaDialogSetting::operationValue(quint32 bitstart, quint32 bitwidth)
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
QString FpgaDialogSetting::uiTreeWidgetItemUpdateFormat(const quint32 value, quint8 hexWidth, bool hasOX)
{
	QString resultStr;

	if (ui->radio_dec->isChecked())
	{
//    if(hexWidth<8)//16 bit
//    {
//      quint16 tempValue=value;
//      resultStr = QString::number(tempValue).toUpper();
//    }
//    else//32 bit
//    {
//      resultStr = QString::number(value).toUpper();
//    }
    resultStr = QString::number(value);
	}
	else if (ui->radio_hex->isChecked())
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
	return resultStr;
}
bool FpgaDialogSetting::eventFilter(QObject *watchObj, QEvent *eve)
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
qint32 FpgaDialogSetting::getValueFromItemText(QTreeWidgetItem *item, int col)
{
	qint32 value;
	if (ui->radio_dec->isChecked())
	{
		value = item->text(col).toLong();
	}
	else
	{
		value = item->text(col).toLong(0, 16);
	}
	return value;
}
