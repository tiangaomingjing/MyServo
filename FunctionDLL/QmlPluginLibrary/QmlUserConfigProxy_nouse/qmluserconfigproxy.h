#ifndef QMLUSERCONFIGPROXY_H
#define QMLUSERCONFIGPROXY_H

#include <QObject>
#include "globaldef.h"
class QmlConfigSetting :public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
  Q_PROPERTY(QString modelName READ modelName WRITE setModelName NOTIFY modelNameChanged)
  Q_PROPERTY(int axisCount READ axisCount WRITE setAxisCount NOTIFY axisCountChanged)
  Q_PROPERTY(QString typeName READ typeName WRITE setTypeName NOTIFY typeNameChanged)
  Q_PROPERTY(int typeId READ typeId WRITE setTypeId NOTIFY typeIdChanged)
public :
  QmlConfigSetting(QObject *parent=0):QObject(parent){}
  ~QmlConfigSetting(){}
  QString version(){return m_version;}
  void setVersion(const QString &ver){m_version=ver;emit versionChanged(m_version);}
  QString modelName(){return m_modelName;}
  void setModelName(const QString &name){m_modelName=name;emit modelNameChanged(m_modelName);}
  int axisCount(){return m_axisCount;}
  void setAxisCount(int count){m_axisCount=count;emit axisCountChanged(m_axisCount);}
  QString typeName(){return m_typeName;}
  void setTypeName(const QString &str){m_typeName=str;emit typeNameChanged(m_typeName);}
  int typeId(){return m_typeId;}
  void setTypeId(int id){m_typeId=id;emit typeIdChanged(m_typeId);}
signals:
  void versionChanged(const QString &version);
  void modelNameChanged(const QString &modelName );
  void axisCountChanged(int count);
  void typeNameChanged(const QString &typeName);
  void typeIdChanged(int typeId);
private:
  QString m_version;
  QString m_modelName;
  int m_axisCount;
  QString m_typeName;
  int m_typeId;
};

class QmlUserConfigProxy : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(QmlUserConfigProxy)
  Q_PROPERTY(QmlConfigSetting* configSetting READ configSetting)

public:
  QmlUserConfigProxy(QObject *parent = 0);
  ~QmlUserConfigProxy();
  QmlConfigSetting* configSetting(){return m_setting;}
  Q_INVOKABLE void setSourceUserConfig(UserConfig *config);
  Q_INVOKABLE void setVersion(QVariant value);
  Q_INVOKABLE void setModelName(QVariant value);
  Q_INVOKABLE void setAxisCount(QVariant value);
  Q_INVOKABLE void setTypeName(QVariant value);
  Q_INVOKABLE void setTypeId(QVariant value);
private:
  UserConfig *mp_userConfig;
  QmlConfigSetting *m_setting;
};

#endif // QMLUSERCONFIGPROXY_H

