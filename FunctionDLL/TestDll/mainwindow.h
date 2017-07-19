#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class QTreeWidgetItem;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
private slots:
  void onActionTestClicked(void);
  void onTreeItemClicked(QTreeWidgetItem * item, int column);
  void onItemChanged(QTreeWidgetItem * item, int column);
  void on_pushButton_clicked(void);
  void on_pushButton_add_clicked(void);
private:
  Ui::MainWindow *ui;
  QAction *m_actTest;
};

#endif // MAINWINDOW_H
