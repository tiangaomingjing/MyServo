#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>
#include <QTimer>

#define DEVICE_DESCRIBE "device"
#define DSP_NAME "dsp"
#define FPGA_NAME "fpga"

namespace Ui {
class DownloadDialog;
}
class MainWindow;

class DownloadDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DownloadDialog(MainWindow *mainwindow,QWidget *parent = 0);
  ~DownloadDialog();

  static void delayms(quint16 ms);

private slots:
  void onBtnDspFileOpenClicked();
  void onBtnFpgaFileOpenClicked();
  void onBtnDspDownloadClicked();
  void onBtnFpgaDownloadClicked();
  void onBtnExitClicked();
  void onBtnResetDSPClicked();
private:

  static void updateProgressValueDSP(void *arg,qint16 *value);//回调函数更新进度条
  static void updateProgressValueFPGA(void *arg,qint16 *value);
  void enableUi(bool state);

private:
  Ui::DownloadDialog *ui;
  MainWindow *mp_mainwindow;
  static int m_dspProgressValue;
  static int m_fpgaProgressValue;
  static int m_dspNum;
  static int m_fpgaNum;
  static int m_currentStep;
};

#endif // DOWNLOADDIALOG_H
