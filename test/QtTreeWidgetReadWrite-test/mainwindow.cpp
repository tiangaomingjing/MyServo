#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../FunctionDLL/PlotWave/QtTreeManager/qttreemanager.h"
#include "useractionwidget.h"
#include "mycombobox.h"
#include "customcurvepage.h"

#include <QTreeWidget>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QWidgetAction>
#include <QDesktopWidget>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

#if TEST_QTTREEWIDGET_RW
  QTreeWidget *treeWidget;
  treeWidget=QtTreeManager::readTreeWidgetFromXmlFile(":/watchcurvesetting.xml");
  treeWidget->show();
  QtTreeManager::writeTreeWidgetToXmlFile("./widget.ui",treeWidget);

  QTreeWidget *treeWidget2;
  treeWidget2=QtTreeManager::readTreeWidgetFromXmlFile(":/FlashPrm_AllAxis.xml");
  treeWidget2->show();
  QtTreeManager::writeTreeWidgetToXmlFile("./flash.ui",treeWidget2);
#endif
  m_popMenu=new QMenu(ui->tableWidget);

  QMenu *menu=new QMenu(ui->toolButton);
  for(int i=0;i<10;i++)
  {
    QAction *act=new QAction(tr("m_%1").arg(i),menu);
    menu->addAction(act);
  }
  ui->toolButton->setMenu(menu);
  ui->toolButton->setPopupMode(QToolButton::MenuButtonPopup);

  ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->tableWidget->setMouseTracking(true);
  connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onTablePopMenu(QPoint)));
//  connect(ui->tableWidget,SIGNAL(cellEntered(int,int)),this,SLOT(onTableCellEntered(int,int)));
//  for(int i=0;i<10;i++)
//  QAction *act=new QAction("test",m_popMenu);

  uWidget=new UserActionWidget();
  connect(uWidget,SIGNAL(actionTriggered(UactionData)),this,SLOT(onActionClicked(UactionData)));

  int virtualWidth = 0;
  int virtualHeight = 0;
  int availableWidth = 0;
  int availableHeight = 0;
  int screenWidth = 0;
  int screenHeight = 0;

  QDesktopWidget *deskWgt = QApplication::desktop();
  if (deskWgt) {
      virtualWidth = deskWgt->width();
      virtualHeight = deskWgt->height();
      qDebug() << "virtual width:" << virtualWidth << ",height:" << virtualHeight ;

      QRect availableRect = deskWgt->availableGeometry();
      availableWidth = availableRect.width();
      availableHeight = availableRect.height();
      qDebug()<< "available width:" <<availableWidth << ",height:" << availableHeight ;

      QRect screenRect = deskWgt->screenGeometry();
      screenWidth = screenRect.width();
      screenHeight = screenRect.height();
      qDebug() << "screen width:" <<screenWidth << ",height:" << screenHeight ;
  }

  QComboBox *comboBox=new QComboBox(ui->tableWidget);
  QStringList list;
  list<<"a"<<"b"<<"c"<<"d";
  comboBox->addItems(list);

  ui->tableWidget->setCellWidget(0,3,comboBox);

  CustomCurvePage *cPage=new CustomCurvePage();
  cPage->show();

  ui->statusBar->showMessage("Hello World ",1000);
  struct Test aa;
  aa.map.insert(0.2,0.5);

  mtestVector.append(aa);

  mtestVector[0].map.insert(0.4,0.8);
  qDebug()<<"length="<<mtestVector.at(0).map.size();
}

MainWindow::~MainWindow()
{
  delete ui;
}
void MainWindow::onTablePopMenu(QPoint pos)
{
  QTableWidgetItem *item=ui->tableWidget->itemAt(pos);

  if(item!=NULL)
  {
    int column=item->column();
//    qDebug()<<"row="<<item->row();
//    qDebug()<<"column="<<item->column();
//    qDebug()<<"text="<<item->text();
    switch (column) {
    case 0:
      for(int i=0;i<10;i++)
      {
        QAction *act=new QAction(tr("Col_0_%1").arg(i),m_popMenu);
        m_popMenu->addAction(act);
        if(i%4==0)
          m_popMenu->addSection("aaaaa");
      }
      m_popMenu->exec(QCursor::pos());
      m_popMenu->clear();
      break;
    case 1:
      for(int i=0;i<20;i++)
      {
        QAction *act=new QAction(tr("Col_1_%1").arg(i),m_popMenu);
        m_popMenu->addAction(act);
      }
      m_popMenu->exec(QCursor::pos());
      m_popMenu->clear();
      break;
    case 2:
//      QWidgetAction *actWgt=new QWidgetAction(m_popMenu);


      uWidget->exec(QCursor::pos(),6);


//      actWgt->setDefaultWidget(uWidget);

//      m_popMenu->addAction(actWgt);
//      m_popMenu->exec(QCursor::pos());


      break;
    }
  }

}

void MainWindow::onTestClicked()
{

}
void MainWindow::onActionClicked(UactionData data)
{
  qDebug()<<"slot data: cmd="<<data.m_cmd<<" axis="<<data.m_axis;
}
void MainWindow::onTableCellEntered(int row, int column)
{
  Q_UNUSED(row);
  if(column==3)
    uWidget->exec(QCursor::pos(),6);
}
