#ifndef GAMEGOTHICDAME_H
#define GAMEGOTHICDAME_H

#include <algorithm> 
#include <iostream>
#include <QDebug>

#include "Game.h"
#include "GameAI.h"
#include "GameExceptions.h"
#include "GameGothicCheckersAI.h"
#include "TreeNode.h"

#define MAX_MOVES_WITH_NO_CAPTURE 30
#define GOTHIC_CHECKERS_BOARD_SIZE 8, 8

class GameGothicCheckers : public Game
{
	public:
		GameGothicCheckers();
		~GameGothicCheckers();

		void ValidateTurn(Player* /*player*/, Turn& /*turn*/) override;
		bool IsTurnAllowed(Turns& /*legalTurns*/, Turn& /*turn*/) override;
        Turns GetMandatoryTurns(Turns& /*turns*/) override;

		void Reset() override;

        Turns GetAllValidTurns(Player* /*player*/, GameBoard* /*board*/, bool ifMandatoryOnlyMandatory = false) override;
        void GetValidTurnsForPiece(ChessPiece* /*piece*/, Player* /*player*/, GameBoard* /*board*/, Turns& /*turns*/) override;
        Turn GetBestTurn(Player* /*player*/, GameBoard* /*board*/) override;
		GameState GetGameState() override { return _gameState; }
        void SetGameState(GameState state) override { _gameState = state; }
		bool CompareTurns(Turn& /*turn*/, Turn& /*turn*/) override;

		void OnTurnDone(Player* /*player*/, Turn* /*turn*/) override;

	private:
        Turns CheckForCaptureTurns(GameBoard* /*board*/, Player* /*player*/, ChessPiece* /*piece*/);
        void CheckForCaptureTurnsInner(GameBoard* /*board*/, Player* /*player*/, ChessPiece* /*piece*/, TreeNode* /*node*/);

        void CheckForMove(GameBoard* /*board*/, ChessPiece* /*piece*/, Turns* /*turns*/);
		bool CheckForMoveInner(GameBoard* /*board*/, Position /*pos*/, Turns* /*turns*/, const int* /*pattern*/, int patternShift = 1);

		const int _pawnCapturePattern[5][2] = { { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 } };
		const int _kingCapturePattern[8][2] = { { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, -1 }, { 1 , 0 }, { 1 , 1 } };

        const int _pieceDefaults[32][4] =
		{
            { 0, 0, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 0, 1, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 0, 2, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 0, 3, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 0, 4, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 0, 5, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 0, 6, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 0, 7, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 1, 0, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 1, 1, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 1, 2, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 1, 3, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 1, 4, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
            { 1, 5, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
			{ 1, 6, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
            { 1, 7, PIECE_COLOR_BLACK, PIECE_TYPE_PAWN },
            { 6, 0, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 6, 1, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 6, 2, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 6, 3, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 6, 4, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 6, 5, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 6, 6, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 6, 7, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 7, 0, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 7, 1, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 7, 2, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 7, 3, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 7, 4, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 7, 5, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
			{ 7, 6, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN },
            { 7, 7, PIECE_COLOR_WHITE, PIECE_TYPE_PAWN }
        };
};

#endif // GAMEGOTHICDAME_H
