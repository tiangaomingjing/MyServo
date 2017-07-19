#ifndef MOBJ_H
#define MOBJ_H

#include <QObject>

class Mobj : public QObject
{
  Q_OBJECT
public:
  explicit Mobj(QObject *parent = 0);

signals:

public slots:
};

#endif // MOBJ_H
