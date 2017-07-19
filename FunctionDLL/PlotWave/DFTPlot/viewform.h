#ifndef VIEWFORM_H
#define VIEWFORM_H

#include <QWidget>

namespace Ui {
class ViewForm;
}

class ViewForm : public QWidget
{
  Q_OBJECT

public:
  explicit ViewForm(QWidget *parent = 0);
  ~ViewForm();
signals:
  void setPlotViewRange(QVector<double>&vector);
private slots:
  void onBtnApplyClicked();
  void onBtnExitClicked();
private:
  Ui::ViewForm *ui;
};

#endif // VIEWFORM_H
