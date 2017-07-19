#include "winpcapmainwindow.h"
#include "ui_winpcapmainwindow.h"

WinPcapMainWindow::WinPcapMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::WinPcapMainWindow)
{
  ui->setupUi(this);
}

WinPcapMainWindow::~WinPcapMainWindow()
{
  delete ui;
}
