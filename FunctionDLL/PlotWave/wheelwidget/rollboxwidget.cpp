#include "rollboxwidget.h"
#include "wheelwidget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QButtonGroup>
#include <QKeyEvent>
#include <QLabel>

class RollBoxWidgetPrivate{
public:
  RollBoxWidgetPrivate(RollBoxWidget *q_ptr);
  ~RollBoxWidgetPrivate();
  RollBoxWidget *const q_ptr;
  QVector<StringWheelWidget *>m_stringWheelVector;
  QVector<QPushButton *>m_buttonVector;
  int m_axisCount;
  bool m_keyCtlIsPressed;
//  QLabel *m_title;
  QStringList m_btnNameList;
  QGridLayout *m_gridLayout;
};

RollBoxWidgetPrivate::RollBoxWidgetPrivate(RollBoxWidget *q):
  q_ptr(q),
  m_keyCtlIsPressed(false)
{
  QString style="QPushButton{\
      color: rgb(97, 97, 97);\
      background-color: rgb(162, 162, 162);\
      border-style:none;\
      min-width:30px;\
      min-height:30px;\
      margin-left:1px;\
      margin-right:1px;\
      font-size:parent;\
      }\
      QPushButton:hover{\
      color: rgb(97, 97, 97);\
      background-color: rgb(214, 214, 214);\
      border-style:none;\
      }\
      QPushButton:checked{\
      color: rgb(97, 97, 97);\
      background-color: rgb(227, 227, 227);\
      border-style:none;\
      }\
      QPushButton:checked:hover{\
      color: rgb(97, 97, 97);\
      background-color: rgb(214, 214, 214);\
      border-style:none;\
      }";
//  m_button->setStyleSheet(style);
}
RollBoxWidgetPrivate::~RollBoxWidgetPrivate()
{

}

QStringList RollBoxWidget::showList(int size)
{
  QStringList showList;
  for(int i=0;i<size;i++)
  {
    QString str=QString::asprintf("%02d",i);
    showList.append(str);
  }

  return showList;
}

RollBoxWidget::RollBoxWidget(int axisCount, const QStringList &strList, QWidget *parent) : QWidget(parent),
  d_ptr(new RollBoxWidgetPrivate(this))
{
  d_ptr->m_gridLayout = new QGridLayout(this);
  d_ptr->m_gridLayout->setHorizontalSpacing(0);
  d_ptr->m_gridLayout->setVerticalSpacing(5);
//  d_ptr->m_title=new QLabel(this);
  d_ptr->m_btnNameList=strList;
//  grid->addWidget(d_ptr->m_title,0,0,1,axisCount);
  StringWheelWidget *wheelWidget;
  QPushButton *btn;
//  QStringList show = showList(strList.count());
//  show.clear();
//  show<<QStringLiteral("0空闭模式")<<QStringLiteral("1电流校正")
//                             <<QStringLiteral("2寻找相位")<<QStringLiteral("3电压开环")
//                             <<QStringLiteral("4电压闭环")<<QStringLiteral("5速度开环")
//                               <<QStringLiteral("6速度闭环")<<QStringLiteral("7周期速度")
//                                 <<QStringLiteral("8轮廓速度")<<QStringLiteral("9电机准停");
  for(int i=0;i<axisCount;i++)
  {
    wheelWidget=new StringWheelWidget(false);
    wheelWidget->setId(i);
    wheelWidget->setMouseTracking(true);
    connect(wheelWidget,SIGNAL(stopped(int)),this,SLOT(onStopped(int)));
    connect(wheelWidget,SIGNAL(hoverIndex(int)),this,SLOT(onHover(int)));
    connect(wheelWidget,SIGNAL(dragIndex(int)),this,SLOT(onDragIndex(int)));
    wheelWidget->setItems(strList);
    wheelWidget->setMinimumHeight(50);
    btn=new QPushButton(tr("axis_%1").arg(i));
    btn->setObjectName(tr("axis_%1").arg(i));
    btn->setCheckable(true);
//    btn->setMinimumSize(50,30);
    connect(btn,SIGNAL(clicked(bool)),this,SLOT(onBtnClicked()));
    d_ptr->m_gridLayout->addWidget(wheelWidget,0,i);
    d_ptr->m_gridLayout->addWidget(btn,1,i);

    d_ptr->m_stringWheelVector.append(wheelWidget);
    d_ptr->m_buttonVector.append(btn);
  }
  d_ptr->m_gridLayout->setRowStretch(0,1);
  d_ptr->m_axisCount=axisCount;
  setUiStyleSheet();
  d_ptr->m_buttonVector.at(0)->setChecked(true);
  this->layout()->setMargin(0);
}
RollBoxWidget::~RollBoxWidget()
{
  for(int i=0;i<d_ptr->m_buttonVector.count();i++)
  {
    delete d_ptr->m_buttonVector[i];
  }
  for(int i=0;i<d_ptr->m_stringWheelVector.count();i++)
  {
    delete d_ptr->m_stringWheelVector[i];
  }
  d_ptr->m_buttonVector.clear();
  d_ptr->m_stringWheelVector.clear();
}

