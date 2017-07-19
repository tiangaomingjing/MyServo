#include "cfgmoveprofile.h"
#include "ui_cfgmoveprofile.h"


CfgMoveProfile::CfgMoveProfile(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::CfgMoveProfile())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
}
CfgMoveProfile::~CfgMoveProfile()
{
  qDebug()<<"CfgMoveProfile release ->";
}
//!------------------private function-----------------------------

QTreeWidget* CfgMoveProfile::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);
  return ui->treeWidget;
}
QStackedWidget* CfgMoveProfile::getUiStackedWidget()
{
  return ui->stackedWidget;
}

void CfgMoveProfile::setActionReadFuncValueFromFlashEnable()
{
  m_actReadFuncValueFromFlash->setEnabled(true);
}
