#ifndef SQLTABLEMODEL_H
#define SQLTABLEMODEL_H
#include <QSqlTableModel>

class SqlTableModel : public QSqlTableModel
{
  Q_OBJECT
public:
  enum SqlEditStrategy {OnFieldChange, OnRowChange, OnManualSubmit};
  Q_ENUM(SqlEditStrategy)//暴露枚举到qml

  enum MotorColumnFieldIndex{MotorId,MotorName,MotorCompanyId
                            ,MotorImax,MotorIrat,MotorSct
                            ,MotorSrat,MotorNos,MotorRm
                            ,MotorLdm,MotorLqm,MotorJm
                            ,MotorJrat,MotorFm,MotorPPN
                            ,MotorTqr,MotorPHIm,MotorVmax, MotorColumnCount};
  Q_ENUM(MotorColumnFieldIndex)

  enum CompanyColumnFieldIndex{CompanyId,CompanyName, CompanyColumnCount};
  Q_ENUM(CompanyColumnFieldIndex)

  explicit SqlTableModel(QObject *parent=0);

  Q_INVOKABLE QVariant        data(const QModelIndex &index,int role) const Q_DECL_OVERRIDE;
  Q_INVOKABLE void            setTable(const QString &tableName)Q_DECL_OVERRIDE;
  QHash<int ,QByteArray>      roleNames() const Q_DECL_OVERRIDE;
  Q_INVOKABLE void            setFilter(const QString &filter) Q_DECL_OVERRIDE;
  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
  Q_INVOKABLE QVariant recordValueAt(int rowIndex,int columnIndex) const ;
  Q_INVOKABLE QVariant recordValueAt(int rowIndex,const QString& name) const ;
  Q_INVOKABLE int recordColumnCount() const ;
  Q_INVOKABLE QString recordFieldName(int index )const;
  Q_INVOKABLE bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
  Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
  Q_INVOKABLE void setEditStrategy(SqlEditStrategy strategy);
private:
  void generateRoleNames();
  QHash<int ,QByteArray>m_roleNames;
};

#endif // SQLTABLEMODEL_H
