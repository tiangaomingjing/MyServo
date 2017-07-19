#include "iowidget.h"
#include "ui_iowidget.h"
#include <QDebug>

IOWidget::IOWidget(IO_Type ioType,int index,QWidget *parent) :
  QWidget(parent),
  m_ioType(ioType),
  m_index(index),
  m_lineEditChangedFlag(false),
  m_lineEditEnterFlag(false),
  ui(new Ui::IOWidget)
{
  ui->setupUi(this);

  initialUiStyleSheet();

  ui->radioButton->setChecked(false);
  ui->checkBox->setChecked(false);
  ui->checkBox->setText(tr("Polarity reverse"));

  if(ioType==IOWidget::IO_INPUT)
    ui->radioButton->setText(tr("input_%1").arg(index));
  else
    ui->radioButton->setText(tr("output_%1").arg(index));

  connect(ui->radioButton,SIGNAL(clicked(bool)),this,SIGNAL(ioEnableChanged(bool)));
  connect(ui->comboBox,SIGNAL(activated(int)),this,SIGNAL(ioComboBoxActived(int)));
  connect(ui->checkBox,SIGNAL(clicked(bool)),this,SIGNAL(ioPolarityChanged(bool)));
//  connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(onBtnSaveTextClicked()));
  connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(onLineEditTextChanged()));
  connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(onLineEditTextEnterClicked()));
  connect(ui->lineEdit,SIGNAL(editingFinished()),this,SLOT(onLineEditSelectChanged()));
}

IOWidget::~IOWidget()
{
  delete ui;
}

int IOWidget::index() const
{
  return m_index;
}

IOWidget::IO_Type IOWidget::ioType() const
{
  return m_ioType;
}
bool IOWidget::polarityStatus() const
{
  return ui->checkBox->isChecked();
}
int IOWidget::comboBoxCurrentIndex() const
{
  return ui->comboBox->currentIndex();
}
void IOWidget::setUiEnabledStatus(bool enabled)
{
  QString str;
  if(!(ui->checkBox->isChecked()))
  {
    str="QRadioButton::indicator::unchecked {\
        image: url(:/led_off.png);\
    }\
    QRadioButton::indicator::checked {\
        image: url(:/led_on.png);\
    }";
  }
  else
  {
    str="QRadioButton::indicator::checked {\
        image: url(:/led_off.png);\
    }\
    QRadioButton::indicator::unchecked {\
        image: url(:/led_on.png);\
    }";
  }
  ui->radioButton->setStyleSheet(str);
  ui->radioButton->setChecked(enabled);
}

void IOWidget::setPolarityEnable(bool enabled)
{
  ui->checkBox->setEnabled(enabled);
}

void IOWidget::setUiPolarityStatus(bool negatived)
{
  ui->checkBox->setChecked(negatived);
}

void IOWidget::setPinDescription(QString &des)
{
  ui->label->setText(des);
}

void IOWidget::setNoteDescription(const QString &note)
{
  disconnect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(onLineEditTextChanged()));
  ui->lineEdit->setText(note);
  connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(onLineEditTextChanged()));
}

void IOWidget::setComboBoxContents(QStringList &list)
{
  ui->comboBox->clear();
  ui->comboBox->addItems(list);
}
void IOWidget::setComboBoxCurrentIndex(int index)
{
  ui->comboBox->setCurrentIndex(index);
}
QString IOWidget::noteDescription()
{
  return ui->lineEdit->text();
}

void IOWidget::onLineEditTextChanged()
{
  m_lineEditChangedFlag=true;
  setLineEditUiStatus(LINEEDIT_STA_RED);
}

void IOWidget::onLineEditTextEnterClicked()
{
//  if(m_lineEditChangedFlag)
//  {
//    setLineEditUiStatus(LINEEDIT_STA_YELLOW);
//    m_lineEditChangedFlag=false;
//    emit ioNoteEnterPressed();
//  }
  setLineEditUiStatus(LINEEDIT_STA_YELLOW);
  m_lineEditChangedFlag=false;
  emit ioNoteEnterPressed();
  m_lineEditEnterFlag=true;
  qDebug()<<"enter";
}
void IOWidget::onLineEditSelectChanged()
{
  if(!m_lineEditEnterFlag&&m_lineEditChangedFlag)
  {
    emit ioNoteDescriptionSelectChanged();
    setLineEditUiStatus(LINEEDIT_STA_NORMAL);
    qDebug()<<"select changed "<<"no enter "<<"require recover";
  }
//  m_lineEditEnterFlag=false;
  m_lineEditChangedFlag=false;

}

void IOWidget::setLineEditUiStatus(LineEdit_Status sta)
{
  QString     str;

  switch (sta)
  {
  case LINEEDIT_STA_NORMAL:
    str="QLineEdit:!hover {\
        border-radius: 10px;\
        padding: 0 2px;\
        background: transparent;\
        color:black;\
        selection-background-color: blue;\
    }\
    QLineEdit:hover {\
          border: 2px solid lightgray;\
          border-radius: 10px;\
          padding: 0 2px;\
          background: white;\
          color:black;\
          selection-background-color: blue;\
      }";
    break;
  case LINEEDIT_STA_YELLOW:
    str="QLineEdit:!hover {\
        border: 2px solid lightgray;\
        border-radius: 10px;\
        padding: 0 2px;\
        background: yellow;\
        color:black;\
        selection-background-color: blue;\
    }\
    QLineEdit:hover {\
          border: 2px solid lightgray;\
          border-radius: 10px;\
          padding: 0 2px;\
          background: white;\
          color:black;\
          selection-background-color: blue;\
      }";
    break;
case LINEEDIT_STA_RED:
  str="QLineEdit:!hover {\
      border-radius: 10px;\
      padding: 0 2px;\
      background: transparent;\
      color:red;\
      selection-background-color: blue;\
  }\
  QLineEdit:hover {\
        border: 2px solid lightgray;\
        border-radius: 10px;\
        color:red;\
        padding: 0 2px;\
        background: white;\
        selection-background-color: blue;\
    }";
  break;
  }
  ui->lineEdit->setStyleSheet(str);
}
void IOWidget::clearLineEnterFlag()
{
  m_lineEditEnterFlag=false;
}

void IOWidget::initialUiStyleSheet()
{
  QString str="  QRadioButton::indicator::unchecked {\
      image: url(:/led_off.png);\
  }\
  QRadioButton::indicator::checked {\
      image: url(:/led_on.png);\
  }";
  ui->radioButton->setStyleSheet(str);

  str="QCheckBox::indicator:unchecked {\
      image: url(:/unchecked.png);\
  }\
  QCheckBox::indicator:checked {\
      image: url(:/checked.png);\
  }";
  ui->checkBox->setStyleSheet(str);

  str="QComboBox {\
      border: 1px solid gray;\
      border-radius: 3px;\
      padding: 1px 18px 1px 3px;\
      min-width: 6em;\
  }\
  QComboBox:on { /* shift the text when the popup opens */\
        padding-top: 3px;\
        padding-left: 4px;\
    }\
  QComboBox:editable {\
                  background: white;\
                  height:30px;\
              }\
  ";
//  ui->comboBox->setStyleSheet(str);
  setLineEditUiStatus(LINEEDIT_STA_NORMAL);
  ui->lineEdit->setMinimumHeight(30);
}
