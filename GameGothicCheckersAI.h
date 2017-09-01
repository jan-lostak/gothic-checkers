#ifndef MINIMAX_H
#define MINIMAX_H

#include <random>

#include "Game.h"
#include "GameAI.h"
#include "Player.h"
#include "GameManager.h"

#define PAWN_SCORE 25
#define QUEEN_SCORE 100
#define MAX_VALUE 99999

class GameGothicCheckersAI : public GameAI
{
	public:
		Turn ComputeBestTurn(Player* /*player*/, GameBoard* /*board*/, Game* /*game*/) override;
		int Evaluate(Player* /*player*/, GameBoard* /*board*/) override;

	private:
        int MiniMax(bool /*maximizing*/, Player* /*player*/, Game* /*game*/, GameBoard* /*board*/, int /*depth*/);

		const int _depthByDifficulty[4] = { 2, 3, 4, 5 };

		const int _whitePawnBonus[8][8] =
		{
			{ 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 },
		};

		const int _whiteKingBonus[8][8] =
		{
			{ 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 2 , 0 , 0 , 0 , 0 , 0 , 0 , 2 },
			{ 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 },
		};

		const int _blackPawnBonus[8][8] =
		{
			{ -3 , -3 , -3 , -3 , -3 , -3 , -3 , -3 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
		};

		const int _blackKingBonus[8][8] =
		{
			{ -3 , -3 , -3 , -3 , -3 , -3 , -3 , -3 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , -2 },
			{ -2 , -2 , -2 , -2 , -2 , -2 , -2 , -2 },
		};
};

#endif // MINIMAX_H
