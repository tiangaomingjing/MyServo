#ifndef ICONLABEL_H
#define ICONLABEL_H

#include <QObject>
#include <QLabel>
class QWidget;

class IconLabel : public QLabel
{
  Q_OBJECT
public:
  IconLabel(QWidget *parent=0);
  Q_INVOKABLE void setIconSize(int w,int h);
  Q_INVOKABLE void loadIconFromFile(QString fileName);
private:
};

#endif // ICONLABEL_H
