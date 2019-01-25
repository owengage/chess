#pragma once

#include <gmock/gmock.h>

#include "player_mock.h"

namespace chess
{
    struct PlayerFixture : public testing::Test
    {
        PlayerMock p1;
        PlayerMock p2;
    };
}