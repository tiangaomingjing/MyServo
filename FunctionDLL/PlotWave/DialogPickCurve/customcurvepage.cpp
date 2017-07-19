#include "customcurvepage.h"
#include "ui_customcurvepage.h"
#include "plotwave_globaldef.h"
#include "globaldef.h"
#include "xmlbuilder.h"

#include "./Qscintilla/includes/qsciscintilla.h"
#include <./Qscintilla/includes/qscilexer.h>
#include <./Qscintilla/includes/qscilexercpp.h>
#include <./Qscintilla/includes/qsciapis.h>
#include <./Qscintilla/includes/qsciabstractapis.h>
#include <QMessageBox>
#include <QDebug>

typedef enum{
  ROW_INDEX_UNIT,
  ROW_INDEX_VARIABLE,
  ROW_INDEX_CONST
}RowIndex;


CustomCurvePage::CustomCurvePage(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CustomCurvePage)
{
  ui->setupUi(this);
  ui->stackedWidget->hide();
  ui->label_status->hide();
  ui->treeWidget_input->expandAll();
  ui->treeWidget_input->resizeColumnToContents(0);
  ui->treeWidget_input->setCurrentItem(ui->treeWidget_input->topLevelItem(0));

  ui->treeWidget_const->expandToDepth(1);
  ui->treeWidget_variable->expandToDepth(1);

  ui->btn_hide->hide();

  ui->lineEdit_curveName->setText("curve_xxx");

  //sci格式代显示与自动补全功能

  QsciLexer *textLexer=new QsciLexerCPP(ui->textEdit);//设置显示的方式C++
  ui->textEdit->setLexer(textLexer);
  QsciAPIs *apis=new QsciAPIs(textLexer);
  m_sciMathKeywordsList<<"sin()"<<"cos()"<<"tan()"<<"asin()"<<"acos()"<<"atan()"<<"_pi"<<"_e"\
                      <<"exp()"<<"log()"<<"max()"<<"min()"<<"abs()";
  foreach (QString str, m_sciMathKeywordsList)
    apis->add(str);
//  apis->add("V0");
//  apis->add("V1");
//  apis->add("sin()");
//  apis->add("cos()");
//  apis->add("power()");
//  apis->add("C0");
//  apis->add("C1");
  apis->prepare();

  ui->textEdit->setAutoCompletionSource(QsciScintilla::AcsAll);
  ui->textEdit->setAutoCompletionCaseSensitivity(true);
  ui->textEdit->setAutoCompletionThreshold(1);

  initialCurveTreeWidgetItem();
//  ui->treeWidget_const->addTopLevelItem(m_curveItem);

  connect(ui->treeWidget_variable,SIGNAL(currentMousePosition(int,int)),this,SLOT(onTreeWidgetMouseMoveEvent(int,int)));
  connect(ui->treeWidget_const,SIGNAL(currentMousePosition(int,int)),this,SLOT(onTreeWidgetMouseMoveEvent(int,int)));
  connect(ui->btn_add,SIGNAL(clicked(bool)),this,SLOT(onBtnAddClicked()));
  connect(ui->btn_hide,SIGNAL(clicked(bool)),this,SLOT(onBtnHideClicked()));
  connect(ui->btn_remove,SIGNAL(clicked(bool)),this,SLOT(onBtnRemoveClicked()));
  connect(ui->btn_new,SIGNAL(clicked(bool)),this,SLOT(onBtnNewClicked()));
  connect(ui->btn_save,SIGNAL(clicked(bool)),this,SLOT(onBtnSaveClicked()));
  connect(ui->treeWidget_input,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetInputClicked(QTreeWidgetItem*,int)));
  connect(ui->treeWidget_input,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetInputDoubleClicked(QTreeWidgetItem*,int)));
  connect(ui->treeWidget_variable,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetVariableDoubleClicked(QTreeWidgetItem*,int)));
  connect(ui->treeWidget_const,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeWidgetConstDoubleClicked(QTreeWidgetItem*,int)));
  connect(ui->treeWidget_variable,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(onTreeWidgetItemExpanded(QTreeWidgetItem*)));
  connect(ui->treeWidget_const,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(onTreeWidgetItemExpanded(QTreeWidgetItem*)));

  ui->treeWidget_input->setColumnCount(5);
}

