#include "cfgio.h"
#include "ui_cfgio.h"

CfgIO::CfgIO(QWidget *parent):
  AbstractFuncWidget(parent),
  ui(new Ui::CfgIO())
{
  ui->setupUi(this);
  ui->stackedWidget->setCurrentIndex(0);
}
CfgIO::~CfgIO()
{
  qDebug()<<"CfgIO release ->";
}

//!------------------private function-----------------------------

QTreeWidget* CfgIO::getUiTree()
{
  QStringList headerlist;
  headerlist<<tr("name")<<tr("ID")<<tr("value")<<tr("type")\
              <<tr("axisnum")<<tr("parent")<<tr("unit")<<tr("introduction")\
              <<tr("deaultvalue")<<tr("uplimit")<<tr("downlimit")<<tr("funcnum");
  ui->treeWidget->setHeaderLabels(headerlist);
  ui->treeWidget->setColumnCount(COLUMN_SIZE);
  return ui->treeWidget;
}
QStackedWidget* CfgIO::getUiStackedWidget()
{
  return ui->stackedWidget;
}

