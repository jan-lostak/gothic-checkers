#include "GameBoard.h"
#include "GameManager.h"

GameBoard::GameBoard(int rows, int cols)
{
    _bRows = rows;
    _bCols = cols;
    _inversed = false;
    _movesCounter = 0;

    for (int i = 0; i < _bRows; i++)
        for (int j = 0; j < _bCols; j++)
            _slots[i][j] = nullptr;
}

GameBoard::~GameBoard()
{
    Clear();
}

GameBoard* GameBoard::Clone()
{
    GameBoard* board = new GameBoard(_bRows, _bCols);

    for (int i = 0; i < _bRows; i++)
    {
        for (int j = 0; j < _bCols; j++)
        {
            ChessPiece* piece = _slots[i][j];

            if (piece == nullptr)
                continue;

            board->AddPiece(piece->GetPieceType(), piece->GetPieceColor(), i, j);
        }
    }

    return board;
}

void GameBoard::AddPiece(PieceType type, PieceColor color, int row, int col)
{
    ChessPiece* piece = new ChessPiece();
    piece->SetPieceColor(color);
    piece->SetPieceType(type);

    SetPieceAt(piece, row, col);
    _pieceHolder.push_back(piece);
}

ChessPiece* GameBoard::GetPieceAt(int row, int col)
{
    if (row > _bRows || col > _bCols)
        return nullptr;

    return _slots[row][col];
}

void GameBoard::SetPieceAt(ChessPiece* piece, int row, int col)
{
    if (row > _bRows || col > _bCols)
        return;

    if (piece != nullptr)
        piece->SetPiecePosition(Position(row, col));

    _slots[row][col] = piece;
}

bool GameBoard::IsInBoard(int row, int col)
{
    return !(row < 0 || col < 0 || row > _bRows - 1 || col > _bCols - 1);
}

bool GameBoard::IsFieldEmpty(int row, int col)
{
    return GetPieceAt(row, col) == nullptr;
}

void GameBoard::Inverse()
{
    for (int i = 0; i < (_bRows / 2); i++)
    {
        for (int j = 0; j < _bCols; j++)
        {
            int iInversed = _bRows - i - 1;
            ChessPiece* tmp = GetPieceAt(i, j);
            SetPieceAt(GetPieceAt(iInversed, j), i, j);
            SetPieceAt(tmp, iInversed, j);
        }
    }

    _inversed = !_inversed;
}

void GameBoard::Clear(bool historyIncl)
{
    for (int i = 0; i < _bRows; i++)
        for (int j = 0; j < _bCols; j++)
            _slots[i][j] = nullptr;

    for (ChessPiece* piece : _pieceHolder)
        delete piece;

    _pieceHolder.clear();

    if (historyIncl)
    {
        _historyRedo.clear();
        _history.clear();
    }
}

void GameBoard::DoTurn(Turn turn, bool count)
{
    std::vector<ChessPiece> piecesBefore;

    if (count)
        for (ChessPiece* piece : GetAllChessPieces())
            piecesBefore.push_back(*piece);

    Position start = turn.front();
    Position end = turn.back();

    ChessPiece* tmp = GetPieceAt(start.row, start.col);

    SetPieceAt(nullptr, start.row, start.col);
    SetPieceAt(tmp, end.row, end.col);

    if (count)
        _movesCounter++;

    for (Position pos : turn)
    {
        if (pos.capturePiece == nullptr)
            continue;

        if (count)
            _movesCounter = 0;

        ChessPiece* captured = GetPieceAt(pos.capturesRow, pos.capturesCol);
        Position captureePos = captured->GetPiecePosition();

        captured->SetCaptured(true);
        SetPieceAt(nullptr, captureePos.row, captureePos.col);
    }

    if (count) // Přidat do historie
    {
        std::vector<ChessPiece> piecesAfter;

        for (ChessPiece* piece : GetAllChessPieces())
            piecesAfter.push_back(*piece);

        _history.push_back({ StringifyTurn(turn), _movesCounter, piecesBefore, piecesAfter, tmp->GetPieceColor() });
        _historyRedo.clear();
    }
}

void GameBoard::UndoTurn(Turn turn)
{
    Position start = turn.front();
    Position end = turn.back();

    ChessPiece* tmp = GetPieceAt(end.row, end.col);

    SetPieceAt(tmp, start.row, start.col);
    SetPieceAt(nullptr, end.row, end.col);

    for (Position pos : turn)
    {
        if (pos.capturePiece == nullptr)
            continue;

        Position captureePos = pos.capturePiece->GetPiecePosition();

        pos.capturePiece->SetCaptured(false);
        SetPieceAt(pos.capturePiece, captureePos.row, captureePos.col);
    }
}

int GameBoard::GetPiecesCountByColorAtRow(int row, PieceColor color)
{
    int count = 0;

    if (row > _bRows)
        return 0;

    for (int i = 0; i < _bCols; i++)
        if (ChessPiece* piece = GetPieceAt(row, i))
            if (piece->GetPieceColor() == color)
                count++;

    return count;
}

int GameBoard::GetNeightboursFriendlyCount(ChessPiece* piece)
{
    int count = 0;

    for (const int* offset : _offset)
    {
        Position pos = piece->GetPiecePosition();
        int rowOffset = pos.row + offset[0];
        int colOffset = pos.col + offset[1];

        if (!IsInBoard(rowOffset, colOffset))
            continue;

        if (IsFieldEmpty(rowOffset, colOffset))
            continue;

        ChessPiece* foundPiece = GetPieceAt(rowOffset, colOffset);

        if (foundPiece->GetPieceColor() == piece->GetPieceColor())
            count++;
    }

    return count;
}

