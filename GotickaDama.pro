#-------------------------------------------------
#
# Project created by QtCreator 2017-07-18T04:33:46
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GotickaDama
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Main.cpp \
    ChessPiece.cpp \
    GameBoard.cpp \
    GameGothicCheckers.cpp \
    GameGothicCheckersAI.cpp \
    GameManager.cpp \
    Main.cpp \
    Player.cpp \
    BoardGraphicScene.cpp \
    MainWindow.cpp \
    SettingsDialog.cpp \
    TreeNode.cpp

HEADERS += \
    ChessPiece.h \
    Game.h \
    GameAI.h \
    GameBoard.h \
    GameExceptions.h \
    GameGothicCheckers.h \
    GameGothicCheckersAI.h \
    GameManager.h \
    Player.h \
    BoardGraphicScene.h \
    MainWindow.h \
    SettingsDialog.h \
    TreeNode.h

FORMS += \
    MainWindow.ui \
    SettingsDialog.ui

RESOURCES += \
    Resources.qrc

DISTFILES += \
    Resources.rc

RC_FILE += \
    Resources.rc
