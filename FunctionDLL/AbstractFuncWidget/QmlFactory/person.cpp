﻿#include "person.h"

Person::Person(QObject *parent) : QObject(parent)
{
  setObjectName("person");
  qDebug()<<this->objectName();

}