QStringList RollBoxWidget::items() const
{
  return d_ptr->m_btnNameList;
}
void RollBoxWidget::setItems( const QStringList &items )
{
  d_ptr->m_btnNameList=items;
  foreach (StringWheelWidget *w, d_ptr->m_stringWheelVector) {
    w->setItems(items);
  }
}

int RollBoxWidget::currentIndexAt(int axisIndex) const
{
  if(axisIndex>=d_ptr->m_axisCount)
    return 0;
  return d_ptr->m_stringWheelVector.at(axisIndex)->currentIndex();
}
void RollBoxWidget::setCurrentIndexAt(int index,int axisIndex)
{
  if(axisIndex<d_ptr->m_axisCount)
    d_ptr->m_stringWheelVector[axisIndex]->setCurrentIndex(index);
}
void RollBoxWidget::setButtonTextAt(const QString &text,int index)
{
  if(index<d_ptr->m_axisCount)
    d_ptr->m_buttonVector[index]->setText(text);
}
bool RollBoxWidget::buttonIsChecked(int axis) const
{
  if(axis<d_ptr->m_axisCount)
    return d_ptr->m_buttonVector.at(axis)->isChecked();
  else
    return false;
}
int RollBoxWidget::rollCount() const
{
  return d_ptr->m_axisCount;
}
void RollBoxWidget::appendBox(void)
{
  int id=d_ptr->m_axisCount;
  QStringList show;
  StringWheelWidget *wheelWidget;
  QPushButton *btn;
  show=d_ptr->m_btnNameList;
  wheelWidget=new StringWheelWidget(false);
  wheelWidget->setId(id);
  wheelWidget->setMouseTracking(true);
  connect(wheelWidget,SIGNAL(stopped(int)),this,SLOT(onStopped(int)));
  connect(wheelWidget,SIGNAL(hoverIndex(int)),this,SLOT(onHover(int)));
  connect(wheelWidget,SIGNAL(dragIndex(int)),this,SLOT(onDragIndex(int)));
  wheelWidget->setItems(show);
  wheelWidget->setMinimumHeight(50);
  btn=new QPushButton(tr("axis_%1").arg(id));
  btn->setObjectName(tr("axis_%1").arg(id));
  btn->setCheckable(true);
  btn->setMinimumSize(50,30);
  connect(btn,SIGNAL(clicked(bool)),this,SLOT(onBtnClicked()));
  d_ptr->m_gridLayout->addWidget(wheelWidget,0,id);
  d_ptr->m_gridLayout->addWidget(btn,1,id);
  d_ptr->m_stringWheelVector.append(wheelWidget);
  d_ptr->m_buttonVector.append(btn);
  d_ptr->m_axisCount++;
}

