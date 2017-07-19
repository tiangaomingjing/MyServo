#ifndef IOWIDGET_H
#define IOWIDGET_H

#include <QWidget>

namespace Ui {
class IOWidget;
}

class IOWidget : public QWidget
{
  Q_OBJECT
public:
  enum IO_Type{IO_INPUT,IO_OUTPUT};
  enum LineEdit_Status{LINEEDIT_STA_NORMAL,LINEEDIT_STA_RED,LINEEDIT_STA_YELLOW};

public:
  explicit IOWidget(IO_Type ioType,int index,QWidget *parent = 0);
  ~IOWidget();

  int index() const;
  IO_Type ioType() const;
  bool polarityStatus(void) const;
  int comboBoxCurrentIndex(void) const;
  void setUiEnabledStatus(bool enabled);
  void setPolarityEnable(bool enabled);
  void setUiPolarityStatus(bool negatived);
  void setPinDescription(QString &des);
  void setNoteDescription(const QString &note);
  void setComboBoxContents(QStringList &list);
  void setComboBoxCurrentIndex(int index);
  QString noteDescription(void);
  void setLineEditUiStatus(LineEdit_Status sta);
  void clearLineEnterFlag();

private slots:
  void onLineEditTextChanged();
  void onLineEditTextEnterClicked();
  void onLineEditSelectChanged();

signals:
  void ioEnableChanged(bool enabled);
  void ioComboBoxActived(int index);
  void ioPolarityChanged(bool checked);
  void ioNoteDescriptionSelectChanged();
  void ioNoteEnterPressed();

private:
  void initialUiStyleSheet(void);

private:
  Ui::IOWidget *ui;
  int m_index;
  IO_Type m_ioType;
  bool m_lineEditChangedFlag;
  bool m_lineEditEnterFlag;
};

#endif // IOWIDGET_H
