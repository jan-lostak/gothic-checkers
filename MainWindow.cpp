#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SettingsDialog.h"

#include <QMessageBox>
#include <QPixMap>
#include <QTextEdit>
#include <QResource>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    _scene = new BoardGraphicScene(this);
    _ui->graphicsView->setScene(_scene);

    _gameSettingsInfo = new QLabel();
    _ui->statusBar->addWidget(_gameSettingsInfo);

    _gameDiffucultyInfo = new QLabel();
    _ui->statusBar->addWidget(_gameDiffucultyInfo);

    _gameCurrentPlayer = new QLabel();
    _gameCurrentPlayer->setText("Na tahu je <b>Bílý</b>&nbsp;");
    _ui->statusBar->addWidget(_gameCurrentPlayer);

    _gameFeedback = new QLabel();
    _gameFeedback->setText("<span style='color: blue;'>Proveďte tah</span>&nbsp;");
    _ui->statusBar->addWidget(_gameFeedback);

    UpdateGameSettingStatuBar();
    UpdateContextMenuControls();
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _scene;
}

void MainWindow::UpdateTurnsHistoryView()
{
    _ui->listWidget->model()->removeRows(1, _ui->listWidget->model()->rowCount() - 1);
    BoardHistory& history = sGameManager->GetBoard()->GetBoardHistory();

    QPixmap chessPieces(GAME_BOARD_CHESS_PIECES);

    for (const HistoryItem &item : history)
    {
        int movesCounter;
        std::string stringifiedTurn;
        std::vector<ChessPiece> piecesBefore;
        std::vector<ChessPiece> piecesAfter;
        PieceColor color;

        std::tie(stringifiedTurn, movesCounter, piecesBefore, piecesAfter, color) = item;

        QIcon icon(chessPieces.copy(PIECE_TYPE_PAWN * PIECE_DIMENSIONS, color * PIECE_DIMENSIONS, PIECE_DIMENSIONS, PIECE_DIMENSIONS));
        QListWidgetItem* listItem = new QListWidgetItem(icon, stringifiedTurn.c_str());

        _ui->listWidget->addItem(listItem);
    }
}

void MainWindow::UpdateGameSettingStatuBar()
{
    Player* black = sGameManager->GetBlackPlayer();
    Player* white = sGameManager->GetWhitePlayer();

    const char* difficulties[4] = {"Začátečník", "Pokročilý", "Mistr", "Velmistr"};
    const char* types[2] = {"Člověk", "Počítač"};

    std::stringstream s;

    s << "Bílý <b>" << difficulties[white->GetGameDifficulty()] << "</b> ";
    s << "Černý <b>" << difficulties[black->GetGameDifficulty()] << "</b>&nbsp;";

    _gameDiffucultyInfo->setText(s.str().c_str());

    s.str(std::string());

    s << "Bílý <b>" << types[white->GetPlayerType()] << "</b> ";
    s << "Černý <b>" << types[black->GetPlayerType()] << "</b>&nbsp;";

    _gameSettingsInfo->setText(s.str().c_str());
}

void MainWindow::UpdateWhosTurn()
{
    const char* players[2] = {"Černý", "Bílý"};
    std::stringstream s;

    s << "Na tahu je <b>" << players[sGameManager->GetPlayerOnTurn()->GetPlayerColor()] << "</b>&nbsp;";

    _gameCurrentPlayer->setText(s.str().c_str());
}

void MainWindow::UpdateContextMenuControls()
{
    // Nápověda nejlepšího tahu
    bool bestTurnEnabled = true;
    bestTurnEnabled = !(sGameManager->GetBlackPlayer()->IsComputer() && sGameManager->GetWhitePlayer()->IsComputer());
    bestTurnEnabled = sGameManager->GetPlayerOnTurn()->IsHuman();

    _ui->actionNapovedaNejlepsihoTahu->setEnabled(bestTurnEnabled);

    // Zpět
    _ui->actionVratitTah->setEnabled(sGameManager->GetBoard()->GetBoardHistory().size() > 0);

    // Znovu
    _ui->actionZnovu->setEnabled(sGameManager->GetBoard()->GetBoardRedoHistory().size() > 0);
}

void MainWindow::SetGameFeedbackText(const QString& str)
{
    _gameFeedback->setText(str);
}

void MainWindow::SpawnPieces(std::vector<ChessPiece*> pieces)
{
    for (ChessPiece* piece : pieces)
        _scene->AddChessPiece(piece);
}

