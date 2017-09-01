#include "GameGothicCheckers.h"

GameGothicCheckers::GameGothicCheckers()
{
	_gameAI = new GameGothicCheckersAI();
	_gameDifficulty = DIFFICULTY_EASY;
	Reset();
}

GameGothicCheckers::~GameGothicCheckers() 
{ 
	delete _gameAI;
}

void GameGothicCheckers::Reset()
{
    GameBoard* board = sGameManager->GetBoard();

	// Pøed resetem musíme vyèistit desku a smazat alokované figurky
	board->Clear();

	for (const int* slot : _pieceDefaults)
		board->AddPiece(PieceType(slot[3]), PieceColor(slot[2]), slot[0], slot[1]);

	_gameState = GAME_STATE_PLAY;
}

bool GameGothicCheckers::IsTurnAllowed(Turns& legalTurns, Turn& turn)
{
	for (Turn legalTurn : legalTurns)
		if (CompareTurns(legalTurn, turn))
			return true;

	return false;
}

void GameGothicCheckers::ValidateTurn(Player* player, Turn& turn)
{
    GameBoard* board = sGameManager->GetBoard();

    Turns legalTurns = GetAllValidTurns(player, board);

	// Kontrola platnosti tahu
	if (!IsTurnAllowed(legalTurns, turn))
		throw IllegalTurnException();

	// Kontrola zda je povinný tah
    Turns mandatoryTurns = GetMandatoryTurns(legalTurns);

	if (mandatoryTurns.size() > 0)
	{
		bool found = false;

		for (Turn mandatoryTurn : mandatoryTurns)
        {
            if (CompareTurns(turn, mandatoryTurn))
            {
                found = true;
                break;
            }
        }

		if (!found)
			throw MandatoryTurnRequiredException();
	}

	// Nahradit tah zadaný hráèem za tah z generátoru, protože tah z generátoru obsahuje informace o zajatých figurkách
	for (Turn legalTurn : legalTurns)
	{
        if (!CompareTurns(turn, legalTurn))
			continue;

		turn = legalTurn;
		break;
	}
}

Turns GameGothicCheckers::GetMandatoryTurns(Turns& turns)
{
	Turns mTurns;

	for (Turn turn : turns)
	{
		for (Position pos : turn)
		{
            if (pos.capturePiece != nullptr)
			{
				mTurns.push_back(turn);
				break;
			}
		}
	}

	return mTurns;
}

Turns GameGothicCheckers::GetAllValidTurns(Player* player, GameBoard* board, bool ifMandatoryOnlyMandatory)
{
	Turns turns;
	std::vector<ChessPiece*> pieces = board->GetAllChessPiecesByColor(player->GetPlayerColor());
	for (ChessPiece* piece : pieces)
        GetValidTurnsForPiece(piece, player, board, turns);

	// Invertovat tahy pokud byl na øadì èerný, protože byla pøevrácená deska
	if (player->GetPlayerColor() == PIECE_COLOR_BLACK)
	{
		int rows = board->GetBoardNumRows();

		for (Turn& legalTurn : turns)
        {
            for (Position& turn : legalTurn)
            {
                turn.row = rows - turn.row - 1;

                if (turn.capturePiece != nullptr)
                    turn.capturesRow = rows - turn.capturesRow - 1;
            }
        }
	}

	if (ifMandatoryOnlyMandatory)
	{
		Turns mandatoryTurns = GetMandatoryTurns(turns);

		if (!mandatoryTurns.empty())
			return mandatoryTurns;
	}

	return turns;
}

void GameGothicCheckers::GetValidTurnsForPiece(ChessPiece* piece, Player* player, GameBoard* board, Turns& turns)
{
	// Pokud je hráè na druhé stranì desky (èerný), tak musíme desku obrátit, aby algoritmus správnì fungoval
	if (player->GetPlayerColor() == PIECE_COLOR_BLACK)
		board->Inverse();

	// Rekurzivnì zjístit pøeskokové tahy popøipadì vícenásobné tahy
    Turns captureTurns = CheckForCaptureTurns(board, player, piece);

	// Rekurzivnì zjístit normální tahy pokud nebyly nalezeny pøeskokové tahy
	if (captureTurns.empty())
		CheckForMove(board, piece, &turns);
	else
		turns.insert(turns.end(), captureTurns.begin(), captureTurns.end());

    // Vrátíme desku do pùvodního stavu a invertujeme tahy
	if (player->GetPlayerColor() == PIECE_COLOR_BLACK)
		board->Inverse();
}

