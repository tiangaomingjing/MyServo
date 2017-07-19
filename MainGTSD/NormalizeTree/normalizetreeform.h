#ifndef NORMALIZETREEFORM_H
#define NORMALIZETREEFORM_H

#include <QDialog>

namespace Ui {
class NormalizeTreeForm;
}

class NormalizeTreeForm : public QDialog
{
  Q_OBJECT

public:
  explicit NormalizeTreeForm(QWidget *parent = 0);
  ~NormalizeTreeForm();
private slots:
  void onBtnFlashRamTreeOpenClicked();
  void onBtnFunctionTreeOpenClicked();
  void onBtnFlashApplyClicked();
  void onBtnFunctionApplyClicked();
  void onBtnExitClicked();

private:
  Ui::NormalizeTreeForm *ui;
};

#endif // NORMALIZETREEFORM_H
