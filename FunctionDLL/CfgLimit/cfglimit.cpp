#include "cfglimit.h"
#include"ui_cfglimit.h"


CfgLimit::CfgLimit(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::CfgLimit())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
}
CfgLimit::~CfgLimit()
{
  qDebug()<<"CfgLimit release ->";
}
//!-------------------------private function--------------------------
QTreeWidget* CfgLimit::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);
  return ui->treeWidget;
}
QStackedWidget* CfgLimit::getUiStackedWidget()
{
  return ui->stackedWidget;
}

