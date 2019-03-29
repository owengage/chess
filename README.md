# C++ Chess WIP

A chess game/engine written in C++. So far all chess moves have been encoded in the game, allowing a user to generate
all possible moves allowable. This includes checking for en passant, castling, check, checkmate, and stalemate when no
moves are available.

## Current work

I'm writing a PGN parser. Once this is done I should be able to throw lots of real chess games at my library as a sort
of extensive test suite. I'm splitting this into at least two parts. 

One part is lexing the PGN, while PGN isn't too complicated, when it comes to verifying each move you actually need to
keep track of the game to resolve ambiguity. For example the "movetext" `Ka3` is valid, but if you're halfway through
a game it isn't obvious if there is really a knight that can move there--unless you track the game. So turning the PGN
into high-level tokens will make writing a parser powered by my `Game` class a lot simpler, which is the second part.

In order to push through a nice database of PGN it will be useful to be able to accept PGN with a different starting
position. This requires parsing FEN/EDN notation. My lexer should also be able to decode games one after the other,
and making a parser that is happy to split up games.

* Create parser that returns all the moves for a game
* Create validator that runs moves through a `Game` checking everything agrees.
* Parse FEN and observe that tag in the game parser.

## TODOs

* Speed up move generation. Benchmark!
* Stalemate through 'inactivity'
* Better chess viewer
* Automated chess player
* Write Algebraic Notation or Portable Game Notation parser

## Implementation notes

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