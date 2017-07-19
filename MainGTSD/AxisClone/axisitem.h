#ifndef AXISITEM_H
#define AXISITEM_H
#include <QGraphicsProxyWidget>
#include <QPushButton>
class Arrow;
class QGraphicsSceneResizeEvent;

class AxisItem : public QGraphicsProxyWidget
{
public:
  explicit AxisItem(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
  ~AxisItem();
  void addArrow(Arrow* arrow);
  void removeArrow(Arrow *arrow);
  QList<Arrow*> arrows();
  QPushButton * button(){return m_button;}
protected:
  void resizeEvent(QGraphicsSceneResizeEvent *event);
private :
  QPushButton *m_button;
  QList<Arrow*> arrowList;
};

#endif // AXISITEM_H
