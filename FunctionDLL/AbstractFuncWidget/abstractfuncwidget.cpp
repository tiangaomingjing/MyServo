#include "abstractfuncwidget.h"

#include <globaldef.h>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QKeyEvent>
#include <QTimer>
#include <QStackedWidget>
#include <QAction>

#include "xmlbuilder.h"
#include "servocontrol.h"
#include "MainGTSD/mainwindow.h"
#include "ServoDriverComDll.h"
#include "QmlFactory/qmlregisterincludes.h"

#define OFFSETADDRESS  "offsetAddress"

AbstractFuncWidget::AbstractFuncWidget( QWidget *parent) : QWidget(parent)
{
  m_uiTree=NULL;
  m_stackedWidget=NULL;
  m_highLightInfo={
    NULL,
    false,
    false,
    "NULL"
  };
}

AbstractFuncWidget::~AbstractFuncWidget()
{
//  qDebug()<<"abstractFunctionWidget release ->";
}
//!-------------------------public function------------------------------
void AbstractFuncWidget::readTreeXMLFile(MainWindow *mainWindow,QString fileName_noxml, int axisNum)
{
  //先从实际的对象获取界面树指针和stackWidget
  setTreeAndStackedWidget(mainWindow,axisNum);
  //构建树内容
  buildTreeWidget(fileName_noxml);
  //改变不是根节点时的字体颜色
  setTreeWidgetParentBackgroundColor();
  //设置读FLASH action (右键菜单)是否可用
  setActionReadFuncValueFromFlashEnable();

  //每一个子类的qml界面处理都不一样
  QML_Register::declareClassToQML();
  m_qmlIconFilePath=ICON_FILE_PATH;
  createUiByQml();

  //信号连接处理，有的子类没有与主mainwidow有信息交流，
  //用virtual function做差异化处理
  connectionSignalSlotHandler();
}
void AbstractFuncWidget::setTreeWidgetHeaderLables()
{
  QStringList header;
  header<<tr("Axis_%1").arg(m_axisNumber)<<NORMAL_VALUE<<NORMAL_TYPE<<\
          NORMAL_UINT<<NORMAL_DOWNLIMIT<<NORMAL_UPLIMIT<<NORMAL_INTRODUCTION<<NORMAL_FRAMOFFSET;
  m_uiTree->setHeaderLabels(header);
}

//!-------------------------protected function--------------------------
void AbstractFuncWidget::buildTreeWidget(QString &fileName_noxml)
{
  QString fileName=fileName_noxml+".xml";
  QTreeWidget *tree;
  tree=XmlBuilder::readTreeWidgetFromFile(fileName);
//  XmlBuilder::functionTreeWidgetNormalization(tree);

  if(m_uiTree==NULL)
  {
    QMessageBox::information(0,tr("Read ERROR"),tr("no getUiTree() function override!"));
    return;
  }
  QTreeWidgetItem *item;
  for(int i=0;i<tree->topLevelItemCount();i++)
  {
    item=tree->topLevelItem(i)->clone();
    m_uiTree->addTopLevelItem(item);
  }
  QTreeWidgetItem *head;
  head=tree->headerItem()->clone();
  m_uiTree->setHeaderItem(head);

  tree->clear();
  delete tree;

  for(int i=0;i<m_uiTree->columnCount();i++)
    m_uiTree->resizeColumnToContents(i);
  m_uiTree->setColumnWidth(COL_VALUE,100);

  if(m_stackedWidget==NULL)
  {
    QMessageBox::information(0,tr("Read ERROR"),tr("no getUiStackedWidget() function override!"));
    return;
  }
  //增加右键弹出Action
  m_actSwitchView= new QAction(this);
  m_actSwitchView->setText(tr("switch to list view"));
  m_actSwitchView->setCheckable(true);
  m_actSwitchView->setChecked(false);
  connect(m_actSwitchView,SIGNAL(triggered(bool)),this,SLOT(onSwitchView(bool)));
  m_stackedWidget->addAction(m_actSwitchView);
  QAction *actSeparator=new QAction(this);
  actSeparator->setSeparator(true);
  m_stackedWidget->addAction(actSeparator);

  m_actReadFuncValue=new QAction(this);
  m_actReadFuncValue->setText(tr("RAM value"));
  connect(m_actReadFuncValue,SIGNAL(triggered(bool)),this,SLOT(onActionReadFuncValueFromRam()));
  m_stackedWidget->addAction(m_actReadFuncValue);

  m_actReadFuncValueFromFlash=new QAction(this);
  m_actReadFuncValueFromFlash->setText(tr("FLASH value"));
  connect(m_actReadFuncValueFromFlash,SIGNAL(triggered(bool)),this,SLOT(onActionReadFuncValueFromFlash()));
  m_stackedWidget->addAction(m_actReadFuncValueFromFlash);

}

