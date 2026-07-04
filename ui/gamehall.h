#ifndef GAMEHALL_H
#define GAMEHALL_H

#include <QFrame>
#include <QWidget>

class GameHall : public QWidget
{
    Q_OBJECT

public:
    explicit GameHall(QWidget *parent = nullptr);

signals:
    void offlineModeRequested();
    void classicModeRequested();
    void friendModeRequested();
    void backToStartRequested();
    void settingsRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QFrame *createModeCard(const QString &title, const QString &subtitle, const QString &iconResource,
                           const QString &accent, bool enabled);
};

#endif // GAMEHALL_H