Turns GameGothicCheckers::CheckForCaptureTurns(GameBoard* board, Player* player, ChessPiece* piece)
{
    TreeNode* node = new TreeNode();
    node->SetPosition(piece->GetPiecePosition());
    CheckForCaptureTurnsInner(board, player, piece, node);
    Turns turns = node->GenerateTurns();
    delete node;
    return turns;
}

void GameGothicCheckers::CheckForCaptureTurnsInner(GameBoard* board, Player* player, ChessPiece* piece, TreeNode* node)
{
    Position position = node->GetPosition();

    if (piece->GetPieceType() == PIECE_TYPE_PAWN)
    {
        for (const int* pattern : _pawnCapturePattern)
        {
            int rowOffset = position.row + pattern[0];
            int colOffset = position.col + pattern[1];

            if (!board->IsInBoard(rowOffset, colOffset))
                continue;

            if (board->IsFieldEmpty(rowOffset, colOffset))
                continue;

            ChessPiece* foundPiece = board->GetPieceAt(rowOffset, colOffset);

            if (foundPiece->GetPieceColor() == player->GetPlayerColor())
                continue;

            if (foundPiece->IsCaptured())
                continue;

            int rowCaptureOffset = position.row + pattern[0] * 2;
            int colCaptureOffset = position.col + pattern[1] * 2;

            if (!board->IsInBoard(rowCaptureOffset, colCaptureOffset))
                continue;

            if (!board->IsFieldEmpty(rowCaptureOffset, colCaptureOffset))
                continue;

            Position captureTurn(rowCaptureOffset, colCaptureOffset);

            captureTurn.capturesRow = foundPiece->GetPiecePosition().row;
            captureTurn.capturesCol = foundPiece->GetPiecePosition().col;
            captureTurn.capturePiece = foundPiece;

            TreeNode* newNode = new TreeNode();
            newNode->SetPosition(captureTurn);

            node->AddChild(newNode);
        }
    }
    else if (piece->GetPieceType() == PIECE_TYPE_QUEEN)
    {
        int maxDimension = std::max(board->GetBoardNumRows(), board->GetBoardNumCols());

        for (const int* pattern : _kingCapturePattern)
        {
            ChessPiece* capturesPiece = nullptr;

            for (int i = 1; i < maxDimension; i++)
            {
                int rowOffset = position.row + pattern[0] * i;
                int colOffset = position.col + pattern[1] * i;

                int rowCaptureOffset = position.row + pattern[0] * (i + 1);
                int colCaptureOffset = position.col + pattern[1] * (i + 1);

                if (capturesPiece == nullptr)
                {
                    if (!board->IsInBoard(rowOffset, colOffset))
                        continue;

                    if (board->IsFieldEmpty(rowOffset, colOffset))
                        continue;

                    ChessPiece* foundPiece = board->GetPieceAt(rowOffset, colOffset);

                    if (foundPiece->GetPieceColor() == player->GetPlayerColor())
                        break;

                    if (foundPiece->IsCaptured())
                        continue;

                    if (!board->IsInBoard(rowCaptureOffset, colCaptureOffset))
                        break;

                    capturesPiece = foundPiece;
                }
                else
                {
                    if (!board->IsInBoard(rowOffset, colOffset))
                        break;

                    if (!board->IsFieldEmpty(rowOffset, colOffset))
                        break;

                    Position captureTurn(rowOffset, colOffset);

                    captureTurn.capturesRow = capturesPiece->GetPiecePosition().row;
                    captureTurn.capturesCol = capturesPiece->GetPiecePosition().col;
                    captureTurn.capturePiece = capturesPiece;

                    TreeNode* newNode = new TreeNode();
                    newNode->SetPosition(captureTurn);

                    node->AddChild(newNode);
                }
            }

            capturesPiece = nullptr;
        }
    }

    for (TreeNode* childNode : node->GetChilds())
    {
        ChessPiece* cPiece = childNode->GetPosition().capturePiece;

        if (cPiece != nullptr)
            cPiece->SetCaptured(true);

        CheckForCaptureTurnsInner(board, player, piece, childNode);

        if (cPiece != nullptr)
            cPiece->SetCaptured(false);
    }
}

