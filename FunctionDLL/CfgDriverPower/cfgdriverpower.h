#ifndef CFGDRIVERPOWER_H
#define CFGDRIVERPOWER_H

#include "cfgdriverpower_global.h"
#include "abstractfuncwidget.h"
#include <QObject>

namespace Ui {
class CfgDriverPower;
}
class QTreeWidget;
class QWidget;

class CFGDRIVERPOWERSHARED_EXPORT CfgDriverPower:public AbstractFuncWidget
{
Q_OBJECT
public:
  explicit  CfgDriverPower(QWidget *parent);
  ~CfgDriverPower();
  bool isWrite2Ram()Q_DECL_OVERRIDE {return false;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return true;}

private slots:
  void onPushButtonClicked(void);
  void onQmlValueChanged(void);
private:
  QTreeWidget* getUiTree()Q_DECL_OVERRIDE;
  QStackedWidget *getUiStackedWidget()Q_DECL_OVERRIDE;
  void createUiByQml()Q_DECL_OVERRIDE;

private:
  Ui::CfgDriverPower *ui;
};

#endif // CFGDRIVERPOWER_H