int GameBoard::GetPiecesCountByColor(PieceColor color)
{
    int count = 0;

    for (int i = 0; i < _bRows; i++)
        for (int j = 0; j < _bCols; j++)
            if (ChessPiece* piece = GetPieceAt(i, j))
                if (piece->GetPieceColor() == color)
                    count++;

    return count;
}

int GameBoard::GetPiecesCountByColorAndType(PieceColor color, PieceType type)
{
    int count = 0;

    for (int i = 0; i < _bRows; i++)
        for (int j = 0; j < _bCols; j++)
            if (ChessPiece* piece = GetPieceAt(i, j))
                if (piece->GetPieceColor() == color && piece->GetPieceType() == type)
                    count++;

    return count;
}

std::vector<ChessPiece*> GameBoard::GetAllChessPiecesByColor(PieceColor color)
{
    std::vector<ChessPiece*> pieces;

    for (int i = 0; i < _bRows; i++)
        for (int j = 0; j < _bCols; j++)
            if (ChessPiece* piece = GetPieceAt(i, j))
                if (piece->GetPieceColor() == color)
                    pieces.push_back(piece);

    return pieces;
}

std::vector<ChessPiece*> GameBoard::GetAllChessPieces()
{
    std::vector<ChessPiece*> pieces;

    for (int i = 0; i < _bRows; i++)
        for (int j = 0; j < _bCols; j++)
            if (ChessPiece* piece = GetPieceAt(i, j))
                pieces.push_back(piece);

    return pieces;
}

std::vector<Position> GameBoard::GetSlots()
{
    std::vector<Position> bSlots;

    for (int i = 0; i < _bRows; i++)
        for (int j = 0; j < _bCols; j++)
            bSlots.push_back(Position(i, j));

    return bSlots;
}

void GameBoard::SetPieces(std::vector<ChessPiece>& pieces)
{
    Clear();

    for (ChessPiece& piece: pieces)
    {
        Position pos = piece.GetPiecePosition();
        AddPiece(piece.GetPieceType(), piece.GetPieceColor(), pos.row, pos.col);
    }
}

bool GameBoard::Undo()
{
    if (_history.empty())
        return false;

    HistoryItem item = _history.back();

    std::string stringifiedTurn;
    std::vector<ChessPiece> piecesBefore;
    std::vector<ChessPiece> piecesAfter;
    PieceColor color;

    std::tie(stringifiedTurn, _movesCounter, piecesBefore, piecesAfter, color) = item;

    sGameManager->SetColorOnTurn(color);

    Clear(false);

    for (ChessPiece& piece: piecesBefore)
    {
        Position pos = piece.GetPiecePosition();
        AddPiece(piece.GetPieceType(), piece.GetPieceColor(), pos.row, pos.col);
    }

    _historyRedo.push_back(item);
    _history.pop_back();

    return true;
}

bool GameBoard::ReDo()
{
    if (_historyRedo.empty())
        return false;

    HistoryItem item = _historyRedo.back();

    std::string stringifiedTurn;
    std::vector<ChessPiece> piecesBefore;
    std::vector<ChessPiece> piecesAfter;
    PieceColor color;

    std::tie(stringifiedTurn, _movesCounter, piecesBefore, piecesAfter, color) = item;

    // V případě vracení tahu opačně
    sGameManager->SetColorOnTurn(color == PIECE_COLOR_BLACK ? PIECE_COLOR_WHITE : PIECE_COLOR_BLACK);

    Clear(false);

    for (ChessPiece& piece: piecesAfter)
    {
        Position pos = piece.GetPiecePosition();
        AddPiece(piece.GetPieceType(), piece.GetPieceColor(), pos.row, pos.col);
    }

    _history.push_back(item);
    _historyRedo.pop_back();

    return true;
}

std::string GameBoard::StringifyTurn(Turn& turn)
{
    int idx = 0;
    std::ostringstream stream;

    for (Position pos : turn)
    {
        char colLetter = ASCII_LETTER_A + pos.col;
        int rowNumber = _bRows - pos.row;

        stream << colLetter << rowNumber;

        if (turn.size() - 1 > idx)
            stream << '-';

        idx++;
    }

    return stream.str();
}

std::string GameBoard::ToString()
{
    std::stringstream stream;

    stream << "   ";

    for (int j = 0; j < _bCols; j++)
    {
        stream << j << " ";
    }

    stream << "\n";

    stream << "   ---------------\n";

    for (int i = 0; i < _bRows; i++)
    {
        stream << _bRows - i << '|';

        for (int j = 0; j < _bCols; j++)
        {
            ChessPiece* piece = GetPieceAt(i, j);

            if (piece == nullptr)
                stream << ' ' << ((j + i) % 2 == 0 ? '+' : '-');
            else
            {
                if (piece->GetPieceType() == PIECE_TYPE_PAWN)
                    stream << ' ' << (piece->GetPieceColor() == PIECE_COLOR_BLACK ? 'b' : 'w');
                else
                    stream << ' ' << (piece->GetPieceColor() == PIECE_COLOR_BLACK ? 'B' : 'W');
            }
        }

        stream << " |" << i << "\n";
    }

    stream << "   ---------------\n";
    stream << "   A B C D E F G H\n";

    return stream.str();
}
