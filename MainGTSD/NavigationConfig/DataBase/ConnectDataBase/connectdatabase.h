#ifndef CONNECTDATABASE_H
#define CONNECTDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#define DB_MotorInfo "MotorInfomation.sqlite"

class ConnectDataBase : public QObject
{
  Q_OBJECT
public:
  explicit ConnectDataBase(QObject *parent = 0);
  ~ConnectDataBase();
  bool doConnectBase(const QString &databaseFileName);
  QSqlDatabase dataBase(){return m_db;}
  bool disConnectBase();
signals:

public slots:
private:
   QSqlDatabase m_db;
};

#endif // CONNECTDATABASE_H
