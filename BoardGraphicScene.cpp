#include "BoardGraphicScene.h"
#include "MainWindow.h"

BoardGraphicScene::BoardGraphicScene(QObject *parent) : QGraphicsScene(parent)
{
    _parent = parent;
    _moveCalcTaskRunning = false;
    _gameState = GAME_STATE_PLAY;

    // Pozadí grafické scény
    QPixmap background(GAME_BOARD_BACKGROUND);
    addPixmap(background.scaled(GAME_BOARD_DIMENSIONS, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // Časovač
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Update()));
    timer->start(GAME_UPDATE_INTERVAL);
}

void BoardGraphicScene::Update()
{
    GameState curState = sGameManager->GetGame()->GetGameState();

    if (sGameManager->GetPlayerOnTurn()->GetPlayerType() == PLAYER_TYPE_COMPUTER && curState == GAME_STATE_PLAY)
    {
        MainWindow* window = reinterpret_cast<MainWindow*>(_parent);

        const char* players[2] = {"Černý", "Bílý"};
        std::stringstream s;
        s << "<span style='color: blue;'>" << players[sGameManager->GetPlayerOnTurn()->GetPlayerColor()] << " hráč přemýšlí o tahu...</span>&nbsp;";
        window->SetGameFeedbackText(s.str().c_str());

        if (_asyncTurnCalculator.isFinished() && _moveCalcTaskRunning)
        {
            _moveCalcTaskRunning = false;

            Turn turn = _asyncTurnCalculator.result();

            if (!turn.empty())
            {
                OnPieceTurn(turn);
                ReDraw();
            }
        }
        else if (!_asyncTurnCalculator.isRunning())
        {
            _asyncTurnCalculator = QtConcurrent::run(sGameManager->GetGame(), &Game::GetBestTurn, sGameManager->GetPlayerOnTurn(), sGameManager->GetBoard());
            _moveCalcTaskRunning = true;
        }

        window->UpdateContextMenuControls();
    }

    if (_gameState != curState)
    {
        OnGameStateChanged(_gameState, curState);
        _gameState = curState;
    }
}

void BoardGraphicScene::ReDraw()
{    
    _moveCalcTaskRunning = false;
    _asyncTurnCalculator.cancel();

    ClearHintRectangles();
    ClearChestPieces();

    for (ChessPiece* piece : sGameManager->GetBoard()->GetAllChessPieces())
        AddChessPiece(piece);
}