void MainWindow::OpenLoadGameFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Otevřít uloženou hru"), "", tr("Save files (*.save);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    else
    {
        _saveFile = fileName;

        QFile file (fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, tr("Nelze otevřít požadovaný soubor."), file.errorString());
            return;
        }

        try
        {
            QDataStream in(&file);
            in.setVersion(QDataStream::Qt_5_9);

            // Hlavička
            QString header;
            in >> header;

            if (header != QString(SAVE_FILE_HEADER))
                throw std::exception();

            // Hráči
            Player whitePlayer;
            Player blackPlayer;

            int type;
            int difficulty;
            int color;

            if (in.atEnd())
                throw std::exception();

            in >> type;
            in >> difficulty;
            in >> color;

            whitePlayer.SetPlayerType(PlayerType(type));
            whitePlayer.SetGameDifficulty(GameDifficulty(difficulty));
            whitePlayer.SetPlayerColor(PieceColor(color));

            if (in.atEnd())
                throw std::exception();

            in >> type;
            in >> difficulty;
            in >> color;

            blackPlayer.SetPlayerType(PlayerType(type));
            blackPlayer.SetGameDifficulty(GameDifficulty(difficulty));
            blackPlayer.SetPlayerColor(PieceColor(color));

            // Hráč na tahu
            int playerOnTurnColor;

            if (in.atEnd())
                throw std::exception();

            in >> playerOnTurnColor;

            // Figurky
            std::vector<ChessPiece> pieces;

            if (in.atEnd())
                throw std::exception();

            size_t piecesCount;
            in >> piecesCount;

            for (int i = 0; i < piecesCount; i++)
            {
                if (in.atEnd())
                    throw std::exception();

                ChessPiece piece;

                int color;
                int type;
                int row;
                int col;

                in >> color;
                in >> type;
                in >> row;
                in >> col;

                piece.SetPieceColor(PieceColor(color));
                piece.SetPieceType(PieceType(type));
                piece.SetPiecePosition(Position(row, col));

                pieces.push_back(piece);
            }

            // Historie tahů
            BoardHistory history;
            BoardHistory historyRedo;

            if (in.atEnd())
                throw std::exception();

            size_t historyEntriesCount;
            size_t historyRedoEntriesCount;
            in >> historyEntriesCount;
            in >> historyRedoEntriesCount;

            for (int i = 0; i < historyEntriesCount; i++)
            {
                if (in.atEnd())
                    throw std::exception();

                char* stringifiedTurn;
                int movesCounter;
                std::vector<ChessPiece> piecesBefore;
                std::vector<ChessPiece> piecesAfter;
                size_t piecesAfterCount;
                size_t piecesBeforeCount;
                int color;

                in >> stringifiedTurn;
                in >> movesCounter;
                in >> color;
                in >> piecesAfterCount;
                in >> piecesBeforeCount;

                for (int j = 0; j < piecesAfterCount; j++)
                {
                    if (in.atEnd())
                        throw std::exception();

                    ChessPiece piece;

                    int color;
                    int type;
                    int row;
                    int col;

                    in >> color;
                    in >> type;
                    in >> row;
                    in >> col;

                    piece.SetPieceColor(PieceColor(color));
                    piece.SetPieceType(PieceType(type));
                    piece.SetPiecePosition(Position(row, col));

                    piecesAfter.push_back(piece);
                }

                for (int j = 0; j < piecesBeforeCount; j++)
                {
                    if (in.atEnd())
                        throw std::exception();

                    ChessPiece piece;

                    int color;
                    int type;
                    int row;
                    int col;

                    in >> color;
                    in >> type;
                    in >> row;
                    in >> col;

                    piece.SetPieceColor(PieceColor(color));
                    piece.SetPieceType(PieceType(type));
                    piece.SetPiecePosition(Position(row, col));

                    piecesBefore.push_back(piece);
                }

                history.push_back({ std::string(stringifiedTurn), movesCounter, piecesBefore, piecesAfter, PieceColor(color) });
            }

            for (int i = 0; i < historyRedoEntriesCount; i++)
            {
                if (in.atEnd())
                    throw std::exception();

                char* stringifiedTurn;
                int movesCounter;
                std::vector<ChessPiece> piecesBefore;
                std::vector<ChessPiece> piecesAfter;
                size_t piecesAfterCount;
                size_t piecesBeforeCount;
                int color;

                in >> stringifiedTurn;
                in >> movesCounter;
                in >> color;
                in >> piecesAfterCount;
                in >> piecesBeforeCount;

                for (int j = 0; j < piecesAfterCount; j++)
                {
                    if (in.atEnd())
                        throw std::exception();

                    ChessPiece piece;

                    int color;
                    int type;
                    int row;
                    int col;

                    in >> color;
                    in >> type;
                    in >> row;
                    in >> col;

                    piece.SetPieceColor(PieceColor(color));
                    piece.SetPieceType(PieceType(type));
                    piece.SetPiecePosition(Position(row, col));

                    piecesAfter.push_back(piece);
                }

                for (int j = 0; j < piecesBeforeCount; j++)
                {
                    if (in.atEnd())
                        throw std::exception();

                    ChessPiece piece;

                    int color;
                    int type;
                    int row;
                    int col;

                    in >> color;
                    in >> type;
                    in >> row;
                    in >> col;

                    piece.SetPieceColor(PieceColor(color));
                    piece.SetPieceType(PieceType(type));
                    piece.SetPiecePosition(Position(row, col));

                    piecesBefore.push_back(piece);
                }

                historyRedo.push_back({ std::string(stringifiedTurn), movesCounter, piecesBefore, piecesAfter, PieceColor(color) });
            }

            // Načtení uložených dat do hry
            Player* whitePlayerPtr = sGameManager->GetWhitePlayer();

            whitePlayerPtr->SetGameDifficulty(whitePlayer.GetGameDifficulty());
            whitePlayerPtr->SetPlayerType(whitePlayer.GetPlayerType());

            Player* blackPlayerPtr = sGameManager->GetBlackPlayer();

            blackPlayerPtr->SetGameDifficulty(blackPlayer.GetGameDifficulty());
            blackPlayerPtr->SetPlayerType(blackPlayer.GetPlayerType());

            sGameManager->SetColorOnTurn(PieceColor(playerOnTurnColor));
            sGameManager->GetBoard()->SetPieces(pieces);

            for (const HistoryItem &item : history)
                sGameManager->GetBoard()->AddBoardHistoryItem(item);

            for (const HistoryItem &item : historyRedo)
                sGameManager->GetBoard()->AddBoardHistoryRedoItem(item);

            UpdateGameSettingStatuBar();
            UpdateWhosTurn();
            UpdateTurnsHistoryView();

            _scene->ReDraw();
            sGameManager->SetGameChanged(false);

            UpdateContextMenuControls();
        }
        catch (std::exception e)
        {
            QMessageBox::information(this, tr("Neplatný soubor"), tr("Soubor je poškozen nebo není platným souborem hry gotická dáma."));
        }
    }
}