CustomCurvePage::~CustomCurvePage()
{
  delete m_curveItem;
  delete ui;
}
void CustomCurvePage::setCurveTreeTemplate(QTreeWidget* treeTemplate)
{
  mp_treeTemplate=treeTemplate;
}
/**
 * @brief CustomCurvePage::setCurveTreeRAM
 * @param treeRam 将RAMALL中第一个轴的树指针传进来
 */
void CustomCurvePage::setCurveTreeRAM(QTreeWidgetItem* treeRam)
{
  //目前常量，变量都设成一致的树，后面再分类
//  QTreeWidgetItem *item;
//  for(int i=0;i<treeRam->topLevelItemCount();i++)
//  {
//    item=treeRam->topLevelItem(i)->clone();
//    ui->treeWidget_const->addTopLevelItem(item);
//    ui->treeWidget_variable->addTopLevelItem(item);
//  }
  ui->treeWidget_const->addTopLevelItem(treeRam->clone());
  ui->treeWidget_variable->addTopLevelItem(treeRam->clone());

  ui->treeWidget_const->setColumnCount(3);
  ui->treeWidget_variable->setColumnCount(3);
  ui->treeWidget_const->expandToDepth(1);
  ui->treeWidget_variable->expandToDepth(1);
}

void CustomCurvePage::onTreeWidgetMouseMoveEvent(int x, int y)
{
//  qDebug()<<"mouse X="<<x<<" Y="<<y;
//  int iYTop=(ui->treeWidget_variable->height()-ui->btn_hide->height())/2;
//  int iYBottom=(ui->treeWidget_variable->height()+ui->btn_hide->height())/2;
//  if((x<30)&&(x>0)&&(y>iYTop)&&(y<iYBottom))
  Q_UNUSED(y);
  if(x<40)
    ui->btn_hide->show();
  else
    ui->btn_hide->hide();
}
void CustomCurvePage::onBtnAddClicked()
{
  QTreeWidgetItem *currentItem=ui->treeWidget_input->currentItem();
  if(currentItem!=NULL)
  {
    QTreeWidgetItem *topItem=findTopLevelItem(currentItem);
    qDebug()<<topItem->text(0);
    int row=ui->treeWidget_input->indexOfTopLevelItem(topItem);
    bool isShow=false;
    switch (row)
    {
    case ROW_INDEX_UNIT:
    {
      QTreeWidgetItem *addItem;
      int childCount=topItem->childCount();
      addItem=new QTreeWidgetItem(topItem);
      addItem->setText(0,tr("undefined_%1").arg(childCount));
      addItem->setText(1,QString::number(1,'f',3));
      ui->treeWidget_input->resizeColumnToContents(0);
    }
      break;
    case ROW_INDEX_VARIABLE:
    case ROW_INDEX_CONST:
      isShow=true;
      break;
    }
    if(isShow)
    {
      if(ui->stackedWidget->isHidden()==true)
      {
        ui->stackedWidget->show();
//        QRect rect=this->geometry();
//        this->setGeometry(rect.x(),rect.y(),rect.width()*2,rect.height());
        emit resizeGeometry(true);
      }
    }
  }
}
void CustomCurvePage::onBtnHideClicked()
{
  ui->stackedWidget->hide();
//  QRect rect=this->geometry();
//  this->setGeometry(rect.x(),rect.y(),rect.width()/2,rect.height());
  ui->btn_hide->hide();
  emit resizeGeometry(false);
}
void CustomCurvePage::onBtnRemoveClicked(void)
{
  QTreeWidgetItem *currentItem=ui->treeWidget_input->currentItem();
  if(currentItem!=NULL)
  {
    qDebug()<<"currentItem:"<<currentItem->text(0);
    if(!(currentItem->childCount()>0))
    {
      QTreeWidgetItem *topItem=findTopLevelItem(currentItem);
      int row=ui->treeWidget_input->indexOfTopLevelItem(topItem);
      bool isVarConst=false;
      qDebug()<<"row="<<row;
      switch (row)
      {
      case ROW_INDEX_UNIT:
        if(topItem->childCount()>1)
        {
          topItem->removeChild(currentItem);
          qDebug()<<"remove";
        }
        break;
      case ROW_INDEX_VARIABLE:
        if(topItem!=currentItem)
        {
          topItem->removeChild(currentItem);
          for(int i=0;i<topItem->childCount();i++)
          {
            topItem->child(i)->setText(1,tr("V%1").arg(i));
          }
          isVarConst=true;
          qDebug()<<"remove";
        }
        break;
      case ROW_INDEX_CONST:
        if(topItem!=currentItem)
        {
          topItem->removeChild(currentItem);
          for(int i=0;i<topItem->childCount();i++)
          {
            topItem->child(i)->setText(1,tr("C%1").arg(i));
          }
          isVarConst=true;
          qDebug()<<"remove";
        }
        break;
      }
      if(isVarConst)
      {
        QsciAPIs *apis=(QsciAPIs *)ui->textEdit->lexer()->apis();
        apis->clear();
        foreach (QString str, m_sciMathKeywordsList)
          apis->add(str);
        QTreeWidgetItem *item=ui->treeWidget_input->topLevelItem(ROW_INDEX_VARIABLE);
        for(int i=0;i<item->childCount();i++)
        {
          apis->add(item->child(i)->text(1));
        }
        item=ui->treeWidget_input->topLevelItem(ROW_INDEX_CONST);
        for(int i=0;i<item->childCount();i++)
        {
          apis->add(item->child(i)->text(1));
        }
        apis->prepare();
      }
    }
  }
}

