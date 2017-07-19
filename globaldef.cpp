#include "globaldef.h"
namespace GlobalFunction{

int getBytesNumber(QString &str)
{
  int num;
  if(str.contains("16"))
    num=2;
  else if(str.contains("32"))
    num=4;
  else if(str.contains("64"))
    num=8;
  else
    num=2;
  return num;
}

QTreeWidgetItem* findTopLevelItem(QTreeWidgetItem *item)
{
  QTreeWidgetItem* top=item;
  while(top->parent()!=NULL)
  {
    top=top->parent();
  }
  return top;
}

void delayms(quint16 ms)
{
  QTime dieTime = QTime::currentTime().addMSecs(ms);
  while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
QString exeFilePath()
{
  QString path=QApplication::applicationDirPath();
  return path;
}

QString binFilePath()
{
  QString path=QApplication::applicationDirPath();
  QDir dir(path);
  dir.cdUp();
  path=dir.absolutePath()+"/";
  return path;
}

}
