#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <vector>
#include <stack>
#include <string>
#include <sstream>
#include <unordered_map>

#include <QDebug>

#include "ChessPiece.h"
#include "Game.h"

#define MAX_BOARD_SIZE 16
#define ASCII_LETTER_A 97

typedef std::tuple<std::string, int, std::vector<ChessPiece>, std::vector<ChessPiece>, PieceColor> HistoryItem;
typedef std::deque<HistoryItem> BoardHistory;

class GameBoard
{
	public:
		GameBoard(int /*rows*/, int /*cols*/);
		~GameBoard();

        GameBoard* Clone();

		ChessPiece* GetPieceAt(int /*row*/, int /*col*/);
		void SetPieceAt(ChessPiece* /*piece*/, int /*row*/, int /*col*/);
        void AddPiece(PieceType /*type*/, PieceColor /*color*/, int /*row*/, int /*col*/);

        int GetMovesCountSinceLastCapture() { return _movesCounter; }
        int GetBoardNumRows() { return _bRows; }
        int GetBoardNumCols() { return _bCols; }

        BoardHistory& GetBoardHistory() { return _history; }
        BoardHistory& GetBoardRedoHistory() { return _historyRedo; }
        void AddBoardHistoryItem(HistoryItem item) { _history.push_back(item); }
        void AddBoardHistoryRedoItem(HistoryItem item) { _historyRedo.push_back(item); }

		bool IsInBoard(int /*row*/, int /*col*/);
		bool IsFieldEmpty(int /*row*/, int /*col*/);

        bool IsInversed() { return _inversed; }
		void Inverse();
        void Clear(bool historyIncl = true);
		void DoTurn(Turn /*turn*/, bool count = true);
		void UndoTurn(Turn /*turn*/);
        void SetPieces(std::vector<ChessPiece>& /*pieces*/);

        bool Undo();
        bool ReDo();

		int GetPiecesCountByColor(PieceColor /*color*/);
		int GetPiecesCountByColorAndType(PieceColor /*color*/, PieceType /*type*/);
		int GetPiecesCountByColorAtRow(int /*row*/, PieceColor /*color*/);
		int GetNeightboursFriendlyCount(ChessPiece* /*piece*/);
		std::vector<ChessPiece*> GetAllChessPiecesByColor(PieceColor /*color*/);
        std::vector<ChessPiece*> GetAllChessPieces();
        std::vector<Position> GetSlots();
        std::string ToString();
        std::string StringifyTurn(Turn& turn);

	private:
		ChessPiece* _slots[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
		const int _offset[8][2] = { { 0, -1 },{ -1, -1 },{ -1, 0 },{ -1, 1 },{ 0, 1 },{ 1, -1 },{ 1 , 0 },{ 1 , 1 } };
		int _bRows;
		int _bCols;
		int _movesCounter;
		bool _inversed;
		std::vector<ChessPiece*> _pieceHolder;
        BoardHistory _history;
        BoardHistory _historyRedo;
};

#endif // GAMEBOARD_H
