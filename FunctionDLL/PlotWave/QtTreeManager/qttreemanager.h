#ifndef QTTREEMANAGER_H
#define QTTREEMANAGER_H

#include <QObject>
class QTreeWidget;
class QTreeWidgetItem;
class QXmlStreamWriter;

class QtTreeManager : public QObject
{
  Q_OBJECT
public:
  explicit QtTreeManager(QObject *parent = 0);
  static QTreeWidget* readTreeWidgetFromXmlFile(const QString &fileName);
  static bool writeTreeWidgetToXmlFile(const QString &fileName, const QTreeWidget *treeWidget);
signals:

public slots:
private:
  static void writeStructXmlFile(QTreeWidgetItem *item, QXmlStreamWriter *writer);

};

#endif // QTTREEMANAGER_H
