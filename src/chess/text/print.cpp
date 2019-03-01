#include <type_traits>
#include <ostream>

#include <chess/text/print.h>

namespace text = chess::text;

namespace
{
    /**
     * A potential alternative to these structs is a variable template. We want to restrict the types this works with to
     * just the Square variants types. With variable templates we'd do this with some enable_if magic like
     *
     *  std::enable_if_t<std::is_base_of_v<chess::PieceCommon, T> || std::is_same_v<chess::Empty, T>, void>
     *
     *  on the template variable and then make specialisations for each. Problem with this is that if we miss a
     *  specialisation then we just get whatever the basic version of the template is, rather than an error. With this
     *  struct version, it doesn't find value on the base template and so fails to compile.
     */
    template<typename> struct BaseSymbol {};
    template<> struct BaseSymbol<chess::Pawn> { static constexpr char value = 'p'; };
    template<> struct BaseSymbol<chess::Rook> { static constexpr char value = 'r'; };
    template<> struct BaseSymbol<chess::Knight> { static constexpr char value = 'n'; };
    template<> struct BaseSymbol<chess::Bishop> { static constexpr char value = 'b'; };
    template<> struct BaseSymbol<chess::King> { static constexpr char value = 'k'; };
    template<> struct BaseSymbol<chess::Queen> { static constexpr char value = 'q'; };
    template<> struct BaseSymbol<chess::Empty> { static constexpr char value = '.'; };

    template<typename T>
    inline constexpr char base_symbol = BaseSymbol<T>::value;

    struct PrinterVisitor
    {
        template<typename T>
        char operator()(T p)
        {
            auto sym = base_symbol<T>;
            auto coloured_sym = p.colour() == chess::Colour::white ? std::toupper(sym) : std::tolower(sym);
            return static_cast<char>(coloured_sym);
        }

        char operator()(chess::Empty p)
        {
            auto sym = base_symbol<chess::Empty>;
            return sym;
        }
    };
}

void text::print(std::ostream & os, Board const& board)
{
    auto constexpr top_row = "   a b c d e f g h";

    os << top_row << "\n\n";

    for (int y = Loc::side_size - 1; y >= 0; --y)
    {
        os << y + 1 << "  ";
        for (int x = 0; x < Loc::side_size; ++x)
        {
            auto sq = board[{x,y}];
            os << std::visit(PrinterVisitor{}, sq) << ' ';
        }
        os << ' ' << y + 1 << '\n';
    }

    os << '\n' << top_row << "\n";
}