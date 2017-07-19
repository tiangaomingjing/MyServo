#include "treemodellibrary_plugin.h"
#include "treemodel.h"

#include <qqml.h>

void TreeModelLibraryPlugin::registerTypes(const char *uri)
{
  // @uri com.googoltech.qmlcomponents.TreeModel
  qmlRegisterType<TreeModel>(uri, 1, 0, "TreeModel");
//  qmlRegisterType<TreeItemProxy>(uri, 1, 0, "TreeItemProxy");
}


