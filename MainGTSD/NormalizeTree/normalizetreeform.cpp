#include "normalizetreeform.h"
#include "ui_normalizetreeform.h"
#include "xmlbuilder.h"
#include "mainwindow.h"

#include <QFileDialog>

NormalizeTreeForm::NormalizeTreeForm(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::NormalizeTreeForm)
{
  ui->setupUi(this);
//  setAttribute(Qt::WA_DeleteOnClose,true);
  setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
  connect(ui->btn_exit,SIGNAL(clicked(bool)),this,SLOT(onBtnExitClicked()));
  connect(ui->btn_flashApply,SIGNAL(clicked(bool)),this,SLOT(onBtnFlashApplyClicked()));
  connect(ui->btn_flashRamTreeOpen,SIGNAL(clicked(bool)),this,SLOT(onBtnFlashRamTreeOpenClicked()));
  connect(ui->btn_functionApply,SIGNAL(clicked(bool)),this,SLOT(onBtnFunctionApplyClicked()));
  connect(ui->btn_functionTreeOpen,SIGNAL(clicked(bool)),this,SLOT(onBtnFunctionTreeOpenClicked()));
}

NormalizeTreeForm::~NormalizeTreeForm()
{
  delete ui;
  qDebug()<<"normalize release ->";
}
void NormalizeTreeForm::onBtnFlashRamTreeOpenClicked()
{
  QString path = QFileDialog::getOpenFileName(this, tr("Open XML file"), ".", tr("Xml Files( *.xml)"));
  if(!path.isEmpty())
  {
    ui->lineEdit_flashRam->setText(path);
  }
}

void NormalizeTreeForm::onBtnFunctionTreeOpenClicked()
{
  QString path = QFileDialog::getOpenFileName(this, tr("Open XML file"), ".", tr("Xml Files( *.xml)"));
  if(!path.isEmpty())
  {
    ui->lineEdit_function->setText(path);
  }
}

void NormalizeTreeForm::onBtnFlashApplyClicked()
{
  QFileInfo fileInfo;
  QString saveFileName;
  QString strDir;

  fileInfo.setFile(ui->lineEdit_flashRam->text());
  strDir=fileInfo.fileName();
  saveFileName = QFileDialog::getSaveFileName(this,tr("SaveXML"), MainWindow::g_lastFilePath+strDir,tr("configXML (*.xml)"));

  if (!saveFileName.isNull())//空表示点的取消按钮
  {
    qDebug()<<strDir<<"\n"<<saveFileName;
    fileInfo.setFile(saveFileName);
    MainWindow::g_lastFilePath=fileInfo.filePath()+"/";
    QTreeWidget *tree=XmlBuilder::readTreeWidgetFromFile(ui->lineEdit_flashRam->text());
    XmlBuilder::ramFlashTreeWidgetNormalization(tree);
    XmlBuilder::saveFlashAllAxisTreeFile(saveFileName,tree);
    tree->clear();
    delete tree;
  }
}

void NormalizeTreeForm::onBtnFunctionApplyClicked()
{
  QFileInfo fileInfo;
  QString saveFileName;
  QString strDir;

  fileInfo.setFile(ui->lineEdit_function->text());
  strDir=fileInfo.fileName();
  saveFileName = QFileDialog::getSaveFileName(this,tr("SaveXML"), strDir,tr("configXML (*.xml)"));

  if (!saveFileName.isNull())//空表示点的取消按钮
  {
    qDebug()<<strDir<<"\n"<<saveFileName;
    fileInfo.setFile(saveFileName);
    QTreeWidget *tree=XmlBuilder::readTreeWidgetFromFile(ui->lineEdit_function->text());
    XmlBuilder::functionTreeWidgetNormalization(tree);
    XmlBuilder::saveFunctionTreeFile(saveFileName,tree);
    tree->clear();
    delete tree;
  }
}

void NormalizeTreeForm::onBtnExitClicked()
{
  close();
}
