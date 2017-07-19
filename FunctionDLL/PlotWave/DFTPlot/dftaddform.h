#ifndef DFTADDFORM_H
#define DFTADDFORM_H

#include <QWidget>

namespace Ui {
class DftAddForm;
}
enum DFT_TYPE
    {
      DFT_TYPE_SIGNAL,
      DFT_TYPE_SYSTEM_RESPOND,

      DFT_TYPE_COUNT
    };

class DftAddForm : public QWidget
{
  Q_OBJECT

public:
  explicit DftAddForm(QWidget *parent = 0);
  ~DftAddForm();
signals:
  void doubleSpinBoxDownValueChanged(double value);
  void doubleSpinBoxUpValueChanged(double value);
public slots:
  void onSetDoubleSpinBoxDownValue(double value);
  void onSetDoubleSpinBoxUpValue(double value);
private slots:
  void onRadioSignalClicked(bool checked);
  void onRadioSystemRespondClicked(bool checked);
  void onBtnCancelClicked();
  void onBtnApplyClicked();

private:
  Ui::DftAddForm *ui;
  DFT_TYPE m_dftType;
  bool m_isAccepted;
};

#endif // DFTADDFORM_H
