#include "led.h"
#include <QLabel>
#include <QWidget>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QImage>

Led::Led(QWidget *parent):QWidget(parent)
{
  m_imageLabel=new QLabel(this);
  m_nameLabel=new QLabel(this);
  m_imageLabel->setText(" ");
  m_nameLabel->setText(" ");
  QHBoxLayout *fLayout=new QHBoxLayout(this);
  fLayout->addWidget(m_imageLabel);
  fLayout->addWidget(m_nameLabel);
//  fLayout->addRow(m_imageLabel,m_nameLabel);
  setLayout(fLayout);

}
void Led::setState(bool checked)
{
  m_ledState=checked;
  QString fileName;
  if(checked)
    fileName=m_onFileName ;
  else
    fileName=m_offFileName;
  QImage image;
  image.load(fileName);
  m_imageLabel->setPixmap(QPixmap::fromImage(image));
  m_imageLabel->resize(image.width(),image.height());
}
void Led::setLedName(QString str)
{
  name=str;
  m_nameLabel->setText(name);
}
int Led::alignment()
{
  m_align=m_imageLabel->alignment();
  return m_align;
}

void Led::setAlignment(int value)
{
  m_imageLabel->setAlignment(static_cast<Qt::Alignment>(value));
  m_nameLabel->setAlignment(static_cast<Qt::Alignment>(value));
}
