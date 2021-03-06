# C++ Chess

A chess game/engine written in C++. All chess moves have been encoded in the game, allowing a user to generate
all possible moves allowable. This includes checking for en passant, castling, check, checkmate, and stalemate when no
moves are available. There is also a PGN parser, PGN validator and Minimax chess engine.

## Current work

Performance!

I've started to micro-benchmark parts of the codebase to improve the speed. Given a big enough improvement the chess
engine will be able to search deeper, or at least respond a little faster.

A new namespace `perf` has popped up to contain performance related types. The first thing to enter is `StackVector`.
This is a thin vector-like type that has a maximum capacity and lives on the stack. This helps avoid lots of allocations
when I know the maximum size ahead of time.

### Location optimisations

`StackVector` was used for `Loc::direction` which would generate all the locations in a certain direction from a given start
point. Since the chess board is only 8 across, you know this function will never return more than 8 locations. So simply
storing these on the stack is a lot quicker. My 4-deep exhaustive chess engine took 1.75 s before, and 1.34 s after this
change on my machine, a fairly nice improvement.

Further to that change, I made a lot of `Loc` `constexpr` and changed from storing `x` and `y` to storing a computed
index, since getting this index seemed more common. The 4-deep exhaustive went from 1.34 s down to 0.96 s. So these two
changes nearly halved the suggestion time for a standard start board. I need to start using a more representative board.

### Move generation optimisations

As a new baseline, with the above optimisations I can parse a PGN game (which involves move generation to validate) in 
41 ms.

Move generation used to have a lot of wasted calculation. The `potential_moves` function was heavily overused, for
example it was used to check for check. I would calculate all potential moves, then for each of those moves determine
all the potential moves from there by the opponent to see if they threaten the king (ie the move we generated for the
current player would put them in check).

I split move generation into two parts, one understands from a given board what moves can be made, and the other
tracks whatever it needs to track. For the players move generation this is tracking the full move including the
resultant board. But for checking for check, I can simply fill in a bit-field of what squares are under threat. This
reduces the amount of work by a fair margin. This can also be done for checking for checkmate.

The 41 ms test was reduced to 15 ms.

### Inlining/constexpr

Running a few thousand games through my PGN validator and recording with `perf` pointed out that the `operator[]` on my
Board class was heavily hit, which isn't very surprising. I inlined this by making it `constexpr` (because why not) and
the same test goes down to 8 ms.

### Direction generation

A lot of time is now spend in `Loc::direction`. This function takes a start point, an x direction, and a y direction and
returns a `StackVector` of valid locations for that direction. I've tried a few things to speed this up to no avail:

* Inlining had almost no effect. I imagine it's too much to inline.
* Rewriting the code to compute the amount of hops it will produce and then having a basic loop increasing the index
  that many times also didn't affect the speed.
* Precomuting every `StackVector` that will be produced. This is only `64 squares * 8 directions * 8 ints` so is not
  too large. Unfortunately it looks like the look-up time for a `std::map` and a `std::unordered_map` is slower than
  computing the vector each time. It took us from 8 ms to 10ms for our PGN validation. I also can't generate this at
  compile time since the standard containers are not constexpr. Look-up would still have to be runtime anyway.
  
The final point about look-up taking too long can be avoided if it's just an index lookup. Since a direction is
essentially keyed on a 0-63 int, and two -1, 0, or +1 directions, I can just OR these up into an index and fill a normal
array with them. There will be gaps, but who cares? This took us from 8 to 6.4 ms! Our chess engine example that started
at 1.75 s now takes 0.2 s. A few simple optimisations have given us nearly an order of magnitude of speed-up.

## TODOs

* [x] Write chess game and ability to generate all legal moves
* [x] Write Algebraic Notation or Portable Game Notation parser
* [x] Automated chess player
* [ ] Speed up move generation via benchmarking
* [ ] Alpha-Beta pruning
* [ ] Move ordering
* [ ] Better chess viewer
* [ ] Implement stalemate through 'inactivity'
* [ ] Make the direction look-up table constexpr.

###### Small things

* [ ] Change `Driver::promote()` to just return a `SquareType`.

## Implementation notes

### Bugs found through PGN

While running thousands of games through my `Game` class, I came across several bugs:-

* Most of my Queen side castle tests were on the wrong squares.
* Promotion of pawns when that move was a capture did not promote.
* When promoting a pawn to a piece, if a piece of the same type can also move to the promoted position it caused an
  ambiguity in my SAN move resolver.


### `std::variant` v. custom type

Originally I wanted to have the type representing a square to be stored as a `std::variant`. This worked fine with some
fairly complex template usage. But I then wanted to make sure that the board class would fit in as few cache-lines as
possible. My original `std::variant` based version originally was 2-bytes, making a chess board fit just about into 2
64-byte cache-lines.

In wanting to make the game class also fit into as few cache-lines as possible, I needed to strip away storing the 
history of the game in the game class. But since the game class requires some knowledge of the past I needed to figure
out exactly what extra information I needed to keep in my game. This turned out to be:

1. Whose turn it is;
2. Whether kings, rooks, and pawns have moved before; and
3. The pawn that last moved *iff* it 'double jumped'.

Number 2 is the real stickler here. I could either store some sort of list of which of those pieces have moved somehow,
or store whether they have moved in the square itself. I went with the latter, deciding to pack it in as a bool along
with the colour of the piece.

That meant that my board could still be stored in 2 cache-lines, but it would take up the entirety of them! Ideally I'd
rather such a primitive as the board to fit in a single cache-line.

So how to squeeze this board object down? Well, `std::variant` on my platform is storing the type of piece as a single
byte. I'm then packing the colour (effectively a bool) and whether the piece has moved (another bool) into another byte.
But there's only 7 types a piece can be including empty. Storing that only requires 3 bits, not 8. And the other
information only requires 2. I can pack this all into a single byte quite easily with a bit of masking.

This now means that a board fits perfectly into a cache-line.

I'm yet to do any proper performance testing, but it seems moving away from variant dramatically reduced the time
to run my test suite. I went from 2.5s to 100ms to run the set. Would be interesting to know where the time was spent.
It doesn't seem to be due to the cache-line since adding padding to my square struct doesn't reverse it.

## Use it

Require
* C++17 compiler
* Google Benchmark installed in a standard place