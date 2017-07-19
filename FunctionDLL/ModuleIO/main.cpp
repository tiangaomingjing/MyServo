#include "moduleio.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  ModuleIO w;
  w.show();

  return a.exec();
}
