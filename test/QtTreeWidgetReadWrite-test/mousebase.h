#ifndef MOUSEBASE_H
#define MOUSEBASE_H

#include <QObject>
#include <QFrame>

class MouseBase : public QFrame
{
  Q_OBJECT
public:
  explicit MouseBase(QWidget*parent=0);
signals:
  void currentMousePosition(int ix,int iy);
protected:
  void mouseMoveEvent(QMouseEvent *e);
};

#endif // MOUSEBASE_H
