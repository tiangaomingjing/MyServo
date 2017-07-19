#include "mycombobox.h"

MyComboBox::MyComboBox(QWidget *parent):
  QComboBox(parent)
{

}

void MyComboBox::enterEvent(QEvent *event)
{
  showPopup();
  QComboBox::enterEvent(event);
}
