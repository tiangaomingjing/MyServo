#include "sqltablemodel.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QVariant>


SqlTableModel::SqlTableModel(QObject *parent):QSqlTableModel(parent)
{

}

QVariant SqlTableModel::data(const QModelIndex &index, int role) const
{
  QVariant value;

  if(role < Qt::UserRole) {
      value = QSqlQueryModel::data(index, role);
  }
  else {
      int columnIdx = role - Qt::UserRole - 1;
      QModelIndex modelIndex = this->index(index.row(), columnIdx);
      value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
  }
  return value;
}

void SqlTableModel::setTable(const QString &tableName)
{
  QSqlTableModel::setTable(tableName);
  generateRoleNames();
}

QHash<int ,QByteArray> SqlTableModel::roleNames() const
{
  return m_roleNames;
}

void SqlTableModel::setFilter(const QString &filter)
{
  QSqlTableModel::setFilter(filter);
}

int SqlTableModel::rowCount(const QModelIndex &parent) const
{
  return QSqlTableModel::rowCount(parent);
}

QVariant SqlTableModel::recordValueAt(int rowIndex, int columnIndex) const
{
  QSqlRecord rec=QSqlTableModel::record(rowIndex);
  return rec.value(columnIndex);
}
QVariant SqlTableModel::recordValueAt(int rowIndex, const QString &name) const
{
  QSqlRecord rec=QSqlTableModel::record(rowIndex);
  return rec.value(name);
}

int SqlTableModel::recordColumnCount() const
{
  QSqlRecord rec=QSqlTableModel::record();
  return rec.count();
}
QString SqlTableModel::recordFieldName(int index) const
{
  QSqlRecord rec=QSqlTableModel::record();
  return rec.fieldName(index);
}

bool SqlTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
  return QSqlTableModel::insertRows(row,count,parent);
}

bool SqlTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
  return QSqlTableModel::removeRows(row,count,parent);
}

void SqlTableModel::setEditStrategy(SqlEditStrategy strategy)
{
  QSqlTableModel::setEditStrategy((EditStrategy)strategy);
}

//--------------------------------------------------------------------------------------
void SqlTableModel::generateRoleNames()
{
  m_roleNames.clear();
  for(int i=0;i<QSqlTableModel::record().count();i++)
  {
    m_roleNames.insert(Qt::UserRole+i+1,QSqlTableModel::record().fieldName(i).toUtf8());
  }
}

