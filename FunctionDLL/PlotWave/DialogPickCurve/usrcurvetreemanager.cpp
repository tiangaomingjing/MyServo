#include "usrcurvetreemanager.h"
#include "QtTreeManager/qttreemanager.h"
#include "plotwave_globaldef.h"
#include "XmlBuilder/xmlbuilder.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>


UsrCurveTreeManager::UsrCurveTreeManager(QTreeWidget *srcAllTreePtr,QString &curveTemplateFileName, QObject *parent) : QObject(parent),
  mp_srcAllTree(srcAllTreePtr),
  m_treeFileName(curveTemplateFileName)
{
  m_usrTreeTemplate=QtTreeManager::readTreeWidgetFromXmlFile(curveTemplateFileName);
  m_currentAxisNumber=mp_srcAllTree->topLevelItemCount();
  createUsrTreeListBaseAxisSize(m_currentAxisNumber);
  updateUsrTreeList();
}
UsrCurveTreeManager::~UsrCurveTreeManager()
{
  m_usrTreeTemplate->clear();
  delete m_usrTreeTemplate;
  clearUsrTreeWidgetList();
}

/**
 * @brief UsrCurveTreeManager::setNewSrcAllTree
 * 当用户切换版本时要调用，以更新树信息
 * @param srcNewTree :多个轴的RAM树指针
 */
void UsrCurveTreeManager::setNewSrcAllTree(QTreeWidget *srcNewTree)
{
  mp_srcAllTree = srcNewTree;
  if(m_currentAxisNumber!=mp_srcAllTree->topLevelItemCount())//轴数变了
  {
    m_currentAxisNumber=mp_srcAllTree->topLevelItemCount();
    clearUsrTreeWidgetList();
    createUsrTreeListBaseAxisSize(m_currentAxisNumber);
  }
  updateUsrTreeList();
}

QTreeWidget *UsrCurveTreeManager::usrTreeTemplate() const
{
  return m_usrTreeTemplate;
}

QList<QTreeWidget *> UsrCurveTreeManager::usrTreeList() const
{
  return m_usrTreeList;
}
/**
 * @brief UsrCurveTreeManager::addTreeCurve
 * 1.对增加的曲线修改id
 * 2.修改多轴参数中的轴号
 * 3.修改对应轴的对应地址
 * @param itemCurve
 */
void UsrCurveTreeManager::addTreeCurve(QTreeWidgetItem *itemCurve)
{
  int total=m_usrTreeTemplate->topLevelItemCount();
  itemCurve->child(ROW_PRM_INDEX_CURVEID)->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(total+1));
  m_usrTreeTemplate->addTopLevelItem(itemCurve->clone());
//  m_usrTreeTemplate->show();

  for(int i=0;i<m_usrTreeList.count();i++)
  {
    QTreeWidget *srcTree=new QTreeWidget;
    srcTree->addTopLevelItem(mp_srcAllTree->topLevelItem(i)->clone());
    itemCurve->child(ROW_PRM_INDEX_AXISNUM)->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(i));//更新成对应的轴号AxisNum
    updateTreeItemOffsetAddrBySrcTree(itemCurve,srcTree);
    m_usrTreeList[i]->addTopLevelItem(itemCurve->clone());
    srcTree->clear();
    delete srcTree;
//    m_usrTreeList[i]->show();
  }

}

void UsrCurveTreeManager::removeTreeItem(int index,QTreeWidget *treeWidget)
{
  for(int i=0;i<treeWidget->topLevelItemCount()-index;i++)
  {
    //下面的曲线 id号减1
    int currentId=treeWidget->topLevelItem(index+i)->child(ROW_PRM_INDEX_CURVEID)->text(COL_CURVESETTING_INDEX_VALUE).toInt();
    treeWidget->topLevelItem(index+i)->child(ROW_PRM_INDEX_CURVEID)->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(currentId-1));
  }
  treeWidget->takeTopLevelItem(index);
}
/**
 * @brief UsrCurveTreeManager::removeTreeCurve
 * @param index 从0开始
 */
bool UsrCurveTreeManager::removeTreeCurve(int index)
{
  //在这多前界面Ui先做减处理
  if(index>m_usrTreeTemplate->topLevelItemCount()-1)
  {
    qDebug()<<"out of range";
    return false;
  }
  removeTreeItem(index,m_usrTreeTemplate);
  for(int i=0;i<m_usrTreeList.count();i++)
  {
    removeTreeItem(index,m_usrTreeList[i]);
  }
  return true;
}

void UsrCurveTreeManager::saveTreeCurveTemplate(QString &fileName)
{
  QtTreeManager::writeTreeWidgetToXmlFile(fileName,m_usrTreeTemplate);
}

