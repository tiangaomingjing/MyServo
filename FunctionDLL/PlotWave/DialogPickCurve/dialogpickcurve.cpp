#include "dialogpickcurve.h"
#include "ui_dialogpickcurve.h"
#include "plotwave.h"
#include "MainGTSD/mainwindow.h"
#include "usrcurvetreemanager.h"
#include "customcurvepage.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <QButtonGroup>
#include <QPushButton>
#include <QComboBox>
#include <QMenu>
#define MAX_ROW_CURVE_REMAIN 10

typedef enum{
  COL_TABLE_CURVE_NAME,
  COL_TABLE_CURVE_DESCRIPTION,
  COL_TABLE_CURVE_FACTOR
}ColumnTableCurve;


DialogPickCurve::DialogPickCurve(PlotWave *plotWidget, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogPickCurve)
{
  ui->setupUi(this);
  mp_plotWave=plotWidget;
  mp_mainWindow=mp_plotWave->getMainWindow();
  mp_usrCurveTreeManager=mp_plotWave->getUsrCurveTreeManager();

  initailUi();

  //-------------------user -----------------------------------------------------
  QButtonGroup *group=new QButtonGroup(this);
  group->addButton(ui->btn_user);
  group->addButton(ui->btn_expert);
  group->addButton(ui->btn_custom);
  ui->btn_user->setChecked(true);
  ui->stackedWidget->setCurrentIndex(0);
  connect(ui->btn_user,SIGNAL(clicked(bool)),this,SLOT(onBtnUserRoleClicked()));
  connect(ui->btn_expert,SIGNAL(clicked(bool)),this,SLOT(onBtnUserRoleClicked()));
  connect(ui->btn_custom,SIGNAL(clicked(bool)),this,SLOT(onBtnUserRoleClicked()));
  connect(ui->tableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(onUsrTableCellDoubleClicked(int,int)));
  connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onTablePopMenu(QPoint)));

  //--------------------expert----------------------------------------------------
  connect(ui->treeWidgetExpert,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(onTreeItemExpanded(QTreeWidgetItem*)));
  connect(ui->treeWidgetExpert,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SIGNAL(expertTreeItemDoubleClicked(QTreeWidgetItem*,int)));

  //---------------------custom------------------------
  connect(m_customCurvePageUi,SIGNAL(resizeGeometry(bool)),this,SLOT(onCustomPageResizeGeometry(bool)));
  connect(m_customCurvePageUi,SIGNAL(addScriptCurve(QTreeWidgetItem*)),this,SLOT(onAddScriptCurve(QTreeWidgetItem*)));

}

DialogPickCurve::~DialogPickCurve()
{
  delete ui;
}
QTableWidget *DialogPickCurve::getSelectAxisTableWidget(void) const
{
  return ui->tableWidgetAxis;
}

void DialogPickCurve::onBtnUserRoleClicked()
{
  QToolButton *btn=qobject_cast<QToolButton *>(sender());
  if(btn->objectName()=="btn_user")
    ui->stackedWidget->setCurrentIndex(0);
  else if(btn->objectName()=="btn_expert")
    ui->stackedWidget->setCurrentIndex(1);
  else if(btn->objectName()=="btn_custom")
    ui->stackedWidget->setCurrentIndex(2);
}
void DialogPickCurve::onTreeItemExpanded(QTreeWidgetItem *item)
{
  Q_UNUSED(item);
  ui->treeWidgetExpert->resizeColumnToContents(0);
}
void DialogPickCurve::onUsrTableCellDoubleClicked(int row, int column)
{
  Q_UNUSED(column);
  QComboBox *box;
  box=qobject_cast<QComboBox *>(ui->tableWidget->cellWidget(row,COL_TABLE_CURVE_FACTOR));
  emit usrTableCellDoubleClicked(row,ui->tableWidgetAxis,box);
}

