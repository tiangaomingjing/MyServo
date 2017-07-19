#ifndef QVBOXLAYOUTPROXY_H
#define QVBOXLAYOUTPROXY_H

#include <QObject>
class QHBoxLayout;
class QVBoxLayout;
class QWidget;
class QLayout;
class QFormLayout;
class QSpacerItemProxy;

class QVBoxLayoutProxy : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QVBoxLayout *vboxLayout READ vboxLayout)
public:
  explicit QVBoxLayoutProxy(QWidget *layoutParent=0,QObject *parent = 0);

  QVBoxLayout *vboxLayout(){return m_vlayout;}
  Q_INVOKABLE void addWidget(QWidget *widget);
  Q_INVOKABLE void addLayout(QHBoxLayout *layout, int stretch = 0);
  Q_INVOKABLE void addLayout(QVBoxLayout *layout, int stretch = 0);
  Q_INVOKABLE void addLayout(QFormLayout *layout, int stretch = 0);
  Q_INVOKABLE void addSpacerItem(QSpacerItemProxy * spacerItem);

signals:

public slots:
private:
  QVBoxLayout *m_vlayout;
};

#endif // QVBOXLAYOUTPROXY_H
