#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QAbstractScrollArea>
#include <QScrollBar>
#include <QAbstractSlider>
#include <QDebug>
#include <QDir>
#define BIN_FILE_PATH  QString(QApplication::applicationDirPath().left(QApplication::applicationDirPath().lastIndexOf("/")+1))

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  QAbstractScrollArea *area1=ui->treeWidget;
  QAbstractScrollArea *area2=ui->treeWidget_2;

  QAbstractSlider *sa1=area1->verticalScrollBar();
  QAbstractSlider *sa2=area2->verticalScrollBar();

  connect(sa1,SIGNAL(valueChanged(int)),sa2,SLOT(setValue(int)));
  connect(sa2,SIGNAL(valueChanged(int)),sa1,SLOT(setValue(int)));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::onValueChanged(int value)
{
  qDebug()<<"value="<<value;
  QAbstractScrollArea *area2=ui->treeWidget_2;
  QAbstractSlider *sa2=area2->verticalScrollBar();
  sa2->setValue(value);
}

void MainWindow::on_pushButton_clicked()
{
  QString exe=QApplication::applicationDirPath();
  qDebug()<<"exe="<<exe;
  qDebug()<<"lastindex="<<exe.lastIndexOf("/");
//  qDebug()<<"cut 3"<<exe.remove(exe.lastIndexOf("/")+1,3);
  qDebug()<<"right="<<exe.left(exe.lastIndexOf("/")+1);
  qDebug()<<BIN_FILE_PATH;

  QDir dir(exe);
  dir.cdUp();
  qDebug()<<"-------------------------";
  qDebug()<<"dir.cdUp="<<dir.absolutePath();
  dir.cdUp();
  qDebug()<<"dir.cdUp.up="<<dir.absolutePath();
}
