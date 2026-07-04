#ifndef GAMERESULTDIALOG_H
#define GAMERESULTDIALOG_H

#include <QDialog>

class GameResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameResultDialog(bool victory, const QString &subtitle, QWidget *parent = nullptr);

signals:
    void restartRequested();
    void leaveRequested();

private:
    void setupUi(bool victory, const QString &subtitle);
};

#endif // GAMERESULTDIALOG_H
