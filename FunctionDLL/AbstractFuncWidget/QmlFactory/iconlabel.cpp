#include "iconlabel.h"
#include <QImage>

IconLabel::IconLabel(QWidget *parent):QLabel(parent)
{

}
void IconLabel::setIconSize(int w,int h)
{
  resize(w,h);
}
void IconLabel::loadIconFromFile(QString fileName)
{
  QImage image;
  image.load(fileName);
  setPixmap(QPixmap::fromImage(image));
  resize(image.width(),image.height());
}
