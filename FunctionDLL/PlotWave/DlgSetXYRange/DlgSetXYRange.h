#ifndef DLGSETXYRANGE_H
#define DLGSETXYRANGE_H

#include <QDialog>
#include <plotwave.h>
namespace Ui {
class DlgSetXYRange;
}

class DlgSetXYRange : public QDialog
{
    Q_OBJECT

public:
  explicit DlgSetXYRange(QWidget *parent = 0);
  ~DlgSetXYRange();
signals:
  void viewSetting(ViewSetting setting);
protected:
  void accept();
private:
    Ui::DlgSetXYRange *ui;
    ViewSetting m_setting;
};

#endif // DLGSETXYRANGE_H