QGraphicsPixmapItem* BoardGraphicScene::AddChessPiece(ChessPiece* piece)
{
    QPixmap chessPieces(GAME_BOARD_CHESS_PIECES);

    PieceColor color = piece->GetPieceColor();
    PieceType type = piece->GetPieceType();

    QGraphicsPixmapItem* item = addPixmap(chessPieces.copy(type * PIECE_DIMENSIONS, color * PIECE_DIMENSIONS, PIECE_DIMENSIONS, PIECE_DIMENSIONS).scaled(GRID_CELL_SIZE, GRID_CELL_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    item->setPos(GetCoordsByCell(piece->GetPiecePosition()));
    item->setZValue(PIECE_STATE_IDLE);

    Player* player = sGameManager->GetPlayerOnTurn();

    // Pohyblivé figurky jsou pouze hráče na tahu
    if (player->GetPlayerColor() == color && player->IsHuman())
    {
        item->setFlag(QGraphicsItem::ItemIsMovable, true);
        item->setCursor(Qt::PointingHandCursor);
    }

    _pieces.push_back(item);
    return item;
}

QGraphicsRectItem* BoardGraphicScene::AddHintRectangle(Position pos)
{
    QRect rect(GRID_BEGIN_OFFSET + (pos.col * GRID_CELL_SIZE), GRID_BEGIN_OFFSET + (pos.row * GRID_CELL_SIZE), GRID_CELL_SIZE, GRID_CELL_SIZE);
    QPen pen;

    pen.setWidth(HINT_RECT_WIDTH);
    pen.setColor(Qt::green);

    QGraphicsRectItem* item = addRect(rect, pen, QBrush());

    _hintRects.push_back(item);

    return item;
}

void BoardGraphicScene::ClearChestPieces()
{
    for (QGraphicsPixmapItem* item : _pieces)
    {
        removeItem(item);
        delete item;
    }

    _pieces.clear();
}

void BoardGraphicScene::ClearHintRectangles()
{
    for (QGraphicsRectItem* item : _hintRects)
    {
        removeItem(item);
        delete item;
    }

    _hintRects.clear();
}

void BoardGraphicScene::OnGameStateChanged(GameState oldState, GameState newState)
{
    MainWindow* window = reinterpret_cast<MainWindow*>(_parent);

    if (oldState == GAME_STATE_PLAY && newState == GAME_STATE_BLACK_WIN)
    {
        window->SetGameFeedbackText("<b>Černý hráč vyhrál partii!</b>&nbsp;");
    }
    else if (oldState == GAME_STATE_PLAY && newState == GAME_STATE_WHITE_WIN)
    {
        window->SetGameFeedbackText("<b>Bílý hráč vyhrál partii!</b>&nbsp;");
    }
    else if (oldState == GAME_STATE_PLAY && newState == GAME_STATE_DRAW)
    {
        window->SetGameFeedbackText("<b>Partie skončila remízou!</b>&nbsp;");
    }
}

bool BoardGraphicScene::OnPieceTurn(Turn turn)
{
    MainWindow* window = reinterpret_cast<MainWindow*>(_parent);

    try
    {
        sGameManager->DoTurn(turn);
    }
    catch (IllegalTurnException e)
    {
        window->SetGameFeedbackText("<span style='color:red;'>Tah odporuje pravidlům hry!</span>&nbsp;");
        return false;
    }
    catch (MandatoryTurnRequiredException e)
    {
        window->SetGameFeedbackText("<span style='color:red;'>Musíte provést povinný tah!</span>&nbsp;");
        return false;
    }

    if (sGameManager->GetPlayerOnTurn()->IsHuman())
        window->SetGameFeedbackText("<span style='color: blue;'>Proveďte tah</span>&nbsp;");

    window->UpdateTurnsHistoryView();
    window->UpdateWhosTurn();
    window->UpdateContextMenuControls();

    return true;
}

bool BoardGraphicScene::GetCellByCoords(QPointF coords, Position& pos)
{
    for (Position slot : sGameManager->GetBoard()->GetSlots())
    {
        int xmin = GRID_BEGIN_OFFSET + (slot.col * GRID_CELL_SIZE);
        int ymin = GRID_BEGIN_OFFSET + (slot.row * GRID_CELL_SIZE);

        int xmax = GRID_BEGIN_OFFSET + (slot.col * GRID_CELL_SIZE) + GRID_CELL_SIZE;
        int ymax = GRID_BEGIN_OFFSET + (slot.row * GRID_CELL_SIZE) + GRID_CELL_SIZE;

        if (coords.x() > xmin && coords.x() < xmax && coords.y() > ymin && coords.y() < ymax)
        {
            pos.Relocate(slot.row, slot.col);
            return true;
        }
    }

    return false;
}

QPointF BoardGraphicScene::GetCoordsByCell(Position& pos)
{
    return QPointF(GRID_BEGIN_OFFSET + (pos.col * GRID_CELL_SIZE), GRID_BEGIN_OFFSET + (pos.row * GRID_CELL_SIZE));
}

void BoardGraphicScene::ShowTurnHint()
{
    if (sGameManager->GetGame()->GetGameState() != GAME_STATE_PLAY)
        return;

    GameBoard* board = sGameManager->GetBoard()->Clone();
    Turn turn = sGameManager->GetGame()->GetBestTurn(sGameManager->GetPlayerOnTurn(), board);
    delete board;

    ClearHintRectangles();

    AddHintRectangle(turn.front());
    AddHintRectangle(turn.back());
}

void BoardGraphicScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

    if (!item)
        return;

    Position pos;

    if (item->zValue() != PIECE_STATE_IDLE || !GetCellByCoords(event->scenePos(), pos))
        return;

    ChessPiece* piece = sGameManager->GetBoard()->GetPieceAt(pos.row, pos.col);

    if (!piece)
        return;

    Player* player = sGameManager->GetPlayerOnTurn();

    if (player->IsComputer())
        return;

    if (piece->GetPieceColor() != player->GetPlayerColor())
        return;

    _draggedFrom = pos;
    item->setZValue(PIECE_STATE_DRAGGED);

    Turns turns;
    sGameManager->GetGame()->GetValidTurnsForPiece(piece, sGameManager->GetPlayerOnTurn(), sGameManager->GetBoard(), turns);

    QGraphicsScene::mousePressEvent(event); // Signál se musí předat před manipulací jinak dojde k nekonzistentnímu stavu

    ClearHintRectangles();

    for (Turn turn : turns)
    {
        Position& pos = turn.back();

        if (sGameManager->GetPlayerOnTurn()->GetPlayerColor() == PIECE_COLOR_BLACK)
            pos.row = (sGameManager->GetBoard()->GetBoardNumRows() - 1) - pos.row;

        AddHintRectangle(pos);
    }
}

void BoardGraphicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());

    if (!item)
        return;

    if (item->zValue() != PIECE_STATE_DRAGGED)
        return;

    Position pos;

    if (GetCellByCoords(event->scenePos(), pos))
        if (!pos.CompareTo(_draggedFrom)) // Pokud pozice není výchozí pozice, tak zahrajeme tah
            OnPieceTurn({ _draggedFrom, pos });

    QGraphicsScene::mouseReleaseEvent(event); // Signál se musí předat před manipulací jinak dojde k nekonzistentnímu stavu

    ReDraw();
}
