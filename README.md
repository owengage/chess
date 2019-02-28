# C++ Chess WIP

A chess game/engine written in C++. So far all chess moves have been encoded in the game, allowing a user to generate
all posible moves allowable. This includes checking for en passant, castling, check, checkmate, and stalemate when no
moves are available.

## Current WIP

Currently trying to rework the use of history. I want to remove storing the history of the game so that the game object
itself it a cheap object. From there I can work on building the chess engine. I need the game to be small since I intend
to build a move tree out of the game objects.
 
I need to
* Update the `has_moved` bool for pieces in move generation so that things like en passant and castling can work.
* Get checkmate working again. I had to break it to stop some infinite looping since game validates moves. 
* Switch away from `std::variant` for storing state of a square. Move to custom type that stores all required
information in a single byte, rather than 2 (three with new `has_moved` bool). This plus maybe shrinking `Loc` should
allow a game object to fit in a cache line of the CPU.
## TODOs

* Stalemate through 'inactivity'
* Better chess viewer
* Automated chess player
* Write Algebraic Notation or Portable Game Notation parser
