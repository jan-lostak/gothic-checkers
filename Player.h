#ifndef PLAYER_H
#define PLAYER_H

#include "ChessPiece.h"
#include "Game.h"

enum PlayerType
{
    PLAYER_TYPE_HUMAN = 0, // Tah zadává na desce hráč
    PLAYER_TYPE_COMPUTER = 1 // Tah vypočítá AI
};

class Player
{
    public:
        Player();
        ~Player();

        void SetPlayerType(PlayerType playerType) { _playerType = playerType; }
        PlayerType GetPlayerType() { return _playerType; }

        void SetPlayerColor(PieceColor color) { _color = color; }
        PieceColor GetPlayerColor() { return _color; }

        void SetGameDifficulty(GameDifficulty difficulty) { _difficulty = difficulty; }
        GameDifficulty GetGameDifficulty() { return _difficulty; }

        bool IsWhite() { return _color == PIECE_COLOR_WHITE; }
        bool IsBlack() { return _color == PIECE_COLOR_BLACK; }
        bool IsHuman() { return _playerType == PLAYER_TYPE_HUMAN; }
        bool IsComputer() { return _playerType == PLAYER_TYPE_COMPUTER; }

    private:
        PlayerType _playerType;
        PieceColor _color;
        GameDifficulty _difficulty;
};

#endif // PLAYER_H
