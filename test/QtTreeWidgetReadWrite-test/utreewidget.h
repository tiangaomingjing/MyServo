#ifndef UTREEWIDGET_H
#define UTREEWIDGET_H
#include <QTreeWidget>

class UTreeWidget : public QTreeWidget
{
  Q_OBJECT
public:
  explicit UTreeWidget(QWidget *parent=0);
signals:
  void currentMousePosition(int ix,int iy);
protected:
  void mouseMoveEvent(QMouseEvent *e)Q_DECL_OVERRIDE;
};

#endif // UTREEWIDGET_H
