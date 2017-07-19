#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <useractionwidget.h>
#define TEST_QTTREEWIDGET_RW 0
#define TEST_QTABLE_MENU 1

namespace Ui {
class MainWindow;
}
class UserActionWidget;
struct Test{
  QMap<double,double>map;
  double t;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
private slots:
  void onTablePopMenu(QPoint pos);
  void onTestClicked(void);
  void onActionClicked(UactionData data);

  void onTableCellEntered(int row,int column);

private:
  Ui::MainWindow *ui;
  QMenu *m_popMenu;
  UserActionWidget *uWidget;
  QVector<Test> mtestVector;
};

#endif // MAINWINDOW_H