void MainWindow::OpenSaveGameToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Uložit hru"), "", tr("Save files (*.save)"));

    if (fileName.isEmpty())
        return;
    else
    {
        _saveFile = fileName;
        SaveToFile(fileName);
        sGameManager->SetGameChanged(false);
    }
}

void MainWindow::SaveToFile(QString& path)
{
    QFile file (path);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Nelze otevřít požadovaný soubor."), file.errorString());
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_9);

    // Hlavička
    out << QString(SAVE_FILE_HEADER);

    // Hráči
    Player* whitePlayer = sGameManager->GetWhitePlayer();
    Player* blackPlayer = sGameManager->GetBlackPlayer();

    out << whitePlayer->GetPlayerType();
    out << whitePlayer->GetGameDifficulty();
    out << whitePlayer->GetPlayerColor();

    out << blackPlayer->GetPlayerType();
    out << blackPlayer->GetGameDifficulty();
    out << blackPlayer->GetPlayerColor();

    // Hráč na tahu
    out << sGameManager->GetPlayerOnTurn()->GetPlayerColor();

    // Figurky
    std::vector<ChessPiece*> pieces = sGameManager->GetBoard()->GetAllChessPieces();
    out << pieces.size();

    for (ChessPiece* piece: pieces)
    {
        out << piece->GetPieceColor();
        out << piece->GetPieceType();
        out << piece->GetPiecePosition().row;
        out << piece->GetPiecePosition().col;
    }

    // Historie tahů
    BoardHistory& history = sGameManager->GetBoard()->GetBoardHistory();
    BoardHistory& historyRedo = sGameManager->GetBoard()->GetBoardRedoHistory();
    out << history.size();
    out << historyRedo.size();

    for (const HistoryItem &item : history)
    {
        std::string stringifiedTurn;
        int movesCounter;
        std::vector<ChessPiece> piecesBefore;
        std::vector<ChessPiece> piecesAfter;
        PieceColor color;

        std::tie(stringifiedTurn, movesCounter, piecesBefore, piecesAfter, color) = item;

        out << stringifiedTurn.c_str();
        out << movesCounter;
        out << color;

        out << piecesAfter.size();
        out << piecesBefore.size();

        for (ChessPiece& piece: piecesAfter)
        {
            out << piece.GetPieceColor();
            out << piece.GetPieceType();
            out << piece.GetPiecePosition().row;
            out << piece.GetPiecePosition().col;
        }

        for (ChessPiece& piece: piecesBefore)
        {
            out << piece.GetPieceColor();
            out << piece.GetPieceType();
            out << piece.GetPiecePosition().row;
            out << piece.GetPiecePosition().col;
        }
    }

    for (const HistoryItem &item : historyRedo)
    {
        std::string stringifiedTurn;
        int movesCounter;
        std::vector<ChessPiece> piecesBefore;
        std::vector<ChessPiece> piecesAfter;
        PieceColor color;

        std::tie(stringifiedTurn, movesCounter, piecesBefore, piecesAfter, color) = item;

        out << stringifiedTurn.c_str();
        out << movesCounter;
        out << color;

        out << piecesAfter.size();
        out << piecesBefore.size();

        for (ChessPiece& piece: piecesAfter)
        {
            out << piece.GetPieceColor();
            out << piece.GetPieceType();
            out << piece.GetPiecePosition().row;
            out << piece.GetPiecePosition().col;
        }

        for (ChessPiece& piece: piecesBefore)
        {
            out << piece.GetPieceColor();
            out << piece.GetPieceType();
            out << piece.GetPiecePosition().row;
            out << piece.GetPiecePosition().col;
        }
    }

    SetGameFeedbackText("<span style='color: green;'>Hra byla úspěšně uložena.</span>&nbsp;");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!sGameManager->HasGameChanged())
    {
        event->accept();
        return;
    }

    event->ignore();

    QMessageBox msgBox(this);
    msgBox.setText("<b>Máte rozehranou hru!</b>");
    msgBox.setInformativeText("Přejete si uložit vaši hru?");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    msgBox.setButtonText(QMessageBox::Save, tr("Uložit"));
    msgBox.setButtonText(QMessageBox::Discard, tr("Neukládat"));
    msgBox.setButtonText(QMessageBox::Cancel, tr("Storno"));

    int ret = msgBox.exec();

    switch (ret)
    {
        case QMessageBox::Save:
            OpenSaveGameToFile();
            event->accept();
            break;
        case QMessageBox::Discard:
            event->accept();
            break;
        case QMessageBox::Cancel:
            break;
        default:
            break;
    }
}

