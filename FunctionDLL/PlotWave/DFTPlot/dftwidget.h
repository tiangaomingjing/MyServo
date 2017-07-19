#ifndef DFTWIDGET_H
#define DFTWIDGET_H

#include <QWidget>
#include "plotwave_globaldef.h"

namespace Ui {
class DftWidget;
}
class PlotWave;
class DftAddForm;
class QTableWidgetItem;
class BoxZoom;
class ViewForm;

enum FFT_TYPE{FFT_SIGNAL,FFT_SYSTEM};
typedef struct _fftAnalyzeInfo
{
  FFT_TYPE fftType;
  int curveSelectIndex;
  int curveSelectIndexOut;
}FFTAnalyzeInfo;

class  DftWidget : public QWidget
{
  Q_OBJECT

public:
  explicit DftWidget(PlotWave *plot=0,QWidget *parent = 0);
  ~DftWidget();
  typedef struct _fftResult
  {
    QVector<double>freq;
    QVector<double>am;
    QVector<double>ph;
  }FFTResult;

  void setComboxContexts(QStringList &stringList);
  void setPhaseValueStyle(int style);
  void setTimeRange(double min,double max);

signals:
  void dftWidgetClose();
  void saveSourceCurve(QString &fileName, QVector<quint16> &curveIndexVector);
public slots:
  void onFFTStartTimeChanged(double value);
  void onFFTEndTimeChanged(double value);
  void onComboxInputClicked(int index);

protected:
  void closeEvent(QCloseEvent *event);
private slots:
  void onBtnAllClicked();
  void onBtnAddClicked(void);
  void onBtnClearClicked();
  void onBtnRemoveClicked();
  void onBtnAmpClicked(bool checked);
  void onBtnPhaClicked(bool checked);
  void onBtnFitAllClicked();
  void onBtnOpenClicked();
  void onBtnSaveClicked();
  void onBtnViewClicked();
  void onRadioSignalClicked(bool checked);
  void onRadioSystemClicked(bool checked);

  void onPlotAmplitudeMouseMoved(QMouseEvent *event);
  void onPlotAmplitudeMousePressed(QMouseEvent *event);
  void onPlotAmplitudeMouseReleased(QMouseEvent *event);
  void onPlotPhaseMouseMoved(QMouseEvent *event);
  void onPlotPhaseMousePressed(QMouseEvent *event);
  void onPlotPhaseMouseReleased(QMouseEvent *event);

  void onTableItemClicked(QTableWidgetItem * item);
  void onTableWidgetCellEnteredMoreDetail(int row, int col);

  void onSetPlotViewRange(QVector<double> &settings);

private :
  void setFFTAnalyzeInformation(FFTAnalyzeInfo &fftinfo){m_fftInformation=fftinfo;}
  FFTAnalyzeInfo & getFFTAnalyzeInformation(){return m_fftInformation;}
  void fftAnalyze(FFTAnalyzeInfo &fftInfo);
  void addCurve(quint16 id,QString &name,QString &fullName, QString &note, QColor &color, bool isShow, FFTResult &fftData);

private:
  Ui::DftWidget *ui;
  PlotWave *mp_plotWave;
  DftAddForm *m_addForm;
  FFTAnalyzeInfo m_fftInformation;
  QVector<bool>m_curveShowVector;//记录显示状态
  QStringList m_curveFullName;//记录全名
  FFTResult m_fftResult;
  double m_piRatio;
  BoxZoom *m_boxZoom1;
  BoxZoom *m_boxZoom2;
  ViewForm *m_viewForm;

};

#endif // DFTWIDGET_H
