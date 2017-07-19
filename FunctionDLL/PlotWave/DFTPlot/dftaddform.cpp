#include "dftaddform.h"
#include "ui_dftaddform.h"

#include <QDebug>

DftAddForm::DftAddForm(QWidget *parent) :
  QWidget(parent),
  m_isAccepted(false),
  m_dftType(DFT_TYPE_SIGNAL),
  ui(new Ui::DftAddForm)
{
  ui->setupUi(this);
  //没有最大化，最小化
  setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
  setWindowTitle(tr("Add FFT Curve"));
  ui->comboBox_output->hide();
  ui->label_output->hide();

  connect(ui->radioButton_signal,SIGNAL(toggled(bool)),this,SLOT(onRadioSignalClicked(bool)));
  connect(ui->radioButton_system,SIGNAL(toggled(bool)),this,SLOT(onRadioSystemRespondClicked(bool)));
  connect(ui->btn_apply,SIGNAL(clicked(bool)),this,SLOT(onBtnApplyClicked()));
  connect(ui->btn_cancel,SIGNAL(clicked(bool)),this,SLOT(onBtnCancelClicked()));
  connect(ui->doubleSpinBox_down,SIGNAL(valueChanged(double)),this,SIGNAL(doubleSpinBoxDownValueChanged(double)));
  connect(ui->doubleSpinBox_up,SIGNAL(valueChanged(double)),this,SIGNAL(doubleSpinBoxUpValueChanged(double)));
}

DftAddForm::~DftAddForm()
{
  delete ui;
  qDebug()<<"DftAddForm release ->";
}
//-------------public slots functions---------------
void DftAddForm::onSetDoubleSpinBoxDownValue(double value)
{
  ui->doubleSpinBox_down->setValue(value);
}

void DftAddForm::onSetDoubleSpinBoxUpValue(double value)
{
    ui->doubleSpinBox_up->setValue(value);
}

//--------------private slots functions-------------
void DftAddForm::onRadioSignalClicked(bool checked)
{
  if(checked)
  {
    ui->comboBox_output->hide();
    ui->label_output->hide();
    m_dftType=DFT_TYPE_SIGNAL;
  }
  qDebug()<<"dft type:"<<m_dftType;
}

void DftAddForm::onRadioSystemRespondClicked(bool checked)
{
  if(checked)
  {
    ui->comboBox_output->show();
    ui->label_output->show();
    m_dftType=DFT_TYPE_SYSTEM_RESPOND;
  }
  qDebug()<<"dft type:"<<m_dftType;
}
void DftAddForm::onBtnCancelClicked()
{
  m_isAccepted=false;
  hide();
}

void DftAddForm::onBtnApplyClicked()
{
  m_isAccepted=true;
  hide();
}
