#include "useractionwidget.h"
#include "ui_useractionwidget.h"

#include <QDebug>
#include <QTimer>
#include <QDesktopWidget>

UserActionWidget::UserActionWidget( QWidget *parent) :
  QWidget(parent),
  ui(new Ui::UserActionWidget)
{
  ui->setupUi(this);
  setWindowFlags(Qt::FramelessWindowHint);
  setFocusPolicy(Qt::StrongFocus);
  setFocus(Qt::ActiveWindowFocusReason);

}

UserActionWidget::~UserActionWidget()
{
  delete ui;
}
void UserActionWidget::exec(const QPoint&point, quint8 axisSize)
{
  for(int i=0;i<m_uActionList.count();i++)
  {
    delete m_uActionList[i];
  }
  m_uActionList.clear();

  for(int i=0;i<axisSize;i++)
  {
    UactionData data(UactionData::ACTION_CMD_COPY,i);
    Uaction *action=new Uaction(tr("Axis_%1").arg(i),data);
    ui->vLayout_copy->addWidget(action);
    connect(action,SIGNAL(clicked(bool)),this,SLOT(onActionClicked()));
    m_uActionList.append(action);
  }
  for(int i=0;i<axisSize;i++)
  {
    UactionData data(UactionData::ACTION_CMD_CHANGE,i);
    Uaction *action=new Uaction(tr("Axis_%1").arg(i),data);
    ui->vLayout_change->addWidget(action);
    connect(action,SIGNAL(clicked(bool)),this,SLOT(onActionClicked()));
    m_uActionList.append(action);
  }
  int virtualWidth = 0;
  int virtualHeight = 0;
//  int availableWidth = 0;
  int availableHeight = 0;
//  int screenWidth = 0;
//  int screenHeight = 0;

  QDesktopWidget *deskWgt = QApplication::desktop();
  if (deskWgt) {
      virtualWidth = deskWgt->width();
      virtualHeight = deskWgt->height();
      qDebug() << "virtual width:" << virtualWidth << ",height:" << virtualHeight ;

      QRect availableRect = deskWgt->availableGeometry();
//      availableWidth = availableRect.width();
      availableHeight = availableRect.height();
//      qDebug()<< "available width:" <<availableWidth << ",height:" << availableHeight ;

//      QRect screenRect = deskWgt->screenGeometry();
//      screenWidth = screenRect.width();
//      screenHeight = screenRect.height();
//      qDebug() << "screen width:" <<screenWidth << ",height:" << screenHeight ;
  }
  int selfWidth=this->width();
  int selfHeight=this->height();
  int dx=point.x()+selfWidth-virtualWidth;
  int dy=point.y()+selfHeight-availableHeight;
  QPoint np(point);
  if(dx>0)
    np.setX(point.x()-dx);
  if(dy>0)
    np.setY(point.y()-dy);

  move(np);
  show();
  setFocus();
}

void UserActionWidget::focusOutEvent(QFocusEvent *event)
{
  Q_UNUSED(event);
  qDebug()<<"focus out ";
//  QWidget::focusOutEvent(event);
//  QTimer::singleShot(200,this,SLOT(deleteLater()));
  QTimer::singleShot(200,this,SLOT(hide()));
}
void UserActionWidget::onActionClicked()
{
  qDebug()<<"onActionClicked";
  Uaction* act=qobject_cast<Uaction*>(sender());
  qDebug()<<"cmd="<<act->actionData().m_cmd<<" axis="<<act->actionData().m_axis;
  UactionData data(act->actionData().m_cmd,act->actionData().m_axis);
  emit actionTriggered(data);
}





Uaction::Uaction(QWidget *parent):QPushButton(parent)
{
  m_actionData.m_axis=0;
  m_actionData.m_cmd=UactionData::ACTION_CMD_COPY;
}
Uaction::Uaction(const QString &text, const UactionData &data, QWidget *parent):
  QPushButton(text,parent),
  m_actionData(data)
{

}
Uaction::~Uaction()
{

}

void Uaction::setText(const QString &text)
{
  QPushButton::setText(text);
}

UactionData Uaction::actionData() const
{
  return m_actionData;
}

void Uaction::setActionData(const UactionData &actionData)
{
  m_actionData=actionData;
}
