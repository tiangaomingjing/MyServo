#include "DlgSetXYRange.h"
#include "ui_DlgSetXYRange.h"
#include <QDebug>

DlgSetXYRange::DlgSetXYRange(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSetXYRange())
{
  ui->setupUi(this);
  ui->doubleSpinBox_storeTimes->setValue(10);
  ui->doubleSpinBox_storeTimes->setToolTip(tr("max store times :10s"));
  ui->doubleSpinBox_xTimes->setValue(5);
  ui->doubleSpinBox_xTimes->setToolTip(tr("max display time:10s"));
  ui->spinBox_points->setValue(1000);
  ui->spinBox_points->setToolTip(tr("max points:1000"));
  ui->doubleSpinBox_yMinValue->setValue(-500);
  ui->doubleSpinBox_yMaxValue->setValue(5000);
}
void DlgSetXYRange::accept()
{
  m_setting.pointNum=ui->spinBox_points->value();
  m_setting.storeTime=ui->doubleSpinBox_storeTimes->value();
  m_setting.xtime=ui->doubleSpinBox_xTimes->value();
  m_setting.ymax=ui->doubleSpinBox_yMaxValue->value();
  m_setting.ymin=ui->doubleSpinBox_yMinValue->value();
  emit viewSetting(m_setting);
  qDebug()<<"accept";
  QDialog::accept();
}

DlgSetXYRange::~DlgSetXYRange()
{
  qDebug()<<"dlgsetxyrange dialog release ->";
    delete ui;
}
