#include "qmluserconfigproxy_plugin.h"
#include "qmluserconfigproxy.h"

#include <qqml.h>

void QmlUserConfigProxyPlugin::registerTypes(const char *uri)
{
  // @uri com.googoltech.qmlcomponents.QmlUserConfigProxy
  qmlRegisterType<QmlUserConfigProxy>(uri, 1, 0, "QmlUserConfigProxy");
  qmlRegisterType<QmlConfigSetting>(uri, 1, 0, "QmlConfigSetting");
}


