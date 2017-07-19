#include "viewform.h"
#include "ui_viewform.h"

ViewForm::ViewForm(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ViewForm)
{
  ui->setupUi(this);
  setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
  setWindowTitle(tr("viewform setting"));

  connect(ui->btn_apply,SIGNAL(clicked(bool)),this,SLOT(onBtnApplyClicked()));
  connect(ui->btn_exit,SIGNAL(clicked(bool)),this,SLOT(onBtnExitClicked()));
}

ViewForm::~ViewForm()
{
  delete ui;
}

//-----------------------------slots-------------------
void ViewForm::onBtnApplyClicked()
{
  QVector<double>settings;
  settings.append(ui->dSpinBox_amXmin->value());
  settings.append(ui->dSpinBox_amXmax->value());
  settings.append(ui->dSpinBox_amYmin->value());
  settings.append(ui->dSpinBox_amYmax->value());
  settings.append(ui->dSpinBox_phXmin->value());
  settings.append(ui->dSpinBox_phXmax->value());
  settings.append(ui->dSpinBox_phYmin->value());
  settings.append(ui->dSpinBox_phYmax->value());
  emit setPlotViewRange(settings);
  deleteLater();
}
void ViewForm::onBtnExitClicked()
{
  deleteLater();
}
