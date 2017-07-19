#ifndef QFORMLAYOUTPROXY_H
#define QFORMLAYOUTPROXY_H

#include <QObject>
class QWidget;
class QLayout;
class QFormLayout;

class QFormLayoutProxy : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QFormLayout* fromlayout READ fromlayout)
public:
  explicit QFormLayoutProxy(QWidget *layoutParent=0,QObject *parent = 0);
  QFormLayout *fromlayout(){return m_formLayout;}
  Q_INVOKABLE void	addRow(QWidget * label, QWidget * field);
  Q_INVOKABLE void	addRow(const QString  labelText, QWidget * field);

signals:

public slots:
private :
  QFormLayout *m_formLayout;
};

#endif // QFORMLAYOUTPROXY_H
