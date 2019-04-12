#pragma once

#include <chess/Driver.h>

namespace chess
{
    struct DriverMock : public Driver
    {
        MOCK_METHOD2(promote, Square(Game const&, Move const&));
        MOCK_METHOD2(checkmate, void(Game const&, Move const&));
        MOCK_METHOD2(stalemate, void(Game const&, Move const&));
    };
}