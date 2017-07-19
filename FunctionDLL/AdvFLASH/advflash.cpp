#include "advflash.h"
#include "ui_advflash.h"
#include "xmlbuilder.h"
#include "servocontrol.h"
#include "MainGTSD/mainwindow.h"

#include <QMessageBox>
#include <QKeyEvent>

AdvFLASH::AdvFLASH(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::AdvFLASH())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(1);
}
AdvFLASH::~AdvFLASH()
{
qDebug()<<"AdvFLASH release ->";
}
void AdvFLASH::setTreeWidgetHeaderLables()
{
  QStringList header;
  header<<tr("FLASH_Axis_%1").arg(m_axisNumber)<<NORMAL_VALUE<<NORMAL_TYPE<<NORMAL_OFFSET\
        <<NORMAL_ISBITS<<NORMAL_BITSTART<<NORMAL_BITWIDTH\
        <<NORMAL_AXISNUM<<NORMAL_HASCHILD<<NORMAL_CHILDNUM<<NORMAL_PARENT;
  ui->treeWidget->setHeaderLabels(header);
}

//------------------------slots function------------------------------------
//void AdvFLASH::updateTreeWidgetValuefromServo()
//{
//  UserConfig *config=mp_mainWindow->getUserConfig();
//  COM_TYPE comtype=(COM_TYPE)config->com.id;
//  ServoControl::updateFlashTreeWidget(ui->treeWidget,m_axisNumber,comtype,config->com.rnStation);
//  qDebug()<<this->objectName();
//}

void AdvFLASH::onItemExpanded(QTreeWidgetItem *item)
{
  if(mp_mainWindow->getComOpenState())
  {
    UserConfig *config=mp_mainWindow->getUserConfig();
    COM_TYPE comtype=(COM_TYPE)config->com.id;
    ServoControl::updateFlashTreeWidgetByExpanded(item,m_axisNumber,comtype,config->com.rnStation);
  }
  else
  {
//    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    item->treeWidget()->resizeColumnToContents(COL_NAME);
    item->treeWidget()->resizeColumnToContents(COL_VALUE);
    item->treeWidget()->resizeColumnToContents(COL_TYPE);
  }
}

void AdvFLASH::onTreeItemClickedEdit(QTreeWidgetItem *item,int column)
{
  if(column==COL_VALUE)
  {
    if(!(item->childCount()>0))
    {
      item->setFlags(item->flags()|Qt::ItemIsEditable);
      ui->treeWidget->editItem(item,column);
      qDebug()<<"item clicked";
      m_highLightInfo.editItem=item;
      m_highLightInfo.editFlag=true;
      m_highLightInfo.prevItemText=item->text(column);
    }
  }
  else item->setFlags(item->flags()&(~Qt::ItemIsEditable));
}

void AdvFLASH::onItemSelecttionChangedHighlight()
{
  if (m_highLightInfo.editFlag)
  {
    QString str = m_highLightInfo.editItem->text(COL_VALUE);
    qDebug() << "after:" << str;
    if (m_highLightInfo.prevItemText !=str )
    {
      if (!m_highLightInfo.enterFlag)
      {
        m_highLightInfo.editItem->setTextColor(COL_VALUE, QColor(Qt::red));
      }
    }
    m_highLightInfo.editFlag = false;
  }
  m_highLightInfo.enterFlag = false;
}

void AdvFLASH::onEnterKeyPress()
{

  QTreeWidgetItem *item=NULL;
  item=ui->treeWidget->currentItem();
  if(item!=NULL)
  {
    qDebug()<<"enter press ";
    UserConfig *config=mp_mainWindow->getUserConfig();
    COM_TYPE comtype=(COM_TYPE)config->com.id;
    ServoControl::writeServoFlash(item,m_axisNumber,comtype,config->com.rnStation);
    m_highLightInfo.enterFlag=true;
    item->setTextColor(COL_VALUE, QColor(Qt::black));
  }
}
//-------------------------private function--------------------------
QTreeWidget* AdvFLASH::getUiTree()
{
  return ui->treeWidget;
}
QStackedWidget* AdvFLASH::getUiStackedWidget()
{
  return ui->stackedWidget;
}

void AdvFLASH::buildTreeWidget(QString &fileName_noxml)
{
//  qDebug()<<"AdvFLASH"<<fileName_noxml;
  QString fileName;
  if(m_axisNumber%2)//奇数
  {
    fileName=fileName_noxml+"1.xml";
  }
  else//偶数
  {
    fileName=fileName_noxml+"0.xml";
  }
  QTreeWidgetItem *item;
  QTreeWidget *tree;
  tree=XmlBuilder::readTreeWidgetFromFile(fileName);
//  XmlBuilder::ramFlashTreeWidgetNormalization(tree);
  for(int i=0;i<tree->topLevelItemCount();i++)
  {
    item=tree->topLevelItem(i)->clone();
    ui->treeWidget->addTopLevelItem(item);
  }
  QTreeWidgetItem *head;
  head=tree->headerItem()->clone();
  ui->treeWidget->setHeaderItem(head);
  tree->clear();
  delete tree;

  ui->treeWidget->hideColumn(COL_AXISNUM);
  ui->treeWidget->hideColumn(COL_PARENT);
  ui->treeWidget->hideColumn(COL_HASCHILD);
  ui->treeWidget->hideColumn(COL_CHILDNUM);
  ui->treeWidget->expandToDepth(0);
  ui->treeWidget->resizeColumnToContents(COL_NAME);
  ui->treeWidget->resizeColumnToContents(COL_VALUE);
  ui->treeWidget->resizeColumnToContents(COL_TYPE);

  //在这个地方连接，不然初始化时会展开，调用槽函数->出错
  connect(ui->treeWidget,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(onItemExpanded(QTreeWidgetItem*)));
}
