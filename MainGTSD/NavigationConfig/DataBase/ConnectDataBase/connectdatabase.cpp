#include "connectdatabase.h"
#include <QMessageBox>
#include <QDebug>

ConnectDataBase::ConnectDataBase(QObject *parent) : QObject(parent)
{

}
ConnectDataBase::~ConnectDataBase()
{
  if(m_db.isOpen()){
    m_db.close();
  }
  qDebug()<<"ConnectDataBase release ->";
}

bool ConnectDataBase::doConnectBase(const QString &databaseFileName)
{
  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(databaseFileName);
  if (!m_db.open()) {
      QMessageBox::critical(0, tr("Cannot open database"),
          tr("Unable to establish a database connection.\n"
                   "This example needs SQLite support. Please read "
                   "the Qt SQL driver documentation for information how "
                   "to build it.\n\n"
                   "Click Cancel to exit."), QMessageBox::Cancel);
      return false;
  }
  qDebug()<<m_db.driverName();

  return true;
}

bool ConnectDataBase::disConnectBase()
{
  m_db.close();
  return true;
}
