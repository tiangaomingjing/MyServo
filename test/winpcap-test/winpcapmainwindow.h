#ifndef WINPCAPMAINWINDOW_H
#define WINPCAPMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class WinPcapMainWindow;
}

class WinPcapMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit WinPcapMainWindow(QWidget *parent = 0);
  ~WinPcapMainWindow();

private:
  Ui::WinPcapMainWindow *ui;
};

#endif // WINPCAPMAINWINDOW_H
