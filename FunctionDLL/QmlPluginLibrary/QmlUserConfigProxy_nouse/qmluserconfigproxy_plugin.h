#ifndef QMLUSERCONFIGPROXY_PLUGIN_H
#define QMLUSERCONFIGPROXY_PLUGIN_H

#include <QQmlExtensionPlugin>

class QmlUserConfigProxyPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
  void registerTypes(const char *uri);
};

#endif // QMLUSERCONFIGPROXY_PLUGIN_H

