QT       += core gui multimedia network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += .

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    core/pokerutils.cpp \
    core/pokertypeutils.cpp \
    core/pokercompareutils.cpp \
    core/pokerseekutils.cpp \
    core/resourcemanager.cpp \
    core/appsettings.cpp \
    core/soundmanager.cpp \
    core/landlordbidengine.cpp \
    network/gamewebsocketclient.cpp \
    ui/cardwidget.cpp \
    ui/cardlayout.cpp \
    ui/cardhandwidget.cpp \
    ui/gameuikit.cpp \
    ui/gametableview.cpp \
    ui/gameplayhelper.cpp \
    ui/gameresultdialog.cpp \
    ui/cardflyanimator.cpp \
    ui/boomeffectanimator.cpp \
    ui/supersamplegraphicsview.cpp \
    ui/startscene.cpp \
    ui/gamehall.cpp \
    ui/settingsdialog.cpp \
    ui/friendroomdialog.cpp \
    ui/standalonegamewidget.cpp \
    ui/classicgamewidget.cpp

HEADERS += \
    mainwindow.h \
    core/poker.h \
    core/pokertype.h \
    core/gameconstants.h \
    core/roomprotocol.h \
    core/landlordbidengine.h \
    core/pokerutils.h \
    core/pokertypeutils.h \
    core/pokercompareutils.h \
    core/pokerseekutils.h \
    core/resourcemanager.h \
    core/appsettings.h \
    core/soundmanager.h \
    network/gamewebsocketclient.h \
    ui/cardwidget.h \
    ui/cardlayout.h \
    ui/cardhandwidget.h \
    ui/gameuikit.h \
    ui/gametableview.h \
    ui/gameplayhelper.h \
    ui/gameresultdialog.h \
    ui/cardflyanimator.h \
    ui/boomeffectanimator.h \
    ui/supersamplegraphicsview.h \
    ui/startscene.h \
    ui/gamehall.h \
    ui/settingsdialog.h \
    ui/friendroomdialog.h \
    ui/standalonegamewidget.h \
    ui/classicgamewidget.h

RESOURCES += \
    pocker.qrc

win32:RC_ICONS = image/poker.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
