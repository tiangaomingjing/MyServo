#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QWidget>
#include <QComboBox>

class MyComboBox : public QComboBox
{
public:
  explicit MyComboBox(QWidget *parent=0);
protected:
  void enterEvent(QEvent *event)Q_DECL_OVERRIDE;
};

#endif // MYCOMBOBOX_H
