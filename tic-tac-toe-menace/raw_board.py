from __future__ import annotations
from piece import Piece


# the `raw_piece` methods avoids allocating a Piece as an object
# usually you would allocate an object, because `def piece_at(self, pos: int) -> Piece` is more readable, than `def piece_at(self, pos: int) -> int`
# however, we care about performance too much, and it's about a 2-3x slower using Piece in the code
# because allocating an object is comparatively really, really slow, when done thousands of times in a hot loop
# the `raw` name is just a naming convention i picked to represent that it's not actually an `int` we're passing about,
# semantically, it's a `Piece`, and has all the same constraints as a `Piece` does. (namely either being 0 for Empty, 1 for Cross, or 2 for Circle)
class Board:
    PIECE_BIT_WIDTH = 2

    def __init__(self, val=0) -> None:
        self.board = val

    def raw_piece_at(self, pos: int) -> int:
        return (self.board >> pos * self.PIECE_BIT_WIDTH) & 0b11

    # manually inlined for performance
    # python function calls are super expensive in a hot loop, and there isn't a way to inline functions in python
    # so i've manually inlined it. this saves about ~0.5-1.0 seconds per 10_000 iterations
    # usually i would not do this, but it's relatively easy pickings for performance
    # try using the non-inlined version that's commented out, and seeing what difference it makes on your machine
    def possible_plays(self) -> list[int]:
        # return [pos for pos in range(9) if self.raw_piece_at(pos) == Piece.Empty.value]
        return [
            pos
            for pos in range(9)
            if (self.board >> pos * self.PIECE_BIT_WIDTH) & 0b11 == 0
        ]

    def clone(self) -> Board:
        return Board(self.board)

    def place_raw_piece_at(self, piece: int, pos: int):
        self.board |= piece << pos * self.PIECE_BIT_WIDTH

    def with_raw_play(self, piece: int, pos: int) -> Board:
        board = self.clone()
        board.place_raw_piece_at(piece, pos)
        return board

    def clear(self):
        self.board = 0

    def rotate(self):
        indices = [6, 3, 0, 7, 4, 1, 8, 5, 2]
        pieces = [self.raw_piece_at(pos) for pos in indices]
        self.clear()
        for pos, piece in enumerate(pieces):
            self.place_raw_piece_at(piece, pos)

    def flip(self):
        indices = [6, 7, 8, 3, 4, 5, 0, 1, 2]
        pieces = [self.raw_piece_at(pos) for pos in indices]
        self.clear()
        for pos, piece in enumerate(pieces):
            self.place_raw_piece_at(piece, pos)

    def as_key(self) -> int:
        return self.board

    def piece_has_won(self, piece: Piece) -> bool:
        combos = [
            (0, 1, 2),
            (3, 4, 5),
            (6, 7, 8),
            (0, 3, 6),
            (1, 4, 7),
            (2, 5, 8),
            (0, 4, 8),
            (2, 4, 6),
        ]
        for combo in combos:
            if all(self.raw_piece_at(pos) == piece.value for pos in list(combo)):
                return True
        return False

    def board_filled(self) -> bool:
        return len(self.possible_plays()) == 0

    def __repr__(self) -> str:
        return (
            "["
            + "".join(
                Piece(self.raw_piece_at(pos)).to_str() for pos in range(9)
            ).replace(" ", ".")
            + "]"
        )

    def print(self) -> None:
        s = [
            Piece(self.raw_piece_at(pos)).to_colored_and_indexed_str(pos)
            for pos in range(9)
        ]
        print("+---+---+---+")
        print(f"| {s[0]} | {s[1]} | {s[2]} |")
        print("|---+---+---|")
        print(f"| {s[3]} | {s[4]} | {s[5]} |")
        print("|---+---+---|")
        print(f"| {s[6]} | {s[7]} | {s[8]} |")
        print("+---+---+---+")
