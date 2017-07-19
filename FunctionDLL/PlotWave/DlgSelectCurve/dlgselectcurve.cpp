#include "dlgselectcurve.h"
#include "ui_dlgselectcurve.h"
#include <QDebug>
#include <QTreeWidgetItem>

DlgSelectCurve::DlgSelectCurve(QTreeWidget *srcTree, QWidget *parent):
  QDialog(parent),
  mp_tree(srcTree),
  ui(new Ui::DlgSelectCurve())
{
  ui->setupUi(this);
  initialUi();
  connect(ui->btn_exit,SIGNAL(clicked(bool)),this,SLOT(onBtnExitClicked()));
  connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SIGNAL(treeItemDoubleClicked(QTreeWidgetItem*,int)));
  connect(ui->treeWidget,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(onTreeItemExpanded(QTreeWidgetItem*)));
}
DlgSelectCurve::~DlgSelectCurve()
{
  ui->treeWidget->clear();
  qDebug()<<"release Dialog ->";

}

void DlgSelectCurve::onBtnExitClicked()
{
  this->close();
}

void DlgSelectCurve::onTreeItemExpanded(QTreeWidgetItem *item)
{
  Q_UNUSED(item);
  ui->treeWidget->resizeColumnToContents(0);
}

void DlgSelectCurve::initialUi(void)
{
  QTreeWidgetItem *item;
  int count=mp_tree->topLevelItemCount();
  ui->treeWidget->setColumnCount(count);
//  QStringList list;
//  list<<tr("name")<<tr("value")<<tr("type")<<tr("offset");
//  ui->treeWidget->setHeaderLabels(list);
  for(int i=0;i<count;i++)
  {
    item=mp_tree->topLevelItem(i)->clone();
    ui->treeWidget->addTopLevelItem(item);
  }
  QTreeWidgetItem *itemHeader;
  itemHeader=mp_tree->headerItem()->clone();
  ui->treeWidget->setHeaderItem(itemHeader);
  ui->treeWidget->hideColumn(1);
  ui->treeWidget->hideColumn(3);
  ui->treeWidget->hideColumn(4);
  ui->treeWidget->hideColumn(5);
  ui->treeWidget->hideColumn(6);
  ui->treeWidget->hideColumn(7);
  ui->treeWidget->hideColumn(8);
  ui->treeWidget->hideColumn(9);
  ui->treeWidget->hideColumn(10);

  ui->treeWidget->expandToDepth(1);
  ui->treeWidget->resizeColumnToContents(0);
  ui->treeWidget->resizeColumnToContents(1);
  ui->treeWidget->resizeColumnToContents(2);
  ui->treeWidget->resizeColumnToContents(3);
  ui->btn_exit->setText(tr("exit"));

  setTreeStyle();

}
void DlgSelectCurve::setTreeStyle()
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
