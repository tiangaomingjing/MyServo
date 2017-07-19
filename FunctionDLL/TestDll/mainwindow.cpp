#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QWidget>
#include <QDir>
#include <QDebug>
#include <QLabel>
#include <globaldef.h>
#include "factory/factory.h"
#include "cfgdriverpower.h"
#include "abstractfuncwidget.h"
#include "cfgmotor.h"
#include "cfgencoder.h"
#include "XmlBuilder/xmlbuilder.h"
#include <QRegExp>
#include <QRegExpValidator>
#include "MainGTSD/FpgaDialogSetting/inputvalidator.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  m_actTest=new QAction(this);
  m_actTest->setText("test");
  connect(m_actTest,SIGNAL(triggered(bool)),this,SLOT(onActionTestClicked()));
  ui->mainToolBar->addAction(m_actTest);
  connect(ui->treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeItemClicked(QTreeWidgetItem*,int)));
  connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(onItemChanged(QTreeWidgetItem*,int)));
//  ui->treeWidget->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
}

MainWindow::~MainWindow()
{
  delete ui;
}
void MainWindow::onActionTestClicked()
{

}
void MainWindow::onTreeItemClicked(QTreeWidgetItem *item, int column)
{
  if(column==0)
  {
    item->setFlags(item->flags()|Qt::ItemIsEditable);
  }
  else item->setFlags(item->flags()&(~Qt::ItemIsEditable));
}
void MainWindow::onItemChanged(QTreeWidgetItem *item, int column)
{
  qDebug()<<item->text(0)<<column;
}
void MainWindow::on_pushButton_clicked()
{
  /*正则表达输入验证测试
  qDebug()<<"clicked....";
  QString text=ui->lineEdit_2->text();
  QString out("out true");
//  QRegExp rx("^0[xX][0-9a-fA-F]{1,4}$");
//  QRegExpValidator v(rx,0);
//  int pos=0;
//  if(v.validate(text,pos)!=QValidator::Acceptable)
//  {
//    qDebug()<<tr("pos:%1").arg(pos);
//  }
//  QString text2=ui->lineEdit_5->text();
//  qDebug()<<"value:"<<text2.toInt();
  InputValidator v(text,ui->lineEdit_5->text().toInt(),false,InputValidator::DecType);
  if(v.validate()==false)
  {
    out="out false";
  }
  qDebug()<<out;
  */
  QWidget *widget=NULL;
  while (ui->stackedWidget->count()>0)
  {
    widget=ui->stackedWidget->currentWidget();
    ui->stackedWidget->removeWidget(widget);
    delete widget;
  }
}
void MainWindow::on_pushButton_add_clicked()
{
  QLabel *label;
  for(int i=0;i<4;i++)
  {
    label=new QLabel(tr("label %1").arg(QString::number(i)),0);
    ui->stackedWidget->addWidget(label);
  }
  qDebug()<<tr("stacked count:%1").arg(ui->stackedWidget->count());
}
