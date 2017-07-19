#ifndef ABSTRACTFUNCWIDGET_H
#define ABSTRACTFUNCWIDGET_H
#include "abstractfuncwidget_global.h"
#include "globaldef.h"
#include <QObject>
#include <QWidget>
#include <QString>
#include <QTreeWidget>
#include <QDebug>
#define COLUMN_SIZE 12
class QStackedWidget;
class MainWindow;
class QQmlEngine;

class  ABSTRACTFUNCWIDGETSHARED_EXPORT AbstractFuncWidget:public QWidget
{
  Q_OBJECT
public:
  explicit  AbstractFuncWidget(QWidget *parent = 0);
  ~AbstractFuncWidget();
  virtual bool isWrite2Ram() {return false;}
  virtual bool isWrite2Flash(){return false;}

signals:
  void itemValueChanged();//树的值发生改变，发送到QML UI中，让其进行更新操作

public slots:
  virtual void onWriteFuncTreetoServoRam(void);//主程序中的 config 按钮请求(FLASH RAM无关)
  virtual void onWriteFuncTreetoServoFlash(void);//主程序中的 save 按钮请求(FLASH RAM无关)
  virtual void onActionReadFuncValueFromRam(void);//从RAM中读取数据到功能模块树（还可以右键弹出请求）
  virtual void updateUiWhenNavigationTreeClicked(void);

protected slots:
  virtual void onItemExpanded(QTreeWidgetItem*item);
  virtual void onTreeItemClickedEdit(QTreeWidgetItem *item,int column);
  virtual void onItemSelecttionChangedHighlight(void);
  virtual void onEnterKeyPress(void);
  void onActionReadFuncValueFromFlash();//从Flas中读取数据到功能模块树（还可以右键弹出请求）

private slots:
  void onSwitchView(bool sw);

public:
  void readTreeXMLFile(MainWindow *mainWindow, QString fileName_noxml, int axisNum);
  virtual void setTreeWidgetHeaderLables();
  virtual void setActiveNow(bool actived){Q_UNUSED(actived)}//当前的窗口激活显示时，可以增加自己的功能(在速度环，位置环时就需要重绘QML UI)
protected:
  virtual void buildTreeWidget(QString &fileName_noxml);
  virtual void saveTreeXMLFile(QString &fileName);
  virtual void setTreeStyle(QTreeWidget *tree);
  virtual bool eventFilter(QObject *obj, QEvent *eve);
  virtual void setActionReadFuncValueFromFlashEnable();
  virtual void connectionSignalSlotHandler(){ }//为了几个不同的功能模块有关不同的信号与槽连接关系
  virtual void createUiByQml();
  //抽象接口，子类必须实现
  virtual QTreeWidget* getUiTree(void)=0;
  virtual QStackedWidget *getUiStackedWidget(void)=0;

private :
  void setTreeAndStackedWidget(MainWindow *mainWindow, int axisNum);
  void setTreeWidgetParentBackgroundColor();
protected:
  int m_axisNumber;
  MainWindow *mp_mainWindow;
  HighLightInfo m_highLightInfo;
  QTreeWidget *m_uiTree;
  QAction *m_actSwitchView;
  QAction *m_actReadFuncValue;
  QAction *m_actReadFuncValueFromFlash;
  QStackedWidget *m_stackedWidget;

  QString m_qmlFilePath;
  QString m_qmlIconFilePath;

};

#endif // ABSTRACTFUNCWIDGET_H
