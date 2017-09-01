#ifndef GAME_EXCEPTION_H
#define GAME_EXCEPTION_H

#include <exception>

class IllegalTurnException : public std::exception { };
class MandatoryTurnRequiredException : public std::exception { };

#endif // GAME_EXCEPTION_H
