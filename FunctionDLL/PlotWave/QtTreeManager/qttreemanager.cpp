#include "qttreemanager.h"

#include<QFormBuilder>
#include<QTreeWidgetItem>
#include<QTreeWidget>
#include<QMessageBox>
#include<QWidget>
#include<QVBoxLayout>
#include<QXmlStreamWriter>
#include<QDebug>

QtTreeManager::QtTreeManager(QObject *parent) : QObject(parent)
{

}

//!
//! \brief QtTreeManager::readTreeWidgetFromXmlFile
//! \param fileName
//! \return QTreeWidget*:OK   NULL:error occurs
//!
QTreeWidget* QtTreeManager::readTreeWidgetFromXmlFile(const QString &fileName)
{
  QFormBuilder builder;
  QFile file(fileName);

  if (!file.open(QFile::ReadOnly))
  {
    QMessageBox::information(0, tr("open error"), tr("open %1 file error ").arg(fileName));
    return NULL;
  }
  QWidget *widget=NULL;
  QTreeWidget *treeWidget=NULL;

  widget = builder.load(&file, 0);
  if (widget==NULL)
  {
    QMessageBox::information(0, tr("load error"), tr("load .xml file error "));
    file.close();
    return NULL;
  }
  file.close();

  QTreeWidget *tree=widget->findChild<QTreeWidget*>("treeWidget");//可以这样子查找
  if(tree!=NULL)
  {
    treeWidget=new QTreeWidget();
    QTreeWidgetItem *item;
    for(int i=0;i<tree->topLevelItemCount();i++)
    {
      item=tree->topLevelItem(i)->clone();
      treeWidget->addTopLevelItem(item);
    }
    QTreeWidgetItem *head;
    head=tree->headerItem()->clone();
    treeWidget->setHeaderItem(head);
    delete widget;
    return treeWidget;
  }
  else
  {
    delete widget;
    QMessageBox::information(0, tr("load error"), tr("findChild treeWidget error! "));
    return NULL;
  }
}

bool QtTreeManager::writeTreeWidgetToXmlFile(const QString &fileName, const QTreeWidget *treeWidget)
{
  QFile fileXML(fileName);
  if (!fileXML.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QMessageBox::information(0,tr("XML File"),tr("XML file save error!"));
    return false;
  }

//  QWidget *widget=new QWidget;
//  QVBoxLayout *vLayout=new QVBoxLayout(widget);
//  QTreeWidget *tree=new QTreeWidget(widget);
//  QTreeWidgetItem *item;
//  QTreeWidgetItem *itemHead;
//  for(int i=0;i<treeWidget->topLevelItemCount();i++)
//  {
//    item=treeWidget->topLevelItem(i)->clone();
//    tree->addTopLevelItem(item);
//  }
//  itemHead=treeWidget->headerItem()->clone();
//  tree->setHeaderItem(itemHead);
//  vLayout->addWidget(tree);
//  widget->setLayout(vLayout);
//  QFormBuilder builder;
//  builder.save(&fileXML,widget);

  QXmlStreamWriter writer(&fileXML);
  writer.setAutoFormatting(true);
  writer.writeStartDocument("1.0");
  writer.writeStartElement("ui");                 //1
  writer.writeAttribute("version","4.0");

  writer.writeTextElement("class", "Form");
  writer.writeStartElement("widget");             //2<widget
    writer.writeAttribute("class","QWidget");
    writer.writeAttribute("name","Form");

    writer.writeStartElement("property");          //3
      writer.writeAttribute("name","geometry");
      writer.writeStartElement("rect");
      writer.writeTextElement("x", "0");
      writer.writeTextElement("y", "0");
      writer.writeTextElement("width", "400");
      writer.writeTextElement("height", "400");
      writer.writeEndElement();
    writer.writeEndElement();                     //-3

    writer.writeStartElement("property");
    writer.writeAttribute("name","windowTitle");
      writer.writeTextElement("string", "Form");
    writer.writeEndElement();

    writer.writeStartElement("layout");             //<layout
    writer.writeAttribute("class","QVBoxLayout");
    writer.writeAttribute("name","verticalLayout");
      writer.writeStartElement("item");             //<item

      writer.writeStartElement("widget");           //<widget
      writer.writeAttribute("class","QTreeWidget");
      writer.writeAttribute("name","treeWidget");

      //写树的列
      QTreeWidgetItem *headerItem=treeWidget->headerItem();
      qDebug()<<"headerItem columnCount:"<<headerItem->columnCount();
      for(int i=0;i<headerItem->columnCount();i++ )
      {
        writer.writeStartElement("column");
        writer.writeStartElement("property");
        writer.writeAttribute("name","text");
        writer.writeTextElement("string", headerItem->text(i));
        writer.writeEndElement();
        writer.writeEndElement();
        qDebug()<<"column:"<<i<<" "<< "text: "<<headerItem->text(i);
      }
      //开始写树结构
      QTreeWidgetItem *item;
      int itemCount=treeWidget->topLevelItemCount();
      for(int i=0;i<itemCount;i++)
      {
        item=treeWidget->topLevelItem(i);
        writer.writeStartElement("item");

        for(int j=0;j<item->columnCount();j++)
        {
          writer.writeStartElement("property");
          writer.writeAttribute("name","text");
          writer.writeTextElement("string",item->text(j));
          writer.writeEndElement();
        }
        writeStructXmlFile(item,&writer);
        writer.writeEndElement();
      }
      writer.writeEndElement();                     //</widget>
      writer.writeEndElement();                     //</item>
    writer.writeEndElement();                       //</layout>
  writer.writeEndElement();                       //-2</widget>
  writer.writeStartElement("resources");
  writer.writeEndElement();
  writer.writeStartElement("connections");
  writer.writeEndElement();                        //
  writer.writeEndElement();                        //-1 </ui>
  writer.writeEndDocument();
  fileXML.close();
  return true;
}
void QtTreeManager::writeStructXmlFile(QTreeWidgetItem *item,QXmlStreamWriter *writer)
{
  QTreeWidgetItem *itemChild;

  for (int i = 0; i<item->childCount();i++)
  {
    itemChild = item->child(i);
    writer->writeStartElement("item");

    for(int j=0;j<item->columnCount();j++)
    {
      writer->writeStartElement("property");
      writer->writeAttribute("name","text");
      writer->writeTextElement("string",itemChild->text(j));
      writer->writeEndElement();
    }
    writeStructXmlFile(itemChild, writer);
    writer->writeEndElement();
  }
}
