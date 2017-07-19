#ifndef MACTION_H
#define MACTION_H

#include <QWidget>
#include <QWidgetAction>
#include <QPushButton>

class MAction : public QWidgetAction
{
  Q_OBJECT
public:
  explicit MAction(QWidget *parent,QIcon &icon):QWidgetAction(parent)
  {
    btn=new QPushButton();
    btn->setMinimumSize(QSize(67, 24));
    btn->setIconSize(QSize(67, 24));
    btn->setIcon(icon);
    QString style="QPushButton:hover{border:1px solid navy;border-radius:4px}\
                   QPushButton:pressed{color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,top: 0 #dadbde, stop: 1 #f6f7fa);}";
    btn->setStyleSheet(style);
    btn->setFlat(true);
    connect(btn,SIGNAL(clicked(bool)),this,SIGNAL(triggered(bool)));
  }

  ~MAction()
  {

  }

protected:
  QWidget *createWidget(QWidget* parent)
  {
    btn->setParent(parent);
    return btn;
  }

private:
  QPushButton *btn;
};

#endif // MACTION_H
