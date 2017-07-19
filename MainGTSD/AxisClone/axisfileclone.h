#ifndef AXISFILECLONE_H
#define AXISFILECLONE_H

#include <QWidget>
namespace Ui {
class AxisFileClone;
}
class QGraphicsScene;
class AxisItem;
class Arrow;
class QButtonGroup;
class AxisBackground;
class MainWindow;
class QTreeWidget;

class AxisFileClone : public QWidget
{
  Q_OBJECT
public:
  explicit AxisFileClone(MainWindow *mainwindow,QWidget *parent=0);
  ~AxisFileClone();

signals:

private slots:
  void onGroupSrcClicked(int id);
  void onGroupDstClicked(int id);
  void onArrowDoubleClicked();

  void onBtnExitClicked();
  void onBtnOpenClicked();
  void onBtnApplyClicked();
private:
  void setMyStyle();
private:
  Ui::AxisFileClone *ui;
  MainWindow *mp_mainWindow;
  QGraphicsScene *m_scene;
  QList<AxisItem*>m_axisItemSourceList;
  QList<AxisItem*>m_axisItemDestinationList;
  QList<Arrow *>m_arrowList;
  QButtonGroup *m_groupSrc;
  QButtonGroup *m_groupDst;

  AxisItem* m_srcItem;
  AxisItem* m_dstItem;
  AxisBackground *m_abackground;
  AxisBackground *m_abackground2;

  QTreeWidget *m_srcTreeWidget;
  QTreeWidget *m_dstTreeWidget;
};

#endif // AXISFILECLONE_H