void CustomCurvePage::onBtnNewClicked()
{
  QList<QTreeWidgetItem*> itemList;
  ui->lineEdit_curveName->clear();
  ui->lineEdit_curveName->setText("curve_xxx");
  for(int i=1;i<ui->treeWidget_input->topLevelItem(ROW_INDEX_UNIT)->childCount();i++)//从1开始，保留第一个
  {
    itemList.append(ui->treeWidget_input->topLevelItem(ROW_INDEX_UNIT)->child(i));
  }
  foreach (QTreeWidgetItem* item, itemList) {
    ui->treeWidget_input->topLevelItem(ROW_INDEX_UNIT)->removeChild(item);
  }
  ui->treeWidget_input->topLevelItem(ROW_INDEX_VARIABLE)->takeChildren();
  ui->treeWidget_input->topLevelItem(ROW_INDEX_CONST)->takeChildren();
  ui->textEdit->clear();
}

void CustomCurvePage::parserPattern(const QString &srcText,const QString &pattern, QStringList &list)
{
  QRegExp rx(pattern);
  int pos=rx.indexIn(srcText);
  qDebug()<<"pos="<<pos;
  while(pos!=-1)
  {
    int length=rx.matchedLength();
    list.append(rx.cap(1));
    pos=rx.indexIn(srcText,pos+length);
    qDebug()<<"pos="<<pos<<" length="<<length;
  }
  foreach (QString str, list) {
    qDebug()<<"cap="<<str;
  }
}

