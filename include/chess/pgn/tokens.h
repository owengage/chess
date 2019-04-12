#pragma once

#include <chess/Square.h>

#include <string>
#include <optional>

namespace chess::pgn
{
    struct TagPairOpen
    {
    };

    struct TagPairName
    {
        explicit TagPairName(std::string name) : name{std::move(name)} {}
        std::string name;
    };

    struct TagPairValue
    {
        explicit TagPairValue(std::string value) : value{std::move(value)} {}
        std::string value;
    };

    struct TagPairClose
    {
    };

    struct MoveNumber
    {
        explicit MoveNumber(int number) : number{number} {}
        int number;
    };

    struct ColourIndicator
    {
        explicit ColourIndicator(Colour colour) : colour{colour} {}
        Colour colour;
    };

    struct SanMove
    {
        SanMove() = default;
        std::optional<int> dest_x;
        std::optional<int> dest_y;
        std::optional<int> src_x;
        std::optional<int> src_y;
        std::optional<SquareType> promotion;
        SquareType type = SquareType::empty;
        bool capture = false;
        bool check = false;
        bool checkmate = false;
        bool king_side_castle = false;
        bool queen_side_castle = false;
        std::string original_text = "";
    };

    struct SyntaxError
    {};

    struct AlternativeOpen
    {};

    struct AlternativeClose
    {};

    struct TerminationMarker
    {
        enum class Type
        {
            white_win,
            black_win,
            draw,
            in_progress,
        };

        Type type;
    };
}