#include "ledproxy.h"
#include <QLabel>
#include <QWidget>
#include <QFormLayout>

LedProxy::LedProxy(QWidget *ledparent, QObject *parent) : QObject(parent),
  m_led(new QWidget(ledparent)),
  m_nameLabel(new QLabel(m_led)),
  m_imageLabel(new QLabel(m_led))
{
  QFormLayout *flayout=new QFormLayout(m_led);
  flayout->addRow(m_imageLabel,m_nameLabel);
  m_led->setLayout(flayout);
}
void LedProxy::setState(bool sta)
{
  m_state=sta;
  QString fileName;
  if(sta)
    fileName="D:\\Exercise\\QML\\QML_CMethod\\QML_Cmethod\\on.png"  ;
  else
    fileName="D:\\Exercise\\QML\\QML_CMethod\\QML_Cmethod\\off.png"  ;
  QImage image;
  image.load(fileName);
  m_imageLabel->setPixmap(QPixmap::fromImage(image));
  m_imageLabel->resize(image.width(),image.height());
}
void LedProxy::setLedName(QString &str)
{
  m_name=str;
  m_nameLabel->setText(m_name);
}

