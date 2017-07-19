#include "inputvalidator.h"
#include <QRegExpValidator>
#include <QtMath>

InputValidator::InputValidator(QString &str, int bitLength,bool symbol, NumberType type, QObject *parent) : QObject(parent),
  m_inputText(str),
  m_bitLength(bitLength),
  isUnsigned(symbol),
  m_type(type)
{

}

bool InputValidator::validate()
{
  QRegExp rx;
  QRegExpValidator valiator;
  int pos=0;
  qlonglong value;
  qlonglong maxValue;
  QString byte="^0[xX][0-9a-fA-F]{1,2}";
  QString byte2="^0[xX][0-9a-fA-F]{1,4}";
  QString byte4="^0[xX][0-9a-fA-F]{1,8}";
  QString byte8="^0[xX][0-9a-fA-F]{1,16}";
  QString str("^\\d+$");
  switch (m_type)
  {
  case HexType:
    switch (m_bitLength)
    {
    case 64:
      rx.setPattern(byte8);
      break;
    case 32:
      rx.setPattern(byte4);
      break;
    case 16:
      rx.setPattern(byte2);
      break;
    default:
      rx.setPattern(byte);
      break;
    }
    //检查格式
    valiator.setRegExp(rx);
    if(valiator.validate(m_inputText,pos)!=QValidator::Acceptable)
      return false;
    //检查大小
    if(m_bitLength<8)
    {
      value=m_inputText.toLongLong(0,16);
      if(isUnsigned==true)
        maxValue=qPow(2,m_bitLength)-1;
      else
        maxValue=qPow(2,m_bitLength)/2;
      if(value>maxValue)
        return false;
    }
    break;

  case DecType:
    rx.setPattern(str);
    //检查格式
    valiator.setRegExp(rx);
    if(valiator.validate(m_inputText,pos)!=QValidator::Acceptable)
      return false;
    //检查大小
    value=m_inputText.toLongLong(0,10);
    if(isUnsigned==true)
      maxValue=qPow(2,m_bitLength)-1;
    else
      maxValue=qPow(2,m_bitLength)/2;
    if(value>maxValue)
      return false;
    break;
  }

  return true;
}
