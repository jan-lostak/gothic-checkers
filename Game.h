#ifndef GAME_H
#define GAME_H

#include <vector>

class Player;
class GameBoard;
class GameAI;
class ChessPiece;

struct Position
{
    int row;
    int col;
    int capturesRow;
    int capturesCol;

    ChessPiece* capturePiece;

    Position(int row, int col) : row(row), col(col), capturesRow(0), capturesCol(0), capturePiece(nullptr) {}
    Position() : row(0), col(0), capturesRow(0), capturesCol(0), capturePiece(nullptr)  {}

    bool CompareTo(Position pos)
    {
        return pos.col == col && pos.row == row;
    }

    void Relocate(int x, int y)
    {
        row = x;
        col = y;
    }
};

enum GameDifficulty
{
    DIFFICULTY_BEGINNER = 0,
    DIFFICULTY_EASY = 1,
    DIFFICULTY_MEDIUM = 2,
    DIFFICULTY_HARD = 3
};

enum GameType
{
    GAME_GOTHIC_CHECKERS = 0,
    GAME_OSETIC_CHECKERS = 1, // Not implemented
    GAME_CHESS = 2 // Not implemented
};

enum GameState
{
    GAME_STATE_PLAY = 0, // Hraje v průběhu
    GAME_STATE_DRAW = 1, // Hra zkončila remízou
    GAME_STATE_WHITE_WIN = 2, // Bílý vyhrál partii
    GAME_STATE_BLACK_WIN = 3 // Černý vyhrál partii
};

typedef std::vector<Position> Turn;
typedef std::vector<Turn> Turns;

class Game
{
	public:
		virtual bool IsTurnAllowed(Turns& /*legalTurns*/, Turn& /*turn*/) = 0;

		virtual void Reset() = 0;

		virtual void ValidateTurn(Player* /*player*/, Turn& /*turn*/) = 0;
        virtual Turns GetAllValidTurns(Player* /*player*/, GameBoard* /*board*/, bool ifMandatoryOnlyMandatory = false) = 0;
        virtual void GetValidTurnsForPiece(ChessPiece* /*piece*/, Player* /*player*/, GameBoard* /*board*/, Turns& /*turns*/) = 0;
        virtual Turn GetBestTurn(Player* /*player*/, GameBoard* /*board*/) = 0;
        virtual Turns GetMandatoryTurns(Turns& /*turns*/) = 0;
		virtual GameState GetGameState() = 0;
        virtual void SetGameState(GameState /*state*/) = 0;
		virtual bool CompareTurns(Turn& /*turn*/, Turn& /*turn*/) = 0;

		virtual void OnTurnDone(Player* /*player*/, Turn* /*turn*/) = 0;

	protected:
		GameDifficulty _gameDifficulty;
		GameState _gameState;
		GameAI* _gameAI;
};

#endif // GAME_H