void CustomCurvePage::onBtnSaveClicked()
{
  int cFactorCount=0;
  int vFactorCount=0;
  QTreeWidgetItem *unitItem=ui->treeWidget_input->topLevelItem(ROW_INDEX_UNIT);
  QTreeWidgetItem *varItem=ui->treeWidget_input->topLevelItem(ROW_INDEX_VARIABLE);
  QTreeWidgetItem *conItem=ui->treeWidget_input->topLevelItem(ROW_INDEX_CONST);

  //没有V返回
  vFactorCount=varItem->childCount();
  cFactorCount=conItem->childCount();
  if(!(vFactorCount>0))
  {
    QMessageBox::information(0,tr("warnning"),tr("no variable"));
    return;
  }

  //简单语法分析
  //获得V变量，存在vlist中
  QString equationDescription;
  equationDescription=ui->textEdit->text();
  QString vPattern("(V\\d+)");
  QString cPattern("(C\\d+)");
  QStringList vlist;
  QStringList clist;
  parserPattern(equationDescription, vPattern,vlist);
  parserPattern(equationDescription, cPattern,clist);
  //个数检查
  if((vlist.count()!=vFactorCount)||(clist.count()!=cFactorCount))
  {
    QMessageBox::information(0,tr("error"),tr("parameters error\nplease check the variable and const variable "));
    return;
  }
  //所输入的变量在能不能在树中找到
  bool vlistOK=true;
  for(int i=0;i<vlist.count();i++)
  {
    QString str=vlist.at(i);
    bool vok=false;
    int index=0;
    while(index<vFactorCount)
    {
      if(varItem->child(index)->text(1)==str)
      {
        vok=true;
        qDebug()<<"find "<<str;
        break;
      }
      index++;
    }
    if(vok==false)
    {
      QMessageBox::information(0,tr("error"),tr("parameters error\nplease check the variable parameters"));
      vlistOK=false;
      break;
    }
  }
  if(vlistOK==false)
  {
    return;
  }

  bool clistOK=true;
  for(int i=0;i<clist.count();i++)
  {
    QString str=clist.at(i);
    bool cok=false;
    int index=0;
    while(index<cFactorCount)
    {
      if(conItem->child(index)->text(1)==str)
      {
        cok=true;
        qDebug()<<"find "<<str;
        break;
      }
      index++;
    }
    if(cok==false)
    {
      QMessageBox::information(0,tr("error"),tr("parameters error\nplease check the const parameters"));
      clistOK=false;
      break;
    }
  }
  if(clistOK==false)
  {
    return;
  }
  //检查是否重名
  bool hasSameName=false;
  for(int i=0;i<vlist.count();i++)
  {
    QString str=vlist.at(i);
    for(int j=i+1;j<vlist.count();j++)
    {
      QString nextStr=vlist.at(j);
      if(str==nextStr)
      {
        hasSameName=true;
        break;
      }
    }
  }
  if(hasSameName)
  {
    QMessageBox::information(0,tr("error"),tr("parameters error\nvariable parameters has the same name"));
    return;
  }
  hasSameName=false;
  for(int i=0;i<clist.count();i++)
  {
    QString str=clist.at(i);
    for(int j=i+1;j<clist.count();j++)
    {
      QString nextStr=clist.at(j);
      if(str==nextStr)
      {
        hasSameName=true;
        break;
      }
    }
  }
  if(hasSameName)
  {
    QMessageBox::information(0,tr("error"),tr("parameters error\nconst parameters has the same name"));
    return;
  }

  QString curveFullName;
  if(ui->lineEdit_note->text().isEmpty())
  {
    curveFullName.append("undefined note");
  }
  else
    curveFullName.append(ui->lineEdit_note->text());
  curveFullName.append(".");
  if(ui->lineEdit_curveName->text().isEmpty())
  {
    curveFullName.append("ucurve");
  }
  curveFullName.append(ui->lineEdit_curveName->text());
  m_curveItem->child(ROW_PRM_INDEX_FULLNAME)->setText(COL_CURVESETTING_INDEX_VALUE,curveFullName);
  //判断是什么类型script_const  script  根据V* C*  只有一个V就是script_const


  CurveType curveType=CURVE_TYPE_SCRIPT;
  if((vFactorCount==1)&&(cFactorCount==0))
    curveType=CURVE_TYPE_SCRIPT_CONST;
  //MultiValue增加单位
  m_curveItem->child(ROW_PRM_INDEX_MULTIVALUE)->takeChildren();

  for(int i=0;i<unitItem->childCount();i++)
  {
    qDebug()<<"add unit:"<<unitItem->child(i)->text(0);
    m_curveItem->child(ROW_PRM_INDEX_MULTIVALUE)->addChild(unitItem->child(i)->clone());
  }
  m_curveItem->child(ROW_PRM_INDEX_EQUATIONDESCRIPTION)->setText(COL_CURVESETTING_INDEX_VALUE,equationDescription);
  if(curveType==CURVE_TYPE_SCRIPT_CONST)
  {
    m_curveItem->setText(COL_CURVESETTING_INDEX_VALUE,"2");
    m_curveItem->child(ROW_PRM_INDEX_PRMFACTOR)->setText(COL_CURVESETTING_INDEX_VALUE,"1");
    QTreeWidgetItem *factorItem=m_curveItem->child(ROW_PRM_INDEX_PRMFACTOR)->child(0);
    QTreeWidgetItem *srcItem=varItem->child(0);
    factorItem->child(ROW_FACTOR_INDEX_FULLNAME)->setText(COL_CURVESETTING_INDEX_VALUE,srcItem->text(COL_NAME));
    int bytesNum=2;
    bytesNum=GlobalFunction::getBytesNumber(srcItem->text(COL_TYPE));
    factorItem->child(ROW_FACTOR_INDEX_BYTESNUM)->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(bytesNum));
    factorItem->child(ROW_FACTOR_INDEX_OFFSETADDR)->setText(COL_CURVESETTING_INDEX_VALUE,srcItem->text(COL_ADDRESS));
  }
  else
  {
    QTreeWidgetItem *prmFactorItem=m_curveItem->child(ROW_PRM_INDEX_PRMFACTOR);
    m_curveItem->setText(COL_CURVESETTING_INDEX_VALUE,"1");

    prmFactorItem->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(vFactorCount));
    QTreeWidgetItem *vfactor=prmFactorItem->child(0)->clone();
    QTreeWidgetItem *cfactor=prmFactorItem->child(1)->clone();
    vfactor->setText(COL_CURVESETTING_INDEX_VALUE,"1");
    cfactor->setText(COL_CURVESETTING_INDEX_VALUE,"0");
    prmFactorItem->takeChildren();
    for(int i=0;i<vFactorCount;i++)
    {
      QTreeWidgetItem *srcItem=varItem->child(i);
      int bytesNum=2;
      bytesNum=GlobalFunction::getBytesNumber(srcItem->text(COL_TYPE));
      vfactor->child(ROW_FACTOR_INDEX_FULLNAME)->setText(COL_CURVESETTING_INDEX_VALUE,srcItem->text(COL_NAME));
      vfactor->child(ROW_FACTOR_INDEX_BYTESNUM)->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(bytesNum));
      vfactor->child(ROW_FACTOR_INDEX_OFFSETADDR)->setText(COL_CURVESETTING_INDEX_VALUE,srcItem->text(COL_ADDRESS));
      prmFactorItem->addChild(vfactor->clone());
