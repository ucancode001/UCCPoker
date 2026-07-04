#ifndef MAINWINDOW_H

#define MAINWINDOW_H



#include <QMainWindow>



class QGraphicsProxyWidget;

class QGraphicsScene;

class SupersampleGraphicsView;

class QStackedWidget;

class StartScene;

class GameHall;

class StandaloneGameWidget;

class ClassicGameWidget;

class QWidget;



class MainWindow : public QMainWindow

{

    Q_OBJECT



public:

    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();



public slots:

    void applyDisplaySettings();

    void toggleFullscreen();



protected:

    void resizeEvent(QResizeEvent *event) override;

    void showEvent(QShowEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;



private:

    void showStartScene();

    void showGameHall();

    void showStandaloneGame();

    void showClassicGame();

    void showFriendGame();

    void fitStage();

    void refreshStageLayout();

    void openSettings();



    SupersampleGraphicsView *m_view = nullptr;

    QGraphicsScene *m_scene = nullptr;

    QGraphicsProxyWidget *m_proxy = nullptr;

    QWidget *m_stageRoot = nullptr;

    QStackedWidget *m_stack = nullptr;

    StartScene *m_startScene = nullptr;

    GameHall *m_gameHall = nullptr;

    StandaloneGameWidget *m_gameWidget = nullptr;

    ClassicGameWidget *m_classicWidget = nullptr;

    ClassicGameWidget *m_friendWidget = nullptr;

    QRect m_savedGeometry;

};



#endif // MAINWINDOW_H

