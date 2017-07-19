/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QUrl>

class TreeItem;
class QTreeWidget;
class QTreeWidgetItem;

//! [0]
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
  Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
public:
    TreeModel(QObject *parent = 0);
    ~TreeModel();
//! [0] //! [1]
    enum Roles  {
        TEXT1 = Qt::UserRole + 4,
        TEXT2 = Qt::UserRole + 5,
        TEXT3 = Qt::UserRole + 6,
        TEXT4 = Qt::UserRole + 7,
        TEXT5 = Qt::UserRole + 8,
        TEXT6 = Qt::UserRole + 9,
        TEXT7 = Qt::UserRole + 10,
        TEXT8 = Qt::UserRole + 11,
        TEXT9 = Qt::UserRole + 12,
        TEXT10 = Qt::UserRole + 13,
    };
    enum Orientation {
        HORIZONTAL = 0x1,
        VERTICAL = 0x2
    };
    Q_ENUM(Roles)
    Q_ENUM(Orientation)

    QUrl source();
    void setSource(const QUrl &srcFileName);

    Q_INVOKABLE void updataTreeUi();

    QHash<int,QByteArray>roleNames() const Q_DECL_OVERRIDE;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Q_INVOKABLE int childrenCount(const QModelIndex &parent = QModelIndex());
    Q_INVOKABLE bool setDataByColumn(const QModelIndex &index, const QVariant &value, int column);

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;//孩子的数目
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;


//! [1]

//! [2]
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    bool insertColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

    TreeItem *invisibleRootItem(){return rootItem;}

signals:
    void sourceChanged();

private:
    void setupModelData();
    TreeItem *getItem(const QModelIndex &index) const;
    void clear();
    void reload();
    QTreeWidget *readTreeWidgetFromFile(const QString &filename);//读取树的数据
    void treeItemClone(TreeItem*item, QTreeWidgetItem *srcTreeItem, quint16 columnCount);

    TreeItem *rootItem;
    QUrl m_source;
};
//! [2]

#endif // TREEMODEL_H
