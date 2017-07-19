#ifndef UBOOTDIALOG_H
#define UBOOTDIALOG_H

#include <QDialog>

namespace Ui {
class UbootDialog;
}

class UbootDialog : public QDialog
{
  Q_OBJECT

public:
  explicit UbootDialog(QWidget *parent = 0);
  ~UbootDialog();

  QString hexFilePath() const;

  QString xmlFilePath() const;

private slots:
  void onBtnLineEditHexFilePathClicked();
  void onBtnLineEditXmlFilePathClicked();
  void onBtnApplyClicked();
  void onBtnCancelClicked();

private:
  Ui::UbootDialog *ui;
  QString m_hexFilePath;
  QString m_xmlFilePath;
  quint8 m_recordClickedStatus;//_ _ _ _ | _ _ 1 0 ( 0:位记录选了hex 文件 1:记录选了xml文件)
};

#endif // UBOOTDIALOG_H
