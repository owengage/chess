#include <chess/Game.h>
#include <chess/Suggester.h>
#include <chess/BasicDriver.h>

#include <chess/text/print.h>

#include <iostream>
#include <string>

int main()
{
    auto driver = chess::BasicDriver{}; // TODO: Let player pick promotion
    auto game = chess::Game{driver};
    auto suggester = chess::Suggester{game.board(), chess::evaluate_with_summation};
    auto move_src = std::string{};
    auto move_dest = std::string{};


    for (bool valid = true; valid;)
    {
        valid = false;
        while (!valid)
        {
            chess::text::print(std::cout, game.board());
            std::cout << "Enter move source: ";
            std::cin >> move_src;

            std::cout << "Enter move destination: ";
            std::cin >> move_dest;

            valid = game.move(move_src.c_str(), move_dest.c_str());
            if (!valid)
            {
                std::cout << "Incorrect move, try again.\n";
            }
        }

        suggester = chess::Suggester{game.board(), chess::evaluate_with_summation};
        auto suggestion = suggester.suggest();
        game.move(suggestion.src, suggestion.dest);
    }
}