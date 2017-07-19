#ifndef TREEMODELLIBRARY_PLUGIN_H
#define TREEMODELLIBRARY_PLUGIN_H

#include <QQmlExtensionPlugin>

class TreeModelLibraryPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
  void registerTypes(const char *uri);
};

#endif // TREEMODELLIBRARY_PLUGIN_H

