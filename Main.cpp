#include <QApplication>

#include "MainWindow.h"
#include "GameManager.h"
#include "GameBoard.h"
#include "GameGothicCheckers.h"
#include "GameGothicCheckersAI.h"
#include "GameExceptions.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    sGameManager->SetGameType(GAME_GOTHIC_CHECKERS);
    sGameManager->Initialize();

    MainWindow w;
    w.show();
    w.SpawnPieces(sGameManager->GetBoard()->GetAllChessPieces());

    return a.exec();
}
