#include "GameManager.h"
#include "GameExceptions.h"

#include <QDebug>

GameManager::GameManager()
{
    _gameType = GAME_GOTHIC_CHECKERS;
    _gameChanged = false;
}

GameManager::~GameManager()
{
    delete _game;
    delete _board;
    delete _whitePlayer;
    delete _blackPlayer;
}

void GameManager::DoTurn(Turn turn)
{
    Player* player = GetPlayerOnTurn();

    if (player->IsHuman())
    {
        _game->ValidateTurn(player, turn);
        _board->DoTurn(turn);
        _game->OnTurnDone(player, &turn);
        SwapPlayers();
    }
    else if (player->IsComputer())
    {
        _board->DoTurn(turn);
        _game->OnTurnDone(player, &turn);
        SwapPlayers();
    }

    _gameChanged = true;
}

Player* GameManager::GetOponentOf(Player* player)
{
    if (player == _whitePlayer)
        return _blackPlayer;

    return _whitePlayer;
}

void GameManager::SwapPlayers()
{
    if (_playerOnTurn == _whitePlayer)
        _playerOnTurn = _blackPlayer;
    else
        _playerOnTurn = _whitePlayer;
}

void GameManager::Initialize()
{
    switch (_gameType)
    {
        case GAME_GOTHIC_CHECKERS:
            _board = new GameBoard(GOTHIC_CHECKERS_BOARD_SIZE);
            _game = new GameGothicCheckers();
            break;
        case GAME_OSETIC_CHECKERS:
            break;
        case GAME_CHESS:
            break;
        default: break;
    }

    _blackPlayer = new Player();
    _blackPlayer->SetPlayerColor(PIECE_COLOR_BLACK);

    _whitePlayer = new Player();
    _whitePlayer->SetPlayerColor(PIECE_COLOR_WHITE);

    _playerOnTurn = _whitePlayer;

    _whitePlayer->SetPlayerType(PLAYER_TYPE_HUMAN);
    _blackPlayer->SetPlayerType(PLAYER_TYPE_HUMAN);
}

void GameManager::SetColorOnTurn(PieceColor color)
{
    if (color == PIECE_COLOR_WHITE)
        _playerOnTurn = _whitePlayer;
    else
        _playerOnTurn = _blackPlayer;
}

GameManager* GameManager::GetInstance()
{
    static GameManager instance;
    return &instance;
}