void AbstractFuncWidget::setTreeStyle(QTreeWidget *tree)
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
  tree->setStyleSheet(style);
}

void AbstractFuncWidget::saveTreeXMLFile(QString &fileName)
{
  qDebug()<<fileName;
}


bool AbstractFuncWidget::eventFilter(QObject *obj, QEvent *eve)
{
  if(eve->type()==QEvent::KeyPress)
  {
    if(obj==m_uiTree)
    {
      QKeyEvent *keyEvent=static_cast<QKeyEvent *>(eve);
      if(keyEvent->key()==Qt::Key_Enter||keyEvent->key()==Qt::Key_Return)
      {
        QTimer::singleShot(100,this,SLOT(onEnterKeyPress()));
        return true;
      }
    }
  }
  return QWidget::eventFilter(obj,eve);
}

void AbstractFuncWidget::setActionReadFuncValueFromFlashEnable()
{
  m_actReadFuncValueFromFlash->setEnabled(true);
}

void AbstractFuncWidget::createUiByQml()
{

}

//!-------------------------------private function---------------------------------
void AbstractFuncWidget::setTreeAndStackedWidget(MainWindow *mainWindow,int axisNum)
{
  m_axisNumber=axisNum;
  mp_mainWindow=mainWindow;
  m_uiTree=getUiTree();
  m_stackedWidget=getUiStackedWidget();
  m_uiTree->installEventFilter(this);
  setTreeStyle(m_uiTree);

  connect(m_uiTree,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeItemClickedEdit(QTreeWidgetItem*,int)));
  connect(m_uiTree,SIGNAL(itemSelectionChanged()),this,SLOT(onItemSelecttionChangedHighlight()));
}
void AbstractFuncWidget::setTreeWidgetParentBackgroundColor()
{
  QTreeWidgetItem *item;
  QTreeWidgetItemIterator it_src(m_uiTree);
  QBrush brush;
  brush.setColor(Qt::darkBlue);
  while((*it_src))
  {
    item=(*it_src);
    if(item->childCount()>0)
    {
      for(int i=0;i<item->columnCount();i++)
//        item->setBackgroundColor(i,Qt::lightGray);
      item->setForeground(i,brush);
//      qDebug()<<"set background color";
    }
    it_src++;
  }
  m_uiTree->repaint();
}

//!----------------------------------protected slots-------------------------------------
void AbstractFuncWidget::onItemExpanded(QTreeWidgetItem *item)
{
  Q_UNUSED(item);
  if(mp_mainWindow->getComOpenState())
  {
//    UserConfig *config=mp_mainWindow->getUserConfig();
//    COM_TYPE comtype=(COM_TYPE)config->com.id;
//    ServoControl::updataFlashRamTreeWidgetByExpanded(item,m_axisNumber,comtype);
  }
  else
  {
    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
  }
}

void AbstractFuncWidget::onTreeItemClickedEdit(QTreeWidgetItem *item, int column)
{
  if(column==COL_FUNC_VALUE)
  {
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    m_uiTree->editItem(item,column);
    qDebug()<<"item clicked";
    m_highLightInfo.editItem=item;
    m_highLightInfo.editFlag=true;
    m_highLightInfo.prevItemText=item->text(column);
  }
  else item->setFlags(item->flags()&(~Qt::ItemIsEditable));
}
void AbstractFuncWidget::onItemSelecttionChangedHighlight()
{
  if (m_highLightInfo.editFlag)
  {
    QString str = m_highLightInfo.editItem->text(COL_FUNC_VALUE);
    qDebug() << "after:" << str;
    if (m_highLightInfo.prevItemText !=str )
    {
      if (!m_highLightInfo.enterFlag)
      {
        m_highLightInfo.editItem->setTextColor(COL_FUNC_VALUE, QColor(Qt::red));
        emit itemValueChanged();
      }
    }
    m_highLightInfo.editFlag = false;
  }
  m_highLightInfo.enterFlag = false;
}
//!在通用模块中不是用回车送命令，用config and save一次送完命令，只有在Flash,与RAM中才用回车送命令
void AbstractFuncWidget::onEnterKeyPress()
{
  if(!mp_mainWindow->getComOpenState())
  {
    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    return;
  }
//  QTreeWidgetItem *item=NULL;
//  item=m_uiTree->currentItem();
//  if(item!=NULL)
//  {
//    qDebug()<<"enter press ";
//    UserConfig *config=mp_mainWindow->getUserConfig();
//    COM_TYPE comtype=(COM_TYPE)config->com.id;
//    QTreeWidget *cmdTree=mp_mainWindow->getFunctionCmdTree();
//    ServoControl::writeFunctionValue2Ram(item,cmdTree,m_axisNumber,comtype);
//    m_highLightInfo.enterFlag=true;
//    item->setTextColor(COL_FUNC_VALUE, QColor(Qt::black));
//  }
}