//      vfactor->child(ROW_FACTOR_INDEX_BASEADDR)->setText(COL_CURVESETTING_INDEX_VALUE,"0");//这个在主界面中修改
    }
    for(int i=0;i<cFactorCount;i++)
    {
      QTreeWidgetItem *srcItem=conItem->child(i);
      int bytesNum=2;
      bytesNum=GlobalFunction::getBytesNumber(srcItem->text(COL_TYPE));
      cfactor->child(ROW_FACTOR_INDEX_FULLNAME)->setText(COL_CURVESETTING_INDEX_VALUE,srcItem->text(COL_NAME));
      cfactor->child(ROW_FACTOR_INDEX_BYTESNUM)->setText(COL_CURVESETTING_INDEX_VALUE,QString::number(bytesNum));
      cfactor->child(ROW_FACTOR_INDEX_OFFSETADDR)->setText(COL_CURVESETTING_INDEX_VALUE,srcItem->text(COL_ADDRESS));
      prmFactorItem->addChild(cfactor->clone());
    }
    delete vfactor;
    delete cfactor;
  }
  emit addScriptCurve(m_curveItem);

  //提示一下增加成功状态
  ui->label_status->setText(tr("add curve:%1 successfully").arg(ui->lineEdit_curveName->text()));
  ui->label_status->show();
  QTimer::singleShot(1000,this,SLOT(onLabelStatusHide()));
}