void DialogPickCurve::onTablePopMenu(QPoint pos)
{
  QTableWidgetItem *item=ui->tableWidget->itemAt(pos);

  if(item!=NULL)
  {
    int column=item->column();
    int row=item->row();
    qDebug()<<"popu: row="<<row<<" column="<<column;
    if(row>=MAX_ROW_CURVE_REMAIN)
    {
      switch (column)
      {
      case 0:
        QAction *act=new QAction(tr("remove curve"),m_popuMenuTable);
        act->setData(row);
        m_popuMenuTable->addAction(act);
        connect(act,SIGNAL(triggered(bool)),this,SLOT(onTablePopActionClicked()));
        m_popuMenuTable->exec(QCursor::pos());
        m_popuMenuTable->clear();
        break;
      }
    }
  }
}
void DialogPickCurve::onTablePopActionClicked()
{

  QAction *act=qobject_cast<QAction *>(sender());
  int index=act->data().toInt();
  qDebug()<<"delete row="<<index;
  bool ret=mp_plotWave->getUsrCurveTreeManager()->removeTreeCurve(index);
  if(ret)
  {
//    mp_plotWave->getUsrCurveTreeManager()->usrTreeTemplate()->show();
//    for(int i=0;i<mp_plotWave->getUsrCurveTreeManager()->usrTreeList().count();i++)
//    {
//      mp_plotWave->getUsrCurveTreeManager()->usrTreeList()[i]->setColumnCount(3);
//      mp_plotWave->getUsrCurveTreeManager()->usrTreeList()[i]->show();
//    }
    //清除界面对应的行
    ui->tableWidget->removeRow(index);
    emit deleteScriptCurveAtId(index);
  }
}

void DialogPickCurve::onCustomPageResizeGeometry(bool isShow)
{
  QRect rect=this->geometry();
  if(isShow)
  {
    this->setGeometry(rect.x(),rect.y(),rect.width()*2,rect.height());
  }
  else
  {
    this->setGeometry(rect.x(),rect.y(),rect.width()/2,rect.height());
  }
}
void DialogPickCurve::onAddScriptCurve(QTreeWidgetItem *item)
{
  qDebug()<<"add script curve";
  //先修改它的BaseAddr
  QTreeWidgetItem* prmFactor=item->child(ROW_PRM_INDEX_PRMFACTOR);
  QTreeWidgetItem* factor;
  QString name;
  CurveType curveType=(CurveType)(item->text(COL_CURVESETTING_INDEX_VALUE).toInt());
  if(curveType==CURVE_TYPE_SCRIPT)
  {
    for(int i=0;i<prmFactor->childCount();i++)
    {
      factor=prmFactor->child(i);
      name=factor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
      quint16 baseAddr=mp_plotWave->getCurveBaseAddress(name);
      QString bstr=QString::number(baseAddr);
      factor->child(ROW_FACTOR_INDEX_BASEADDR)->setText(COL_CURVESETTING_INDEX_VALUE,bstr);
      qDebug()<<name<<"="<<bstr;
    }
  }
  else
  {
    factor=prmFactor->child(0);
    name=factor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
    quint16 baseAddr=mp_plotWave->getCurveBaseAddress(name);
    QString bstr=QString::number(baseAddr);
    factor->child(ROW_FACTOR_INDEX_BASEADDR)->setText(COL_CURVESETTING_INDEX_VALUE,bstr);
    qDebug()<<name<<"="<<bstr;
  }
  mp_usrCurveTreeManager->addTreeCurve(item);
  //界面显示增加
  int curveCount=ui->tableWidget->rowCount();
  addUserTableCurveRow(curveCount, item);
}

void DialogPickCurve::addUserTableCurveRow(int i, QTreeWidgetItem *itemCurveRow)
{
  QTreeWidgetItem *item;
  QTableWidgetItem *tableItem;
  QStringList strList;
  item=itemCurveRow->child(ROW_PRM_INDEX_FULLNAME);
  strList=item->text(COL_CURVESETTING_INDEX_VALUE).split(".");
  qDebug()<<strList;
  ui->tableWidget->insertRow(i);

  tableItem=new QTableWidgetItem;
  tableItem->setText(strList.last());
  tableItem->setIcon(QIcon(":/user_curve_icon.png"));
  tableItem->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  ui->tableWidget->setItem(i,0,tableItem);

  tableItem=new QTableWidgetItem;
  tableItem->setText(strList.first());
  tableItem->setTextAlignment(Qt::AlignCenter);
  ui->tableWidget->setItem(i,1,tableItem);

  //设置下拉单位
  QComboBox *comboBox;
  QString title;
  double unitFactor;
  item=itemCurveRow->child(ROW_PRM_INDEX_MULTIVALUE);
  comboBox=new QComboBox;
  for(int j=0;j<item->childCount();j++)
  {
    title=item->child(j)->text(COL_CURVESETTING_INDEX_KEY);
    unitFactor=item->child(j)->text(COL_CURVESETTING_INDEX_VALUE).toDouble();
    comboBox->addItem(title,unitFactor);
  }
  ui->tableWidget->setCellWidget(i,2,comboBox);
}

