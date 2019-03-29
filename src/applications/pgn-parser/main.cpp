#include <chess/pgn/move_parser.h>

#include <iostream>
#include <string>
#include <fstream>

// For stat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using chess::pgn::MoveParser;
using chess::pgn::IncompleteGameError;

namespace
{
    struct Args
    {
        std::vector<std::string> pgn_files;
        std::vector<std::string> errors;
    };

    Args parse_args(int argc, char const ** argv)
    {
        Args args{};

        // eat first arg (the program name)
        argc--;
        argv++;

        for (int i = 0; i < argc; ++i)
        {
            struct stat status{};
            auto stat_result = ::stat(argv[i], &status);

            auto potential_filename = std::string{argv[i]};

            if (stat_result != 0)
            {
                args.errors.emplace_back("Could not check file: " + potential_filename);
            }
            else
            {
                if (S_ISREG(status.st_mode))
                {
                    args.pgn_files.emplace_back(potential_filename);
                }
                else
                {
                    args.errors.emplace_back(potential_filename + " exists, but is not a regular file");
                }
            }
        }

        if (args.pgn_files.empty())
        {
            args.errors.emplace_back("No files given");
        }

        return args;
    }
}

int main(int argc, char const ** argv)
{
    auto args = parse_args(argc, argv);

    if (!args.errors.empty())
    {
        std::cerr << "The following errors occurred:\n";
        for (auto const& error : args.errors)
        {
            std::cerr << '\t' << error << '\n';
        }

        return 1;
    }

    for (auto const& filename : args.pgn_files)
    {
        auto ifs = std::ifstream{filename};

        if (ifs)
        {
            auto parser = MoveParser{ifs};
            auto game_count = 0;

            try
            {
                while (parser.next_game())
                {
                    game_count++;
                }
            }
            catch (IncompleteGameError const&)
            {
                std::cerr << "Incomplete game found\n";
            }

            std::cout << filename << " contains " << game_count << " games.\n";
        }
    }
}