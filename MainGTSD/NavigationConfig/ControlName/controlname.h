#ifndef CONTROLNAME_H
#define CONTROLNAME_H

#include <QObject>
#include <QMap>
#include <QUrl>

class ControlName : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
public:
  explicit ControlName(QObject *parent = 0);
  ~ControlName();

  QUrl source() const;
  void setSource(const QUrl &source);
  Q_INVOKABLE QString value(const QString &key) const;

signals:
  void sourceChanged(QUrl &source);

public slots:
private:
  QUrl m_source;
  QMap<QString ,QString> m_map;
};

#endif // CONTROLNAME_H
