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
#include "treeitem.h"
#include "treemodel.h"
#include <QDebug>
#include <QTreeWidget>
#include <QFormBuilder>
#include <QMessageBox>
#include <QHeaderView>
#include <QUrl>

TreeModel::TreeModel( QObject *parent)
    : QAbstractItemModel(parent),
      rootItem(nullptr)
{
//    QVector<QVariant> rootData;
//    rootData<<"title"<<"parameter";
////    foreach (QString header, headers)
////        rootData << header;

//    rootItem = new TreeItem(rootData);

}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
  if(rootItem!=nullptr)
  {
    qDebug()<<"release treemodel ";
    delete rootItem;
  }

}
//! [1]

QUrl TreeModel::source()
{
  return m_source;
}

void TreeModel::setSource(const QUrl &srcFileName)
{
  qDebug()<<"setSource:"<<srcFileName;
  m_source=srcFileName;
  reload();
  emit sourceChanged();
}

void TreeModel::updataTreeUi()
{
  beginResetModel();
  endResetModel();
}

//! [2]
int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
    return rootItem->columnCount();
}
//! [2]

QHash<int,QByteArray> TreeModel::roleNames() const
{
  QHash<int, QByteArray> result = QAbstractItemModel::roleNames();
  result.insert(TEXT1, QByteArrayLiteral("Text1"));
  result.insert(TEXT2, QByteArrayLiteral("Text2"));
  result.insert(TEXT3, QByteArrayLiteral("Text3"));
  result.insert(TEXT4, QByteArrayLiteral("Text4"));
  result.insert(TEXT5, QByteArrayLiteral("Text5"));
  result.insert(TEXT6, QByteArrayLiteral("Text6"));
  result.insert(TEXT7, QByteArrayLiteral("Text7"));
  result.insert(TEXT8, QByteArrayLiteral("Text8"));
  result.insert(TEXT9, QByteArrayLiteral("Text9"));
  result.insert(TEXT10, QByteArrayLiteral("Text10"));
  return result;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    QVariant var;
    TreeItem *item = getItem(index);
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
      var=item->data(index.column());
      break;
    case TEXT1:
      var=item->data(0);
      break;
    case TEXT2:
      var=item->data(1);
      break;
    case TEXT3:
      var=item->data(2);
      break;
    case TEXT4:
      var=item->data(3);
      break;
    case TEXT5:
      var=item->data(4);
      break;
    case TEXT6:
      var=item->data(5);
      break;
    case TEXT7:
      var=item->data(6);
      break;
    case TEXT8:
      var=item->data(7);
      break;
    case TEXT9:
      var=item->data(8);
      break;
    case TEXT10:
      var=item->data(9);
      break;
    }

    return var;
}

//! [3]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}
//! [3]

//! [4]
TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}
//! [4]

void TreeModel::clear()
{
  if(rootItem!=nullptr)
    delete rootItem;
  rootItem=nullptr;
}

void TreeModel::reload()
{
  beginResetModel();
  //加载xml文件
  clear();
  setupModelData();
  endResetModel();
}

QTreeWidget *TreeModel::readTreeWidgetFromFile(const QString &filename)
{
  QFormBuilder builder;
  QFile file(filename);

  if (!file.open(QFile::ReadOnly))
  {
    QMessageBox::information(0, tr("open error"), tr("open %1 file error ").arg(filename));
    return NULL;
  }
  QWidget *widget=NULL;
  QTreeWidget *treeWidget=NULL;
  QTreeWidget *outTreeWidget=NULL;
//  qDebug()<<"fileName1"<<filename;

  widget = builder.load(&file, 0);
  if (widget==NULL)
  {
    QMessageBox::information(0, tr("load error"), tr("load .xml file error "));
    return NULL;
  }
  file.close();

  treeWidget=widget->findChild<QTreeWidget*>("treeWidget");//可以这样子查找
//  QList<QTreeWidget *>trees=widget->findChildren<QTreeWidget*>;
  if(treeWidget!=NULL)
  {
    outTreeWidget=new QTreeWidget();
    QTreeWidgetItem *item;
    for(int i=0;i<treeWidget->topLevelItemCount();i++)
    {
      item=treeWidget->topLevelItem(i)->clone();
      outTreeWidget->addTopLevelItem(item);
    }
    QTreeWidgetItem *head;
    head=treeWidget->headerItem()->clone();
    outTreeWidget->setHeaderItem(head);
  }
  delete widget;
//  qDebug()<<"fileName2"<<filename;
  return outTreeWidget;


}

void TreeModel::treeItemClone(TreeItem *item, QTreeWidgetItem *srcTreeItem, quint16 columnCount)
{
  QTreeWidgetItem *itemChild;
  for(int i=0;i<columnCount;i++)
    item->setData(i,srcTreeItem->text(i));
  for(int i=0;i<srcTreeItem->childCount();i++)
  {
    itemChild=srcTreeItem->child(i);
    item->insertChildren(i,1,columnCount);
    treeItemClone(item->child(i),itemChild,columnCount);
  }
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
int TreeModel::childrenCount(const QModelIndex &parent)
{
  return rowCount(parent);
}
bool TreeModel::setDataByColumn(const QModelIndex &index, const QVariant &value ,int column)
{
  TreeItem *item = getItem(index);
  bool result = item->setData(column, value);
  return result;
}

//! [5]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
//! [5]

//! [6]
    TreeItem *parentItem = getItem(parent);

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}
//! [7]

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);

    return parentItem->childCount();
}
//! [8]

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    TreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

void TreeModel::setupModelData()
{
  QTreeWidget *tree;
  QString fileName=m_source.toLocalFile();

  tree=readTreeWidgetFromFile(fileName);
//  tree->show();
  quint16 count=tree->topLevelItemCount();
  quint16 headerColumnCount=tree->columnCount();
  QVector<QVariant> rootData;
  for(int i=0;i<headerColumnCount;i++)
    rootData<<tree->headerItem()->text(i);
  rootItem = new TreeItem(rootData);
  rootItem->insertChildren(0,count,headerColumnCount);

  QTreeWidgetItem *topItem;

  for(int i=0;i<count;i++)
  {
    topItem=tree->topLevelItem(i);
    treeItemClone(rootItem->child(i),topItem,headerColumnCount);
  }

  tree->clear();
  delete tree;
//  QVector<QVariant>data;
//  data<<"c1"<<"c2"<<"c3";
//  QVector<QVariant> rootData;
//  rootData<<"title"<<"parameter";
//  rootItem = new TreeItem(rootData);

//  rootItem->insertChildren(0,2,2);
//  rootItem->child(0)->setData(0,"a1");
//  rootItem->child(0)->setData(1,"a2");
//  rootItem->child(0)->insertChildren(0,2,2);
//  rootItem->child(0)->child(0)->setData(0,"a11");
//  rootItem->child(0)->child(0)->setData(1,"a12");
//  rootItem->child(0)->child(1)->setData(0,"a21");
//  rootItem->child(0)->child(1)->setData(1,"a22");
//  TreeItem *item1=new TreeItem(data);
//  rootItem->child(0)->child(1)->appendChild(item1);
//  TreeItem *item2=new TreeItem(data);
//  item1->appendChild(item2);

//  rootItem->child(1)->setData(0,"b1");
//  rootItem->child(1)->setData(1,"b2");

//  insertColumns(2,2);
//  setHeaderData(2,Qt::Horizontal,"aaaaaa");
}
