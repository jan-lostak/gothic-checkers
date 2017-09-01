#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include "Game.h"

enum PieceType
{
    PIECE_TYPE_PAWN = 5,
    PIECE_TYPE_KNIGHT = 3,
    PIECE_TYPE_BISHOP = 4,
    PIECE_TYPE_ROOK = 2,
    PIECE_TYPE_QUEEN = 0,
    PIECE_TYPE_KING = 1
};

enum PieceColor
{
    PIECE_COLOR_WHITE = 1,
    PIECE_COLOR_BLACK = 0
};

class ChessPiece
{
    public:
        ChessPiece();
        ~ChessPiece();

        void SetPieceColor(PieceColor color) { _color = color;  }
        PieceColor GetPieceColor() { return _color; }

        void SetPieceType(PieceType type) { _type = type; }
        PieceType GetPieceType() { return _type; }

        void SetPiecePosition(Position position) { _position = position; }
        Position GetPiecePosition() { return _position; }

        void SetCaptured(bool captured) { _captured = captured; }
        bool IsCaptured() { return _captured; }

    private:
        PieceColor _color;
        PieceType _type;
        Position _position;
        bool _captured;
};

#endif // CHESSPIECE_H
