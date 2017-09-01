#ifndef GAMEAI_H
#define GAMEAI_H

#include "Game.h"
#include "GameBoard.h"

class GameAI
{
    public:
        virtual Turn ComputeBestTurn(Player* /*player*/, GameBoard* /*board*/, Game* /*game*/) = 0;
        virtual int Evaluate(Player* /*player*/, GameBoard* /*board*/) = 0;
};

#endif // GAMEAI_H
