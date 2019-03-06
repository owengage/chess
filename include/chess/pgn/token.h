#pragma once

#include <chess/square.h>
#include <chess/pgn/parser.h>

#include <string>
#include <optional>

namespace chess::pgn
{
    struct Token
    {
        virtual ~Token() = default;
        virtual void accept(Parser &) const = 0;
    };

    template<typename Derived>
    struct TokenCommon : Token
    {
        void accept(Parser & parser) const override
        {
            parser.visit(static_cast<Derived const&>(*this));
        }
    };

    struct TagPairOpen : TokenCommon<TagPairOpen>
    {
    };

    struct TagPairName : TokenCommon<TagPairName>
    {
        explicit TagPairName(std::string name) : name{std::move(name)} {}
        std::string name;
    };

    struct TagPairValue : TokenCommon<TagPairValue>
    {
        explicit TagPairValue(std::string value) : value{std::move(value)} {}
        std::string value;
    };

    struct TagPairClose : TokenCommon<TagPairClose>
    {
    };

    struct MoveNumber : TokenCommon<MoveNumber>
    {
        explicit MoveNumber(int number) : number{number} {}
        int number;
    };

    struct ColourIndicator : TokenCommon<ColourIndicator>
    {
        explicit ColourIndicator(Colour colour) : colour{colour} {}
        Colour colour;
    };

    struct SanMove : TokenCommon<SanMove>
    {
        SanMove() = default;
        std::optional<int> dest_x;
        std::optional<int> dest_y;
        std::optional<int> src_x;
        std::optional<int> src_y;
        SquareType type;
        std::optional<SquareType> promotion;
        bool capture = false;
        bool check = false;
        bool checkmate = false;
        bool king_side_castle = false;
        bool queen_side_castle = false;
    };

    struct SyntaxError : TokenCommon<SyntaxError>
    {};

    struct TerminationMarker : TokenCommon<TerminationMarker>
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