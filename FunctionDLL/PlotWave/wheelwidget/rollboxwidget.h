#ifndef ROLLBOXWIDGET_H
#define ROLLBOXWIDGET_H

#include <QWidget>
class RollBoxWidgetPrivate;
class QPushButton;
class RollBoxWidget : public QWidget
{
  Q_OBJECT
public:
  explicit RollBoxWidget(int axisCount,const QStringList &strList,QWidget *parent = 0);
  ~RollBoxWidget(void);

  QStringList items() const;
  void setItems( const QStringList &items );
  int currentIndexAt(int axisIndex) const;
  void setCurrentIndexAt(int index,int axisIndex);
  void setButtonTextAt(const QString &text,int index);
  bool buttonIsChecked(int axis) const;
  int rollCount(void) const;
  void appendBox(void);
  void removeLastBox(void);
  void setButtonChecked(int axis);

signals:
  void stopAt(int axis,int index);
  void hoverAt(int axis,int index);
  void clickedAt(int axis);
  void dragIndexAt(int axis,int index);

public slots:

private slots:
  void onHover(int index);
  void onStopped(int index);
  void onBtnClicked(void);
  void onDragIndex(int index);

protected:
  void keyPressEvent(QKeyEvent *e)Q_DECL_OVERRIDE;
  void keyReleaseEvent(QKeyEvent *e)Q_DECL_OVERRIDE;

private :
  void setSingleButtonActived(QPushButton *btn);
  void setUiStyleSheet(void);
  QStringList showList(int size);

private:
  RollBoxWidgetPrivate *const d_ptr;

};

#endif // ROLLBOXWIDGET_H
