#include "qmlservocmd_plugin.h"
#include "servocmd.h"
#include <QTreeWidget>

#include <qqml.h>

void QmlServoCmdPlugin::registerTypes(const char *uri)
{
  // @uri com.googoltech.qmlcomponents
  qmlRegisterType<ServoCmd>(uri, 1, 0, "ServoCmd");
  qmlRegisterType<QTreeWidget>(uri, 1, 0, "QTreeWidget");
}


