#ifndef NEWCONFIGDIALOG_H
#define NEWCONFIGDIALOG_H

#include <QWidget>
#include "globaldef.h"
namespace Ui {
class NewConfigDialog;
}

class NewConfigDialog : public QWidget
{
  Q_OBJECT
public:
  NewConfigDialog(QWidget *parent = 0);
  explicit NewConfigDialog(SysConfig *srcConfig, QWidget *parent = 0);
  ~NewConfigDialog(void);

signals:
  void newConfig(UserConfig *config);

private slots:
  void onTreeItemClicked(QTreeWidgetItem *item,int column);
  void onBtnCancleClicked(void);
  void onBtnApplyClicked(void);

private:
  void drawPicture(void);
  void setIpInputFormat(void);
private:
  Ui::NewConfigDialog *ui;
  SysConfig *mp_sysConfig;
  UserConfig m_userConfig;
  QString m_strCom;
  QString m_strType;
  quint8 m_recordClick;//记录单击情况0x_ _ _ _|_ _ _ _(tcpip_2 RnNet Ethernet)最高位记录是否选择了版本
};

#endif // NEWCONFIGDIALOG_H
