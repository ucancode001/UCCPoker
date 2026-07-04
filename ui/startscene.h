#ifndef STARTSCENE_H
#define STARTSCENE_H

#include <QWidget>

class QPushButton;

class StartScene : public QWidget
{
    Q_OBJECT

public:
    explicit StartScene(QWidget *parent = nullptr);

signals:
    void startRequested();
    void settingsRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPushButton *m_startButton = nullptr;
};

#endif // STARTSCENE_H
