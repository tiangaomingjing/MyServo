#ifndef USERACTIONWIDGET_H
#define USERACTIONWIDGET_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class UserActionWidget;
}

class UactionData
{
public:
  typedef enum{
    ACTION_CMD_COPY,
    ACTION_CMD_CHANGE
  }ActionCmd;

  UactionData(){
    m_cmd=ACTION_CMD_COPY;
    m_axis=0;
  }
  UactionData(ActionCmd cmd,quint8 axis):m_cmd(cmd),m_axis(axis){
  }
  ActionCmd m_cmd;
  quint8 m_axis;
};

class Uaction :public QPushButton
{
  Q_OBJECT
public:
  explicit Uaction(QWidget *parent=0);
  explicit Uaction(const QString & text,const UactionData &data,QWidget *parent=0);
  ~Uaction();
  void setActionData(const UactionData &actionData);
  void setText(const QString & text);

  UactionData actionData() const;

private:
  UactionData m_actionData;
};

class UserActionWidget : public QWidget
{
  Q_OBJECT
public:
  explicit UserActionWidget(QWidget *parent = 0);
  ~UserActionWidget();

  void exec(const QPoint &point, quint8 axisSize);

signals:
  void actionTriggered(UactionData data);
protected:
  void focusOutEvent(QFocusEvent *event)Q_DECL_OVERRIDE;
private slots:
  void onActionClicked(void);

private:
  Ui::UserActionWidget *ui;
  QList<Uaction *>m_uActionList;
};





#endif // USERACTIONWIDGET_H