void CustomCurvePage::onLabelStatusHide()
{
  ui->label_status->hide();
}

void CustomCurvePage::onTreeWidgetInputClicked(QTreeWidgetItem *item,int column)
{
  Q_UNUSED(column);
  QTreeWidgetItem *topItem=findTopLevelItem(item);
  int row=ui->treeWidget_input->indexOfTopLevelItem(topItem);
  if(row>0)
    ui->stackedWidget->setCurrentIndex(row-1);
}

void CustomCurvePage::onTreeWidgetInputDoubleClicked(QTreeWidgetItem *item,int column)
{
  if(!(item->childCount()>0))
  {
    QTreeWidgetItem *topItem=findTopLevelItem(item);
    int row=ui->treeWidget_input->indexOfTopLevelItem(topItem);
    if(row==0)
    {
      item->setFlags(item->flags()|Qt::ItemIsEditable);
      ui->treeWidget_input->editItem(item,column);
    }
  }
}
void CustomCurvePage::sciAddKeyword(QString str)
{
  QsciAPIs *apis=(QsciAPIs *)ui->textEdit->lexer()->apis();
  apis->add(str);
  apis->prepare();
}
void CustomCurvePage::initialCurveTreeWidgetItem()
{
  QTreeWidgetItem *itemLevel1;
  QTreeWidgetItem *itemLevel2;
  QTreeWidgetItem *itemLevel3;
  m_curveItem=new QTreeWidgetItem;
  m_curveItem->setText(0,"Row");
  m_curveItem->setText(1,"1");
  m_curveItem->setText(2,"CurveType 0:normal 1:script 2:script const");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"CurveId");
  itemLevel1->setText(1,"1");
  itemLevel1->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"IsDraw");
  itemLevel1->setText(1,"1");
  itemLevel1->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"FullName");
  itemLevel1->setText(1,"curve_xxx");
  itemLevel1->setText(2,"curve_xxx");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"Color");
  itemLevel1->setText(1,"NULL");
  itemLevel1->setText(2,"NULL");
  itemLevel2=new QTreeWidgetItem(itemLevel1);
  itemLevel2->setText(0,"ColorR");
  itemLevel2->setText(1,"255");
  itemLevel2->setText(2,"NULL");
  itemLevel2=new QTreeWidgetItem(itemLevel1);
  itemLevel2->setText(0,"ColorG");
  itemLevel2->setText(1,"0");
  itemLevel2->setText(2,"NULL");
  itemLevel2=new QTreeWidgetItem(itemLevel1);
  itemLevel2->setText(0,"ColorB");
  itemLevel2->setText(1,"0");
  itemLevel2->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"AxisNum");
  itemLevel1->setText(1,"0");
  itemLevel1->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"BytesNum");
  itemLevel1->setText(1,"2");
  itemLevel1->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"OffsetAddr");
  itemLevel1->setText(1,"-1");
  itemLevel1->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"BaseAddr");
  itemLevel1->setText(1,"-1");
  itemLevel1->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"AddValue");
  itemLevel1->setText(1,"0");
  itemLevel1->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"MultiValue");
  itemLevel1->setText(1,"1");
  itemLevel1->setText(2,"current unit value");
  itemLevel2=new QTreeWidgetItem(itemLevel1);
  itemLevel2->setText(0,"undefined");
  itemLevel2->setText(1,"1");
  itemLevel2->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"Xoffset");
  itemLevel1->setText(1,"0");
  itemLevel1->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"EquationDescription");
  itemLevel1->setText(1,"NULL");
  itemLevel1->setText(2,"NULL");

  itemLevel1=new QTreeWidgetItem(m_curveItem);
  itemLevel1->setText(0,"PrmFactor");
  itemLevel1->setText(1,"1");
  itemLevel1->setText(2,"VarCount");
  itemLevel2=new QTreeWidgetItem(itemLevel1);
  itemLevel2->setText(0,"Factor");
  itemLevel2->setText(1,"1");
  itemLevel2->setText(2,"PlotPrmFatorTypeID");
  itemLevel3=new QTreeWidgetItem(itemLevel2);
  itemLevel3->setText(0,"FullName");
  itemLevel3->setText(1,"NULL");
  itemLevel3->setText(2,"NULL");
  itemLevel3=new QTreeWidgetItem(itemLevel2);
  itemLevel3->setText(0,"BytesNum");
  itemLevel3->setText(1,"NULL");
  itemLevel3->setText(2,"NULL");
  itemLevel3=new QTreeWidgetItem(itemLevel2);
  itemLevel3->setText(0,"OffsetAddr");
  itemLevel3->setText(1,"NULL");
  itemLevel3->setText(2,"NULL");
  itemLevel3=new QTreeWidgetItem(itemLevel2);
  itemLevel3->setText(0,"BaseAddr");
  itemLevel3->setText(1,"NULL");
  itemLevel3->setText(2,"NULL");

  itemLevel2=new QTreeWidgetItem(itemLevel1);
  itemLevel2->setText(0,"Factor");
  itemLevel2->setText(1,"0");
  itemLevel2->setText(2,"PlotPrmFatorTypeID");
  itemLevel3=new QTreeWidgetItem(itemLevel2);
  itemLevel3->setText(0,"FullName");
  itemLevel3->setText(1,"NULL");
  itemLevel3->setText(2,"NULL");
  itemLevel3=new QTreeWidgetItem(itemLevel2);
  itemLevel3->setText(0,"BytesNum");
  itemLevel3->setText(1,"NULL");
  itemLevel3->setText(2,"NULL");
  itemLevel3=new QTreeWidgetItem(itemLevel2);
  itemLevel3->setText(0,"OffsetAddr");
  itemLevel3->setText(1,"NULL");
  itemLevel3->setText(2,"NULL");
  itemLevel3=new QTreeWidgetItem(itemLevel2);
  itemLevel3->setText(0,"BaseAddr");
  itemLevel3->setText(1,"NULL");
  itemLevel3->setText(2,"NULL");
}

