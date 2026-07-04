#include "mainwindow.h"

#include "ui/classicgamewidget.h"

#include "ui/friendroomdialog.h"

#include "ui/gamehall.h"

#include "ui/settingsdialog.h"

#include "ui/startscene.h"

#include "ui/standalonegamewidget.h"

#include "ui/supersamplegraphicsview.h"

#include "core/appsettings.h"

#include "core/gameconstants.h"



#include <QDialog>

#include <QFrame>

#include <QGraphicsProxyWidget>

#include <QGraphicsScene>

#include <QKeyEvent>

#include <QResizeEvent>

#include <QShowEvent>

#include <QStackedWidget>

#include <QVBoxLayout>



MainWindow::MainWindow(QWidget *parent)

    : QMainWindow(parent)

{

    setWindowTitle(QStringLiteral("UCC 斗地主"));

    resize(GameConstants::StageWidth, GameConstants::StageHeight);

    setMinimumSize(960, 540);

    m_savedGeometry = geometry();



    m_stageRoot = new QWidget;

    m_stageRoot->setFixedSize(GameConstants::StageWidth, GameConstants::StageHeight);



    auto *stageLayout = new QVBoxLayout(m_stageRoot);

    stageLayout->setContentsMargins(0, 0, 0, 0);

    stageLayout->setSpacing(0);



    m_stack = new QStackedWidget(m_stageRoot);

    stageLayout->addWidget(m_stack);



    m_scene = new QGraphicsScene(this);

    m_scene->setSceneRect(0, 0, GameConstants::StageWidth, GameConstants::StageHeight);

    m_proxy = m_scene->addWidget(m_stageRoot);



    m_view = new SupersampleGraphicsView(m_scene, this);

    m_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_view->setFrameShape(QFrame::NoFrame);

    m_view->setBackgroundBrush(QColor(12, 28, 20));

    m_view->setDragMode(QGraphicsView::NoDrag);

    m_view->setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);

    setCentralWidget(m_view);



    showStartScene();

    applyDisplaySettings();

}



MainWindow::~MainWindow() = default;



void MainWindow::applyDisplaySettings()

{

    if (m_view)

        m_view->setSupersampleFactor(AppSettings::instance().supersampleFactor());



    if (AppSettings::instance().fullscreen()) {

        if (!isFullScreen()) {

            m_savedGeometry = geometry();

            showFullScreen();

        }

    } else if (isFullScreen()) {

        showNormal();

        if (m_savedGeometry.isValid())

            setGeometry(m_savedGeometry);

    }



    fitStage();

}



void MainWindow::toggleFullscreen()

{

    AppSettings::instance().setFullscreen(!isFullScreen());

    applyDisplaySettings();

}



void MainWindow::openSettings()

{

    SettingsDialog dialog(this);

    connect(&dialog, &SettingsDialog::settingsSaved, this, &MainWindow::applyDisplaySettings);

    dialog.exec();

}



void MainWindow::fitStage()

{

    if (!m_view || !m_scene)

        return;

    m_view->fitInView(QRectF(0, 0, GameConstants::StageWidth, GameConstants::StageHeight),

                      Qt::KeepAspectRatio);

}



void MainWindow::refreshStageLayout()

{

    if (!m_stageRoot || !m_stack || !m_view || !m_scene)

        return;

    m_stageRoot->setFixedSize(GameConstants::StageWidth, GameConstants::StageHeight);

    if (m_proxy)

        m_proxy->resize(GameConstants::StageWidth, GameConstants::StageHeight);

    m_scene->setSceneRect(0, 0, GameConstants::StageWidth, GameConstants::StageHeight);

    if (QWidget *page = m_stack->currentWidget())

        page->resize(GameConstants::StageWidth, GameConstants::StageHeight);

    fitStage();

    m_view->resetCachedContent();

    m_scene->update();

    m_view->viewport()->update();

}



void MainWindow::resizeEvent(QResizeEvent *event)

{

    QMainWindow::resizeEvent(event);

    fitStage();

}



void MainWindow::showEvent(QShowEvent *event)

{

    QMainWindow::showEvent(event);

    fitStage();

}



void MainWindow::keyPressEvent(QKeyEvent *event)

