#include "dftwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  DftWidget w;
  w.show();

  return a.exec();
}
