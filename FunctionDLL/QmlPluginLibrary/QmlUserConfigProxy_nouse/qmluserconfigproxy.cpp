#include "qmluserconfigproxy.h"

QmlUserConfigProxy::QmlUserConfigProxy(QObject *parent):
  QObject(parent),
  m_setting(new QmlConfigSetting())
{
  // By default, QQuickItem does not draw anything. If you subclass
  // QQuickItem to create a visual item, you will need to uncomment the
  // following line and re-implement updatePaintNode()

  // setFlag(ItemHasContents, true);
}

QmlUserConfigProxy::~QmlUserConfigProxy()
{
}

void QmlUserConfigProxy::setSourceUserConfig(UserConfig *config)
{
  mp_userConfig=config;
}

void QmlUserConfigProxy::setVersion(QVariant value)
{
  mp_userConfig->model.version.clear();
  mp_userConfig->model.version.append(value.toString());
  m_setting->setVersion(value.toString());
}

void QmlUserConfigProxy::setModelName(QVariant value)
{
  mp_userConfig->model.modelName=value.toString();
  m_setting->setModelName(mp_userConfig->model.modelName);
}

void QmlUserConfigProxy::setAxisCount(QVariant value)
{
  mp_userConfig->model.axisCount=value.toInt();
  m_setting->setAxisCount(value.toInt());
}

void QmlUserConfigProxy::setTypeName(QVariant value)
{
  mp_userConfig->typeName=value.toString();
  m_setting->setTypeName(value.toString());
}

void QmlUserConfigProxy::setTypeId(QVariant value)
{
  mp_userConfig->typeId=value.toInt();
  m_setting->setTypeId(value.toInt());
}
