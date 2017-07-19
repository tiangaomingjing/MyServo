#ifndef AXISCLONE_H
#define AXISCLONE_H

#include <QWidget>

namespace Ui {
class AxisClone;
}
class MainWindow;
class AxisClone : public QWidget
{
  Q_OBJECT
private:
  static AxisClone *instance;
private:
  explicit AxisClone(MainWindow *window,QWidget *parent = 0);
public:
  static AxisClone *getAxisCloneInstance(MainWindow *window,QWidget *parent = 0);
  ~AxisClone();
signals:
  void updateProgressBar(int value);
private slots:
  void onBtnSubmitClicked(void);
private:
  void closeEvent(QCloseEvent *event);

private:
  Ui::AxisClone *ui;
  MainWindow *mp_mainWindow;
};

#endif // AXISCLONE_H