{

    if (event->key() == Qt::Key_F11) {

        toggleFullscreen();

        event->accept();

        return;

    }

    if (event->key() == Qt::Key_Escape && isFullScreen()) {

        AppSettings::instance().setFullscreen(false);

        applyDisplaySettings();

        event->accept();

        return;

    }

    QMainWindow::keyPressEvent(event);

}



void MainWindow::showStartScene()

{

    if (!m_startScene) {

        m_startScene = new StartScene(this);

        m_stack->addWidget(m_startScene);

        connect(m_startScene, &StartScene::startRequested, this, &MainWindow::showGameHall);

        connect(m_startScene, &StartScene::settingsRequested, this, &MainWindow::openSettings);

    }

    m_stack->setCurrentWidget(m_startScene);

    refreshStageLayout();

}



void MainWindow::showGameHall()

{

    if (m_friendWidget) {

        m_stack->removeWidget(m_friendWidget);

        delete m_friendWidget;

        m_friendWidget = nullptr;

    }

    if (m_classicWidget) {

        m_stack->removeWidget(m_classicWidget);

        delete m_classicWidget;

        m_classicWidget = nullptr;

    }

    if (m_gameWidget) {

        m_stack->removeWidget(m_gameWidget);

        delete m_gameWidget;

        m_gameWidget = nullptr;

    }



    if (!m_gameHall) {

        m_gameHall = new GameHall(this);

        m_stack->addWidget(m_gameHall);

        connect(m_gameHall, &GameHall::offlineModeRequested, this, &MainWindow::showStandaloneGame);

        connect(m_gameHall, &GameHall::classicModeRequested, this, &MainWindow::showClassicGame);

        connect(m_gameHall, &GameHall::friendModeRequested, this, &MainWindow::showFriendGame);

        connect(m_gameHall, &GameHall::backToStartRequested, this, &MainWindow::showStartScene);

        connect(m_gameHall, &GameHall::settingsRequested, this, &MainWindow::openSettings);

    }

    m_stack->setCurrentWidget(m_gameHall);

    refreshStageLayout();

}



void MainWindow::showStandaloneGame()

{

    if (m_gameWidget) {

        m_stack->removeWidget(m_gameWidget);

        delete m_gameWidget;

        m_gameWidget = nullptr;

    }



    m_gameWidget = new StandaloneGameWidget(this);

    m_stack->addWidget(m_gameWidget);

    connect(m_gameWidget, &StandaloneGameWidget::backToHallRequested, this, &MainWindow::showGameHall);

    connect(m_gameWidget, &StandaloneGameWidget::restartRequested, this, &MainWindow::showStandaloneGame);

    m_stack->setCurrentWidget(m_gameWidget);

    refreshStageLayout();

}



void MainWindow::showClassicGame()

{

    if (m_classicWidget) {

        m_stack->removeWidget(m_classicWidget);

        delete m_classicWidget;

        m_classicWidget = nullptr;

    }



    m_classicWidget = new ClassicGameWidget(ClassicGameWidget::RoomEntry::Classic, QString(), this);

    m_stack->addWidget(m_classicWidget);

    connect(m_classicWidget, &ClassicGameWidget::backToHallRequested, this, &MainWindow::showGameHall);

    connect(m_classicWidget, &ClassicGameWidget::restartRequested, this, &MainWindow::showClassicGame);

    m_stack->setCurrentWidget(m_classicWidget);

    refreshStageLayout();

}



void MainWindow::showFriendGame()

{

    FriendRoomDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted)

        return;



    if (m_friendWidget) {

        m_stack->removeWidget(m_friendWidget);

        delete m_friendWidget;

        m_friendWidget = nullptr;

    }



    const auto entry = dialog.selectedAction() == FriendRoomDialog::Action::CreateRoom

        ? ClassicGameWidget::RoomEntry::FriendCreate

        : ClassicGameWidget::RoomEntry::FriendJoin;



    m_friendWidget = new ClassicGameWidget(entry, dialog.roomId(), this);

    m_stack->addWidget(m_friendWidget);

    connect(m_friendWidget, &ClassicGameWidget::backToHallRequested, this, &MainWindow::showGameHall);

    connect(m_friendWidget, &ClassicGameWidget::restartRequested, this, &MainWindow::showFriendGame);

    m_stack->setCurrentWidget(m_friendWidget);

    refreshStageLayout();

}

