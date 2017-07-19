#ifndef HBOXLAYOUT_H
#define HBOXLAYOUT_H

#include <QObject>
class QHBoxLayout;
class QVBoxLayout;
class QWidget;
class QLayout;
class QSpacerItemProxy;

class QHBoxLayoutProxy : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QHBoxLayout *hboxLayout READ hboxLayout)
public:
  explicit QHBoxLayoutProxy(QWidget *layoutParent=0,QObject *parent = 0);

  QHBoxLayout *hboxLayout(){return m_hlayout;}
  Q_INVOKABLE void addWidget(QWidget *widget);
  Q_INVOKABLE void addLayout(QHBoxLayout *layout, int stretch = 0);
  Q_INVOKABLE void addLayout(QVBoxLayout *layout, int stretch = 0);
  Q_INVOKABLE void addSpacerItem(QSpacerItemProxy * spacerItem);

signals:

public slots:
private:
  QHBoxLayout *m_hlayout;
};

#endif // HBOXLAYOUT_H
