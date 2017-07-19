#include "advram.h"
#include "ui_advram.h"
#include "xmlbuilder.h"
#include "ServoControl/servocontrol.h"
#include "MainGTSD/mainwindow.h"

#include <QMessageBox>

AdvRAM::AdvRAM(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::AdvRAM())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(1);
//  connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeItemClickedEdit(QTreeWidgetItem*,int)));
//  connect(ui->treeWidget,SIGNAL(itemSelectionChanged()),this,SLOT(onItemSelecttionChangedHighlight()));
}
AdvRAM::~AdvRAM()
{
  qDebug()<<"AdvRAM release ->";
}
void AdvRAM::setTreeWidgetHeaderLables()
{
  QStringList header;
  header<<tr("RAM_Axis_%1").arg(m_axisNumber)<<NORMAL_VALUE<<NORMAL_TYPE<<NORMAL_OFFSET\
        <<NORMAL_ISBITS<<NORMAL_BITSTART<<NORMAL_BITWIDTH\
        <<NORMAL_AXISNUM<<NORMAL_HASCHILD<<NORMAL_CHILDNUM<<NORMAL_PARENT;
  ui->treeWidget->setHeaderLabels(header);
}
void AdvRAM::buildTreeWidget(QString &fileName_noxml)
{
  qDebug()<<"AdvRAM"<<fileName_noxml<<m_axisNumber;
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

  connect(ui->treeWidget,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(onItemExpanded(QTreeWidgetItem*)));
}

//----------------------------slots----------------------------------
//void AdvRAM::updateTreeWidgetValuefromServo()
//{
//  UserConfig *config=mp_mainWindow->getUserConfig();
//  COM_TYPE comtype=(COM_TYPE)config->com.id;
//  ServoControl::updateFlashTreeWidget(ui->treeWidget,m_axisNumber,comtype,config->com.rnStation);
//  qDebug()<<this->objectName();
//}

void AdvRAM::onItemExpanded(QTreeWidgetItem *item)
{
  if(mp_mainWindow->getComOpenState())
  {
    UserConfig *config=mp_mainWindow->getUserConfig();
    COM_TYPE comtype=(COM_TYPE)config->com.id;
    QMap<QString,QVariant>map;
    map[EXTENSION_ADVCONTROLPRM_GSERVDRV]=mp_mainWindow->getModuleShareMapData()->value(EXTENSION_ADVCONTROLPRM_GSERVDRV,0);
    map[EXTENSION_ADVCONTROLPRM_GAUXFUNC]=mp_mainWindow->getModuleShareMapData()->value(EXTENSION_ADVCONTROLPRM_GAUXFUNC,0);
    qDebug()<<"gSevDrv :"<<map.value(EXTENSION_ADVCONTROLPRM_GSERVDRV);
    qDebug()<<"gAuxFunc :"<<map.value(EXTENSION_ADVCONTROLPRM_GAUXFUNC);
    ServoControl::updateRamTreeWidgetByExpanded(item,m_axisNumber,map,comtype,config->com.rnStation);
  }
  else
  {
//    QMessageBox::information(0,tr("connect"),tr("please open com first !"));
    item->treeWidget()->resizeColumnToContents(COL_NAME);
    item->treeWidget()->resizeColumnToContents(COL_VALUE);
    item->treeWidget()->resizeColumnToContents(COL_TYPE);
  }
}

void AdvRAM::onTreeItemClickedEdit(QTreeWidgetItem *item,int column)
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

void AdvRAM::onItemSelecttionChangedHighlight()
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

void AdvRAM::onEnterKeyPress()
{

  QTreeWidgetItem *item=NULL;
  item=ui->treeWidget->currentItem();
  if(item!=NULL)
  {
    qDebug()<<"enter press ";
    UserConfig *config=mp_mainWindow->getUserConfig();
    COM_TYPE comtype=(COM_TYPE)config->com.id;
    QMap<QString,QVariant>map;
    map[EXTENSION_ADVCONTROLPRM_GSERVDRV]=mp_mainWindow->getModuleShareMapData()->value(EXTENSION_ADVCONTROLPRM_GSERVDRV,0);
    map[EXTENSION_ADVCONTROLPRM_GAUXFUNC]=mp_mainWindow->getModuleShareMapData()->value(EXTENSION_ADVCONTROLPRM_GAUXFUNC,0);
    qDebug()<<"gSevDrv :"<<map.value(EXTENSION_ADVCONTROLPRM_GSERVDRV);
    qDebug()<<"gAuxFunc :"<<map.value(EXTENSION_ADVCONTROLPRM_GAUXFUNC);;
    ServoControl::writeServoRam(item,m_axisNumber,map,comtype,config->com.rnStation);
    m_highLightInfo.enterFlag=true;
    item->setTextColor(COL_VALUE, QColor(Qt::black));
  }
}
//!-------------------------private function--------------------------
QTreeWidget* AdvRAM::getUiTree()
{
  return ui->treeWidget;
}
QStackedWidget* AdvRAM::getUiStackedWidget()
{
  return ui->stackedWidget;
}
