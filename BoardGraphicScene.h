#ifndef BOARDGRAPHICSCENE_H
#define BOARDGRAPHICSCENE_H

#include <QtCore>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QCursor>
#include <QDebug>
#include <QTimer>
#include <QFuture>
#include <QtConcurrent>
#include <QGraphicsSceneMouseEvent>

#include <Game.h>
#include <ChessPiece.h>
#include <GameManager.h>

#define GAME_BOARD_DIMENSIONS 541, 541

#define GRID_BEGIN_OFFSET 34
#define GRID_CELL_SIZE 59

#define PIECE_DIMENSIONS 64
#define HINT_RECT_WIDTH 2

#define GAME_BOARD_BACKGROUND ":/images/board.png"
#define GAME_BOARD_CHESS_PIECES ":/images/pieces.png"

#define GAME_UPDATE_INTERVAL 500

enum PieceDragState
{
    PIECE_STATE_IDLE = 1, // Figurka je umístěna na plátně
    PIECE_STATE_DRAGGED = 2 // Figurka je přetahována kurzorem
};

class BoardGraphicScene : public QGraphicsScene
{
    Q_OBJECT

    public:
        explicit BoardGraphicScene(QObject* parent = 0);
        QGraphicsPixmapItem* AddChessPiece(ChessPiece* piece);
        QGraphicsRectItem* AddHintRectangle(Position pos);
        void ClearChestPieces();
        void ClearHintRectangles();
        void ShowTurnHint();
        void ReDraw();

    private:
        bool GetCellByCoords(QPointF pos, Position& gpos);
        QPointF GetCoordsByCell(Position& pos);
        bool OnPieceTurn(Turn turn);
        void OnGameStateChanged(GameState oldState, GameState newState);

        std::vector<QGraphicsPixmapItem*> _pieces;
        std::vector<QGraphicsRectItem*> _hintRects;
        Position _draggedFrom;

        QFuture<Turn> _asyncTurnCalculator;
        QObject* _parent;

        bool _moveCalcTaskRunning;
        GameState _gameState;

    public slots:
        void Update();

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};

#endif // BOARDGRAPHICSCENE_H
