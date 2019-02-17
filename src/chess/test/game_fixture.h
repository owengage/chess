#pragma once

#include <gmock/gmock.h>

#include "driver_mock.h"

namespace chess
{
    struct GameFixture : public testing::Test
    {
        DriverMock driver;
    };
}