void DialogPickCurve::initailUi()
{
  //---------------------------user tab------------------------------
  QTreeWidgetItem *itemCurveRow;
  int topLevelCount=mp_usrCurveTreeManager->usrTreeTemplate()->topLevelItemCount();
  qDebug()<<"topLevelCount "<<topLevelCount;
  for(int i=0;i<topLevelCount;i++)
  {
    itemCurveRow=mp_usrCurveTreeManager->usrTreeTemplate()->topLevelItem(i);
    addUserTableCurveRow(i, itemCurveRow);
  }
  ui->tableWidget->setEditTriggers(QTableView::NoEditTriggers);
  ui->tableWidget->setAlternatingRowColors(true);
  ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选中的方式
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  //设置右键菜单
  ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  m_popuMenuTable=new QMenu(this);
  //各个轴号的选择表
  QTableWidgetItem *tableItem;
  int axisCount=mp_mainWindow->getUserConfig()->model.axisCount;
  ui->tableWidgetAxis->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableWidgetAxis->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableWidgetAxis->setRowCount(1);
  ui->tableWidgetAxis->setColumnCount(axisCount);
  ui->tableWidgetAxis->setEditTriggers(QTableView::NoEditTriggers);
  ui->tableWidgetAxis->setSelectionMode(QAbstractItemView::ExtendedSelection);
  for(int i=0;i<axisCount;i++)
  {
    tableItem=new QTableWidgetItem;
    tableItem->setText(tr("Axis_%1").arg(i));
//    tableItem->setIcon(QIcon(":/user_curve_axis.png"));
    tableItem->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetAxis->setItem(0,i,tableItem);
  }
  ui->tableWidgetAxis->item(0,0)->setSelected(true);


  //-------------------------expert tab-----------------------------------------
  QTreeWidgetItem *item;
  int count=mp_mainWindow->getRamAllAxisTree()->topLevelItemCount();
  qDebug()<<"expert tab count="<<count;
  ui->treeWidgetExpert->setColumnCount(10);
  for(int i=0;i<count;i++)
  {
    item=mp_mainWindow->getRamAllAxisTree()->topLevelItem(i)->clone();
    ui->treeWidgetExpert->addTopLevelItem(item);
  }
  QTreeWidgetItem *itemHeader;
  itemHeader=mp_mainWindow->getRamAllAxisTree()->headerItem()->clone();
  ui->treeWidgetExpert->setHeaderItem(itemHeader);
  ui->treeWidgetExpert->hideColumn(1);
  ui->treeWidgetExpert->hideColumn(3);
  ui->treeWidgetExpert->hideColumn(4);
  ui->treeWidgetExpert->hideColumn(5);
  ui->treeWidgetExpert->hideColumn(6);
  ui->treeWidgetExpert->hideColumn(7);
  ui->treeWidgetExpert->hideColumn(8);
  ui->treeWidgetExpert->hideColumn(9);
  ui->treeWidgetExpert->hideColumn(10);
  ui->treeWidgetExpert->expandToDepth(1);
  ui->treeWidgetExpert->resizeColumnToContents(0);
  ui->treeWidgetExpert->resizeColumnToContents(1);
  ui->treeWidgetExpert->resizeColumnToContents(2);
  ui->treeWidgetExpert->resizeColumnToContents(3);


  //---------------------custom tab------------------------------------------------
  m_customCurvePageUi=new CustomCurvePage(this);
  ui->verticalLayout_custom->addWidget(m_customCurvePageUi);
  QTreeWidgetItem *itemFirstAxis=mp_mainWindow->getRamAllAxisTree()->topLevelItem(0);
  m_customCurvePageUi->setCurveTreeRAM(itemFirstAxis);
}
