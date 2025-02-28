from __future__ import annotations
from piece import EMPTY, piece_board_str, piece_to_str

PIECE_SIZE = 2
PIECE_MASK = 0b11

class Board:
    def __init__(self, bitfield=0) -> None:
        self.bitfield = bitfield

    def piece_at(self, pos: int) -> int:
        return (self.bitfield >> pos * PIECE_SIZE) & PIECE_MASK

    def possible_plays(self) -> list[int]:
        return [pos for pos in range(9) if self.piece_at(pos) == EMPTY]

    def clone(self) -> Board:
        return Board(self.bitfield)

    def place_piece_at(self, piece: int, pos: int):
        self.bitfield |= piece << pos * PIECE_SIZE

    def with_play(self, piece: int, pos: int) -> Board:
        board = self.clone()
        board.place_piece_at(piece, pos)
        return board

    def clear(self):
        self.bitfield = 0

    def rotate(self):
        indices = [6, 3, 0, 7, 4, 1, 8, 5, 2]
        pieces = [self.piece_at(pos) for pos in indices]
        self.clear()
        for pos, piece in enumerate(pieces):
            self.place_piece_at(piece, pos)

    def flip(self):
        indices = [6, 7, 8, 3, 4, 5, 0, 1, 2]
        pieces = [self.piece_at(pos) for pos in indices]
        self.clear()
        for pos, piece in enumerate(pieces):
            self.place_piece_at(piece, pos)

    def hash(self) -> int:
        return self.bitfield

    def board_filled(self) -> bool:
        return all(self.piece_at(pos) != EMPTY for pos in range(9))

    def piece_has_won(self, piece: int) -> bool:
        # fmt: off
        patterns = [
            # horizontal
            (0, 1, 2), (3, 4, 5),  (6, 7, 8),
            # vertical
            (0, 3, 6), (1, 4, 7),  (2, 5, 8),
            # diagonal
            (0, 4, 8), (2, 4, 6),
        ]
        # fmt: on
        for pattern in patterns:
            if all(self.piece_at(pos) == piece for pos in list(pattern)):
                return True
        return False

    def __repr__(self) -> str:
        return (
            "["
            + "".join(piece_to_str(self.piece_at(pos)) for pos in range(9)).replace(
                " ", "."
            )
            + "]"
        )

    def print(self) -> None:
        s = [
            piece_board_str(self.piece_at(pos), pos)
            for pos in range(9)
        ]
        print("+---+---+---+")
        print(f"| {s[0]} | {s[1]} | {s[2]} |")
        print("|---+---+---|")
        print(f"| {s[3]} | {s[4]} | {s[5]} |")
        print("|---+---+---|")
        print(f"| {s[6]} | {s[7]} | {s[8]} |")
        print("+---+---+---+")
