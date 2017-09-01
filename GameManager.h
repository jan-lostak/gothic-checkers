#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "GameGothicCheckers.h"
#include "ChessPiece.h"
#include "GameBoard.h"

#define sGameManager GameManager::GetInstance() \

class GameManager
{
	public:
		static GameManager* GetInstance();

		GameManager();
		~GameManager();

		void SetGameType(GameType gameType) { _gameType = gameType; }
		GameType GetGameType() { return _gameType; }

		GameBoard* GetBoard() { return _board; }
		Game* GetGame() { return _game; }

        Player* GetPlayerOnTurn() { return _playerOnTurn; }
		Player* GetOponentOf(Player* /*player*/);
        Player* GetBlackPlayer() { return _blackPlayer; }
        Player* GetWhitePlayer() { return _whitePlayer; }

		void DoTurn(Turn turn);
		void SwapPlayers();
		void Initialize();
        void SetColorOnTurn(PieceColor /*color*/);

        void SetGameChanged(bool changed) { _gameChanged = changed; }
        bool HasGameChanged() { return _gameChanged; }

	private:
		Game* _game;
		GameBoard* _board;
		GameType _gameType;
		PieceColor _firstPlayerColor;
		Player* _blackPlayer;
		Player* _whitePlayer;
		Player* _playerOnTurn;
        bool _gameChanged;
};

#endif // GAMEMANAGER_H
