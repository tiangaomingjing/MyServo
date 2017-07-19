#ifndef QSPACERITEMPROXY_H
#define QSPACERITEMPROXY_H

#include <QObject>
class QSpacerItem ;

class QSpacerItemProxy : public QObject
{
  Q_OBJECT
  Q_PROPERTY(int  type READ type WRITE setType)
  Q_PROPERTY(QSpacerItem *spaceItem READ spaceItem)
public:
  explicit QSpacerItemProxy(int type=0,QObject *parent = 0);//type 0:水平，1：垂直
  int type(){return m_type;}
  void setType(int type);
  QSpacerItem *spaceItem(){return m_spacer;}

signals:

public slots:
private:
  QSpacerItem *m_spacer;
  int m_type;
};

#endif // QSPACERITEMPROXY_H