void RollBoxWidget::removeLastBox(void)
{
  int count=d_ptr->m_axisCount;
  if(count>0)
  {
    StringWheelWidget *wheel;
    QPushButton *btn;
    wheel=d_ptr->m_stringWheelVector.last();
    btn=d_ptr->m_buttonVector.last();
    d_ptr->m_gridLayout->removeWidget(wheel);
    d_ptr->m_gridLayout->removeWidget(btn);
    d_ptr->m_stringWheelVector.removeLast();
    d_ptr->m_buttonVector.removeLast();
    delete wheel;
    delete btn;
    d_ptr->m_axisCount--;
  }
}
void RollBoxWidget::setButtonChecked(int axis)
{
  QPushButton *btn=d_ptr->m_buttonVector.at(axis);
  setSingleButtonActived(btn);
}

void RollBoxWidget::onHover(int index)
{
  StringWheelWidget *wheel=qobject_cast<StringWheelWidget*>(sender());
  int axis=wheel->id();
//  qDebug()<<"hover :axis="<<axis<<"emit index="<<index;
  int count=items().count();
  if(index<count)
  {
//    d_ptr->m_title->setText(d_ptr->m_strList.at(index));

    emit hoverAt(axis,index);
  }
}

void RollBoxWidget::onStopped(int index)
{
  StringWheelWidget *wheel=qobject_cast<StringWheelWidget*>(sender());
  int axis=wheel->id();
  qDebug()<<"stop: axis="<<axis<<"emit index="<<index;
  setSingleButtonActived(d_ptr->m_buttonVector.at(axis));
  emit stopAt(axis,index);
}
void RollBoxWidget::onBtnClicked()
{
  QPushButton *btn=qobject_cast<QPushButton *>(sender());
  int axis=d_ptr->m_buttonVector.indexOf(btn);
  qDebug()<<"btn click axis="<<axis<<"count="<<d_ptr->m_buttonVector.count();
  if(d_ptr->m_keyCtlIsPressed==false)
  {
    setSingleButtonActived(btn);
  }
  emit clickedAt(axis);
}
void RollBoxWidget::onDragIndex(int index)
{
  StringWheelWidget *wheel=qobject_cast<StringWheelWidget*>(sender());
  int axis=wheel->id();
//  qDebug()<<"hover :axis="<<axis<<"emit index="<<index;
  if(index<items().count())
  {
//    d_ptr->m_title->setText(d_ptr->m_strList.at(index));
    emit dragIndexAt(axis,index);
  }
}

void RollBoxWidget::keyPressEvent(QKeyEvent *e)
{
  if(e->key()==Qt::Key_Control)
  {
    qDebug()<<"control press";
    d_ptr->m_keyCtlIsPressed=true;

  }
  else
  {
    QWidget::keyPressEvent(e);
  }
}
void RollBoxWidget::keyReleaseEvent(QKeyEvent *e)
{
  if(e->key()==Qt::Key_Control)
  {
    qDebug()<<"control release";
    d_ptr->m_keyCtlIsPressed=false;
  }
  else
  {
    QWidget::keyReleaseEvent(e);
  }
}
void RollBoxWidget::setSingleButtonActived(QPushButton *btn)
{
  foreach (QPushButton *btn2, d_ptr->m_buttonVector)
  {
    if(btn2!=btn)
      btn2->setChecked(false);
    else
      btn2->setChecked(true);
  }
}
void RollBoxWidget::setUiStyleSheet()
{
  QString str="QPushButton{\
      border:1px solid gray;\
      border-radius:5px;\
      min-width:50px;\
      min-height:30px;\
      color:black;\
      margin:2px;\
    }\
  QPushButton:hover{\
      background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e7effd, stop: 1 #cbdaf1);\
      color:black;\
    }\
  QPushButton:checked{\
      background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);\
      color:white;\
    }";
  this->setStyleSheet(str);
}