void MainWindow::on_actionOProgramu_triggered()
{
    QMessageBox msgBox(this);
    msgBox.setIconPixmap(QPixmap(":/images/icons/chess.png").scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    msgBox.setWindowTitle("O programu gotická dáma");
    msgBox.setText("Tento program byl vytvořen jako závěrečná práce do předmětu <b>KMI/YPS2 Projektový seminář 2<b>");
    msgBox.setInformativeText("<i>Autor programu:</i> <a href='https://www.jan-lostak.eu/'>Jan Lošťák</a>");
    msgBox.exec();
}

void MainWindow::on_actionUkoncit_triggered()
{
    if (!sGameManager->HasGameChanged())
    {
        QCoreApplication::quit();
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setText("<b>Máte rozehranou hru!</b>");
    msgBox.setInformativeText("Přejete si uložit vaši hru?");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    msgBox.setButtonText(QMessageBox::Save, tr("Uložit"));
    msgBox.setButtonText(QMessageBox::Discard, tr("Neukládat"));
    msgBox.setButtonText(QMessageBox::Cancel, tr("Storno"));

    int ret = msgBox.exec();

    switch (ret)
    {
        case QMessageBox::Save:
            OpenSaveGameToFile();
            QCoreApplication::quit();
            break;
        case QMessageBox::Discard:
            QCoreApplication::quit();
            break;
        case QMessageBox::Cancel:
            break;
        default:
            break;
    }
}

void MainWindow::on_actionNovaHra_triggered()
{
    if (!sGameManager->HasGameChanged())
    {
        sGameManager->GetGame()->Reset();
        sGameManager->SetColorOnTurn(PIECE_COLOR_WHITE);
        sGameManager->SetGameChanged(false);
        UpdateWhosTurn();
        UpdateTurnsHistoryView();
        _scene->ReDraw();
        _saveFile.clear();
        UpdateContextMenuControls();
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setText("<b>Máte rozehranou hru!</b>");
    msgBox.setInformativeText("Přejete si uložit vaši hru?");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    msgBox.setButtonText(QMessageBox::Save, tr("Uložit"));
    msgBox.setButtonText(QMessageBox::Discard, tr("Neukládat"));
    msgBox.setButtonText(QMessageBox::Cancel, tr("Storno"));

    int ret = msgBox.exec();

    switch (ret)
    {
        case QMessageBox::Save:
            OpenSaveGameToFile();
            break;
        case QMessageBox::Discard:
        {
            sGameManager->GetGame()->Reset();
            sGameManager->SetColorOnTurn(PIECE_COLOR_WHITE);
            sGameManager->SetGameChanged(false);
            UpdateWhosTurn();
            UpdateTurnsHistoryView();
            _scene->ReDraw();
            _saveFile.clear();
            break;
        }
        case QMessageBox::Cancel:
            break;
        default:
            break;
    }

    UpdateContextMenuControls();
}

void MainWindow::on_actionOtevritHru_triggered()
{
    if (!sGameManager->HasGameChanged())
    {
        OpenLoadGameFromFile();
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setText("<b>Máte rozehranou hru!</b>");
    msgBox.setInformativeText("Přejete si uložit vaši hru?");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    msgBox.setButtonText(QMessageBox::Save, tr("Uložit"));
    msgBox.setButtonText(QMessageBox::Discard, tr("Neukládat"));
    msgBox.setButtonText(QMessageBox::Cancel, tr("Storno"));

    int ret = msgBox.exec();

    switch (ret)
    {
        case QMessageBox::Save:
            OpenSaveGameToFile();
            OpenLoadGameFromFile();
            break;
        case QMessageBox::Discard:
            OpenLoadGameFromFile();
            break;
        case QMessageBox::Cancel:
            break;
        default:
            break;
    }
}

void MainWindow::on_actionUlozit_triggered()
{
    if (_saveFile.isEmpty())
        OpenSaveGameToFile();
    else
    {
        SaveToFile(_saveFile);
        sGameManager->SetGameChanged(false);
    }
}

void MainWindow::on_actionUlozitHruJako_triggered()
{
    OpenSaveGameToFile();
}

void MainWindow::on_actionNastaven_triggered()
{
    SettingsDialog* settings = new SettingsDialog();

    if (settings->exec() == QDialog::Accepted)
    {
        UpdateGameSettingStatuBar();
        UpdateContextMenuControls();
        _scene->ReDraw();
    }
}

void MainWindow::on_actionZobrazitNapovedu_triggered()
{
    QFile file(":/help.html");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    QTextEdit* help = new QTextEdit(this);
    help->setWindowFlag(Qt::Dialog);
    help->setReadOnly(true);
    help->append(in.readAll());
    help->setWindowTitle("Nápověda");
    help->setMinimumSize(480, 520);
    help->setFrameStyle(QFrame::NoFrame);
    help->moveCursor(QTextCursor::Start);
    help->ensureCursorVisible();
    help->show();
}

void MainWindow::on_actionNapovedaNejlepsihoTahu_triggered()
{
    if (sGameManager->GetBlackPlayer()->IsComputer() && sGameManager->GetWhitePlayer()->IsComputer())
        return;

    _scene->ShowTurnHint();
}

void MainWindow::on_actionZnovu_triggered()
{
    if (sGameManager->GetBoard()->ReDo())
    {
        UpdateTurnsHistoryView();
        UpdateWhosTurn();
        _scene->ReDraw();

        UpdateContextMenuControls();
    }
}

void MainWindow::on_actionVratitTah_triggered()
{
    if (sGameManager->GetBoard()->Undo())
    {
        UpdateTurnsHistoryView();
        UpdateWhosTurn();
        _scene->ReDraw();
        sGameManager->GetGame()->SetGameState(GAME_STATE_PLAY);

        UpdateContextMenuControls();
    }
}