void GameGothicCheckers::CheckForMove(GameBoard* board, ChessPiece* piece, Turns* turns)
{
	Position pos = piece->GetPiecePosition();

	if (piece->GetPieceType() == PIECE_TYPE_PAWN)
	{
		for (const int* pattern : _pawnCapturePattern)
			CheckForMoveInner(board, pos, turns, pattern);
	}
    else if (piece->GetPieceType() == PIECE_TYPE_QUEEN)
	{
		int maxDimension = std::max(board->GetBoardNumRows(), board->GetBoardNumCols());

        for (const int* pattern : _kingCapturePattern)
        {
            for (int i = 1; i < maxDimension; i++)
            {
                if (!CheckForMoveInner(board, pos, turns, pattern, i))
                    break;
            }
        }
	}
}

bool GameGothicCheckers::CheckForMoveInner(GameBoard* board, Position pos, Turns* turns, const int* pattern, int patternShift)
{
	int rowOffset = pos.row + pattern[0] * patternShift;
	int colOffset = pos.col + pattern[1] * patternShift;

	if (!board->IsInBoard(rowOffset, colOffset))
        return true;

	if (!board->IsFieldEmpty(rowOffset, colOffset))
		return false;

	Turn turn;
	turn.push_back(pos);
	turn.push_back(Position(rowOffset, colOffset));
	turns->push_back(turn);

	return true;
}

void GameGothicCheckers::OnTurnDone(Player* player, Turn* turn)
{
    GameBoard* board = sGameManager->GetBoard();
	int whitePieces = board->GetPiecesCountByColor(PIECE_COLOR_WHITE);
	int blackPieces = board->GetPiecesCountByColor(PIECE_COLOR_BLACK);

	// Povýšení na dámu
    if (!turn->empty())
	{
		Position finalPos = turn->back();

		if (player->GetPlayerColor() == PIECE_COLOR_WHITE)
		{
			if (finalPos.row == 0) // Horní strana desky
                board->GetPieceAt(finalPos.row, finalPos.col)->SetPieceType(PIECE_TYPE_QUEEN);
		}
		else
		{
			if (finalPos.row == (board->GetBoardNumRows() - 1)) // Dolní strana desky
                board->GetPieceAt(finalPos.row, finalPos.col)->SetPieceType(PIECE_TYPE_QUEEN);
		}
	}

	// Aktualizovat stav hry
	if (board->GetMovesCountSinceLastCapture() == MAX_MOVES_WITH_NO_CAPTURE)
	{
		if (whitePieces == blackPieces)
			_gameState = GAME_STATE_DRAW;
		else if (whitePieces < blackPieces)
			_gameState = GAME_STATE_BLACK_WIN;
		else
			_gameState = GAME_STATE_WHITE_WIN;
	}
	else 
	{
		if (whitePieces == 0)
			_gameState = GAME_STATE_BLACK_WIN;
		else if (blackPieces == 0)
			_gameState = GAME_STATE_WHITE_WIN;
	}
}

Turn GameGothicCheckers::GetBestTurn(Player* player, GameBoard* board)
{
    GameBoard* clone = board->Clone();
    Turn turn = _gameAI->ComputeBestTurn(player, clone, this);
    delete clone;
    return turn;
}

bool GameGothicCheckers::CompareTurns(Turn& l, Turn& r)
{
    if (l.front().CompareTo(r.front()) && l.back().CompareTo(r.back())) // Tranzitivní kontrola
        return true;

	if (l.size() != r.size())
		return false;

	int matches = 0;

	for (int i = 0; i < l.size(); i++)
		if (l.at(i).col == r.at(i).col && l.at(i).row == r.at(i).row)
			matches++;

	if (matches == l.size())
		return true;

	return false;
}
