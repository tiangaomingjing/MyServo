#ifndef CFGIO_H
#define CFGIO_H

#include "cfgio_global.h"
#include "abstractfuncwidget.h"

namespace Ui {
class CfgIO;
}
class QTreeWidget;
class QWidget;

class CFGIOSHARED_EXPORT CfgIO:public AbstractFuncWidget
{
Q_OBJECT
public:
  CfgIO(QWidget* parent=0);
  ~CfgIO();
  bool isWrite2Ram()Q_DECL_OVERRIDE {return true;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return true;}
private:
  QTreeWidget* getUiTree();
  QStackedWidget *getUiStackedWidget();

private:
  Ui::CfgIO *ui;
};

#endif // CFGIO_H
