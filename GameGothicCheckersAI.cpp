#include <QTime>

#include "GameGothicCheckersAI.h"

Turn GameGothicCheckersAI::ComputeBestTurn(Player* player, GameBoard* board, Game* game)
{
    Turn bestTurn;

    int depth = _depthByDifficulty[player->GetGameDifficulty()];

    Turns turns = game->GetAllValidTurns(player, board, true);
    int bestRating = -MAX_VALUE;

    for (Turn& turn : turns)
    {
        board->DoTurn(turn, false);
        int rating = MiniMax(false, sGameManager->GetOponentOf(player), game, board, depth - 1);
        board->UndoTurn(turn);

        if (rating > bestRating)
        {
            bestRating = rating;
            bestTurn = turn;
        }
    }

    return bestTurn;
}

int GameGothicCheckersAI::Evaluate(Player* player, GameBoard* board)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    int whiteScore = 0;
    int blackScore = 0;

    std::vector<ChessPiece*> wPieces = board->GetAllChessPiecesByColor(PIECE_COLOR_WHITE);
    std::vector<ChessPiece*> bPieces = board->GetAllChessPiecesByColor(PIECE_COLOR_BLACK);

    for (ChessPiece* piece : wPieces)
    {
        // Břičtení bonusu za umístění figurek
        Position pos = piece->GetPiecePosition();
        if (piece->GetPieceType() == PIECE_TYPE_PAWN)
        {
            whiteScore += _whitePawnBonus[pos.row][pos.col] + PAWN_SCORE;

            // Blíže k povýšení na dámu, více bodů a malá náhodná složka simulující chybu člověka
            int promotionPoints = 1 + 7 - pos.row;
            whiteScore += promotionPoints * (qrand() % promotionPoints);
        }
        else
            whiteScore += _whiteKingBonus[pos.row][pos.col] + QUEEN_SCORE;
    }

    for (ChessPiece* piece : bPieces)
    {
        // Břičtení bonusu za umístění figurek
        Position pos = piece->GetPiecePosition();
        if (piece->GetPieceType() == PIECE_TYPE_PAWN)
        {
            blackScore += _blackPawnBonus[pos.row][pos.col] + PAWN_SCORE;

            // Blíže k povýšení na dámu, více bodů a malá náhodná složka simulující chybu člověka
            int promotionPoints = 1 + pos.row;
            blackScore += promotionPoints * (qrand() % promotionPoints);
        }
        else
            blackScore += _blackKingBonus[pos.row][pos.col] + QUEEN_SCORE;
    }

    if (sGameManager->GetOponentOf(player)->GetPlayerColor() == PIECE_COLOR_WHITE)
        return blackScore - whiteScore;
    else
        return whiteScore - blackScore;

    return 0;
}

int GameGothicCheckersAI::MiniMax(bool maximizing, Player* player, Game* game, GameBoard* board, int depth)
{
    if (depth == 0)
        return Evaluate(player, board);

    Turns turns = game->GetAllValidTurns(player, board, true);
    if (turns.empty())
        return Evaluate(player, board);

    if (maximizing)
    {
        int bestValue = -MAX_VALUE;

        for (Turn& turn : turns)
        {
            board->DoTurn(turn, false);
            bestValue = std::max(bestValue, MiniMax(!maximizing, sGameManager->GetOponentOf(player), game, board, depth - 1));
            board->UndoTurn(turn);
        }

        return bestValue;
    }
    else
    {
        int bestValue = MAX_VALUE;

        for (Turn& turn : turns)
        {
            board->DoTurn(turn, false);
            bestValue = std::min(bestValue, MiniMax(!maximizing, sGameManager->GetOponentOf(player), game, board, depth - 1));
            board->UndoTurn(turn);
        }

        return bestValue;
    }
}