void CustomCurvePage::onTreeWidgetVariableDoubleClicked(QTreeWidgetItem *item,int column)
{
  Q_UNUSED(column);
  if(!(item->childCount()>0))
  {
    int count=ui->treeWidget_input->topLevelItem(ROW_INDEX_VARIABLE)->childCount();
    QTreeWidgetItem *itemAdd=item->clone();
    QString str=tr("V%1").arg(count);
    itemAdd->setText(1,str);
    ui->treeWidget_input->topLevelItem(1)->addChild(itemAdd);
    sciAddKeyword(str);
    qDebug()<<"add "<<item->text(0);
    ui->treeWidget_input->resizeColumnToContents(0);
  }
}

void CustomCurvePage::onTreeWidgetConstDoubleClicked(QTreeWidgetItem *item,int column)
{
  Q_UNUSED(column);
  if(!(item->childCount()>0))
  {
    int count=ui->treeWidget_input->topLevelItem(ROW_INDEX_CONST)->childCount();
    QTreeWidgetItem *itemAdd=item->clone();
    QString str=tr("C%1").arg(count);
    itemAdd->setText(1,str);
    ui->treeWidget_input->topLevelItem(2)->addChild(itemAdd);
    sciAddKeyword(str);
    qDebug()<<"add "<<item->text(0);
    ui->treeWidget_input->resizeColumnToContents(0);
  }
}

void CustomCurvePage::onTreeWidgetItemExpanded(QTreeWidgetItem *item)
{
  item->treeWidget()->resizeColumnToContents(0);
}

QTreeWidgetItem* CustomCurvePage::findTopLevelItem(QTreeWidgetItem *item)
{
  QTreeWidgetItem* top=item;
  while(top->parent()!=NULL)
  {
    top=top->parent();
  }
  return top;
}
