#ifndef COMPLETELINEEDIT_H
#define COMPLETELINEEDIT_H
#include <QLineEdit>
#include <QStringList>
class QListView;
class QStringListModel;
class QModelIndex;
class CompleteLineEdit : public QLineEdit {
    Q_OBJECT
public:
    QStringList words; // 整个完成列表的单词
    int  ModelFlag;
    CompleteLineEdit(QWidget *parent = 0);
public slots:
    void setCompleter(const QString &text); // 动态的显示完成列表
    void completeText(const QModelIndex &index); // 点击完成列表中的项，使用此项自动完成输入的单词
    void replyMove();
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
private:

    QListView *listView; // 完成列表
    QStringListModel *model; // 完成列表的model
signals:
    void modelChanged();
};
#endif // COMPLETELINEEDIT_H
