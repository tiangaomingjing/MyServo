#ifndef CFGMOVEPROFILE_H
#define CFGMOVEPROFILE_H

#include "cfgmoveprofile_global.h"
#include "abstractfuncwidget.h"

namespace Ui {
class CfgMoveProfile;
}
class QTreeWidget;
class QWidget;

class CFGMOVEPROFILESHARED_EXPORT CfgMoveProfile:public AbstractFuncWidget
{
Q_OBJECT
public:
  CfgMoveProfile(QWidget*parent=0);
  ~CfgMoveProfile();
  bool isWrite2Ram()Q_DECL_OVERRIDE {return true;}
  bool isWrite2Flash()Q_DECL_OVERRIDE {return true;}

private:
  QTreeWidget* getUiTree();
  QStackedWidget *getUiStackedWidget();
  void setActionReadFuncValueFromFlashEnable() Q_DECL_OVERRIDE;

private:
  Ui::CfgMoveProfile *ui;
};

#endif // CFGMOVEPROFILE_H
