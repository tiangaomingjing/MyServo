#ifndef CFGLIMIT_H
#define CFGLIMIT_H

#include "cfglimit_global.h"
#include "abstractfuncwidget.h"
namespace Ui {
class CfgLimit;
}
class QTreeWidget;
class QWidget;

class CFGLIMITSHARED_EXPORT CfgLimit:public AbstractFuncWidget
{
Q_OBJECT
public:
  CfgLimit(QWidget *parent=0);
  ~CfgLimit();
  bool isWrite2Ram()Q_DECL_OVERRIDE {return true;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return true;}
private:
  QTreeWidget* getUiTree();
  QStackedWidget *getUiStackedWidget();

private:
  Ui::CfgLimit *ui;
};

#endif // CFGLIMIT_H