void AbstractFuncWidget::onWriteFuncTreetoServoRam()
{
  if(!mp_mainWindow->getComOpenState())
  {
    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    return;
  }
  m_highLightInfo.enterFlag=true;
  UserConfig *config=mp_mainWindow->getUserConfig();
  COM_TYPE comtype=(COM_TYPE)config->com.id;
  QTreeWidget *cmdTree=mp_mainWindow->getFunctionCmdTree();
  ServoControl::writeFunctionValue2RamAllTree(m_uiTree,cmdTree,m_axisNumber,comtype,config->com.rnStation);
  qDebug()<<this->objectName()<<"axisnum:"<<m_axisNumber;
  emit itemValueChanged();
}

void AbstractFuncWidget::onSwitchView(bool sw)
{
  if(sw)
  {
    m_stackedWidget->setCurrentIndex(1);
    m_actSwitchView->setText(tr("switch to graph view"));
//    onActionReadFuncValueFromRam();
  }
  else
  {
    m_stackedWidget->setCurrentIndex(0);
    m_actSwitchView->setText(tr("switch to list view"));
    setActiveNow(true);
  }
}

void AbstractFuncWidget::onActionReadFuncValueFromFlash()
{
  if(!mp_mainWindow->getComOpenState())
  {
    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    return;
  }
  m_highLightInfo.enterFlag=true;
  quint16 offsetAddr=0;
  UserConfig *config=mp_mainWindow->getUserConfig();
  COM_TYPE comtype=(COM_TYPE)config->com.id;
  QTreeWidget *cmdTree=mp_mainWindow->getFunctionCmdTree();

  //从扩展树中找到偏移基地址
  QTreeWidget *funcExtion=mp_mainWindow->getFunctionExtensionTree();
  QTreeWidgetItem *item;
  QTreeWidgetItem *itemChild;
  for(int i=0;i<funcExtion->topLevelItemCount();i++)
  {
    item=funcExtion->topLevelItem(i);
    if(OFFSETADDRESS==item->text(COL_FUNC_EXTENSION_NAME))
    {
      itemChild=item->child(m_axisNumber);
      offsetAddr=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
      break;
    }
  }
  ServoControl::updateFunctionValueFromFlashAllTree(m_uiTree,cmdTree,offsetAddr,m_axisNumber,comtype,config->com.rnStation);
  qDebug()<<this->objectName()<<"axisnum:"<<m_axisNumber;
  emit itemValueChanged();
}


void AbstractFuncWidget::onActionReadFuncValueFromRam()
{
  if(!mp_mainWindow->getComOpenState())
  {
    //QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    return;
  }
  m_highLightInfo.enterFlag=true;
  UserConfig *config=mp_mainWindow->getUserConfig();
  COM_TYPE comtype=(COM_TYPE)config->com.id;
  QTreeWidget *cmdTree=mp_mainWindow->getFunctionCmdTree();

  ServoControl::updateFunctionValueFromRamAllTree(m_uiTree,cmdTree,m_axisNumber,comtype,config->com.rnStation);
  qDebug()<<this->objectName()<<"axisnum:"<<m_axisNumber;
  emit itemValueChanged();
}

void AbstractFuncWidget::updateUiWhenNavigationTreeClicked()
{
  onActionReadFuncValueFromRam();
}

//写通用模块的FLASH 与 RAM 都需要cmdtree ,Flash是要获得增益值 ,RAM要获得通用指令结构
void AbstractFuncWidget::onWriteFuncTreetoServoFlash()
{
  if(!mp_mainWindow->getComOpenState())
  {
    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    return;
  }
  m_highLightInfo.enterFlag=true;
  quint16 offsetAddr=0;
  UserConfig *config=mp_mainWindow->getUserConfig();
  COM_TYPE comtype=(COM_TYPE)config->com.id;
  QTreeWidget *cmdTree=mp_mainWindow->getFunctionCmdTree();

  //从ExtensionTree中获得Flash的偏移地址
  QTreeWidget *funcExtion=mp_mainWindow->getFunctionExtensionTree();
  QTreeWidgetItem *item;
  QTreeWidgetItem *itemChild;
  for(int i=0;i<funcExtion->topLevelItemCount();i++)
  {
    item=funcExtion->topLevelItem(i);
    if(OFFSETADDRESS==item->text(COL_FUNC_EXTENSION_NAME))
    {
      itemChild=item->child(m_axisNumber);
      offsetAddr=itemChild->text(COL_FUNC_EXTENSION_PARAMETER).toUInt();
      break;
    }
  }

  //从cmdtree中获得增益调用写指令函数
  ServoControl::writeFunctionValue2FlashAllTree(m_uiTree,cmdTree,offsetAddr,m_axisNumber,comtype,config->com.rnStation);
  qDebug()<<this->objectName()<<"axisnum:"<<m_axisNumber;
  emit itemValueChanged();
}
