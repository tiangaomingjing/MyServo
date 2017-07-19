#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <QObject>

class InputValidator : public QObject
{
  Q_OBJECT
public:
  enum NumberType{HexType=0,DecType=1};

  explicit InputValidator(QString &str,int bitLength=1,bool symbol=true,NumberType type=HexType,QObject *parent = 0);

  bool validate();
  void setInputText(QString &str){m_inputText=str;}
  void setByteLength(int length){m_bitLength=length;}
  void setNumberType(NumberType type){m_type=type;}

signals:

public slots:
private:
  QString m_inputText;
  int m_bitLength;
  bool isUnsigned;
  NumberType m_type;
};

#endif // INPUTVALIDATOR_H