void UsrCurveTreeManager::clearUsrTreeWidgetList()
{
  QTreeWidget *treeWidget;
  for(int i=0;i<m_usrTreeList.count();i++)
  {
    treeWidget=m_usrTreeList[i];
    treeWidget->clear();
    delete treeWidget;
  }
  m_usrTreeList.clear();
}
void UsrCurveTreeManager::createUsrTreeListBaseAxisSize(int axisSize)
{
  QTreeWidgetItem *treeItem;
  int axisCount=axisSize;
  for(int i=0;i<axisCount;i++)
  {
    QTreeWidget *treeWidget=new QTreeWidget;
    for(int j=0;j<m_usrTreeTemplate->topLevelItemCount();j++)
    {
      treeItem=m_usrTreeTemplate->topLevelItem(j)->clone();
      treeWidget->addTopLevelItem(treeItem);
    }
    m_usrTreeList.append(treeWidget);
  }
}
void UsrCurveTreeManager::updateTreeItemOffsetAddrBySrcTree(QTreeWidgetItem *item, QTreeWidget *treeSrcAxis)
{
  QTreeWidgetItem *itemPrmFactor;
  QTreeWidgetItem *itemFactor;

  CurveType curveType;
  curveType=(CurveType)item->text(COL_CURVESETTING_INDEX_VALUE).toUInt();

  QTreeWidgetItem *itemSrc;
  QTreeWidgetItemIterator itSrc(treeSrcAxis);
  QString offset;
  QString name;

  if(curveType==CURVE_TYPE_SCRIPT_CONST)
  {
    itemPrmFactor=item->child(ROW_PRM_INDEX_PRMFACTOR);
    itemFactor=itemPrmFactor->child(0);//只有一个变量
    name=itemFactor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);//FullName
    while((*itSrc))
    {
      itemSrc=(*itSrc);
      if(name==itemSrc->text(COL_NAME))
      {
        offset=itemSrc->text(COL_ADDRESS);
        qDebug()<<"--find-- "<<itemSrc->text(COL_NAME)<<"offset "<<offset;
        break;
      }
      itSrc++;
    }
    itemFactor->child(ROW_FACTOR_INDEX_OFFSETADDR)->setText(COL_CURVESETTING_INDEX_VALUE,offset);
  }
  else//脚本类型
  {
    itemPrmFactor=item->child(ROW_PRM_INDEX_PRMFACTOR);
    for(int k=0;k<itemPrmFactor->childCount();k++)
    {
      itemFactor=itemPrmFactor->child(k);
      name=itemFactor->child(ROW_FACTOR_INDEX_FULLNAME)->text(COL_CURVESETTING_INDEX_VALUE);
      QTreeWidgetItemIterator itSrc(treeSrcAxis);//重新定位到树开始
      while((*itSrc))
      {
        itemSrc=(*itSrc);
        if(name==itemSrc->text(COL_NAME))
        {
          offset=itemSrc->text(COL_ADDRESS);
          qDebug()<<"--find-- "<<itemSrc->text(COL_NAME)<<"offset "<<offset;
          break;
        }
        itSrc++;
      }
      itemFactor->child(ROW_FACTOR_INDEX_OFFSETADDR)->setText(COL_CURVESETTING_INDEX_VALUE,offset);
      //目前这几种中都是已知的，不对BaseAddr处理(增加曲线时就确定了BaseAddr)[在里面写0-gServo 而 1-gAux]
    }
  }
}

void UsrCurveTreeManager::updateUsrTreeList()
{
  if(mp_srcAllTree->topLevelItemCount()!=m_usrTreeList.count())
  {
    qDebug()<<"srcAllTree count is not equal to usrTreeList count";
    return;
  }

  for(int i=0;i<mp_srcAllTree->topLevelItemCount();i++)
  {
    QTreeWidget *treeAxis=new QTreeWidget;
    QTreeWidget *treeUsr=m_usrTreeList[i];//第i轴曲线模板
    QTreeWidgetItem *item;

    item=mp_srcAllTree->topLevelItem(i)->clone();
    treeAxis->addTopLevelItem(item);//第i个轴的ram树

    for(int j=0;j<treeUsr->topLevelItemCount();j++)
    {
      item=treeUsr->topLevelItem(j);//第i轴第j条曲线
      item->child(ROW_PRM_INDEX_AXISNUM)->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(i));//更新成对应的轴号AxisNum
      updateTreeItemOffsetAddrBySrcTree(item,treeAxis);
    }
    treeAxis->clear();
    delete treeAxis;
  }
}

