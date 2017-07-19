#ifndef QMLSERVOCMD_PLUGIN_H
#define QMLSERVOCMD_PLUGIN_H

#include <QQmlExtensionPlugin>

class QmlServoCmdPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
  void registerTypes(const char *uri);
};

#endif // QMLSERVOCMD_PLUGIN_H

