from __future__ import annotations
from piece import EMPTY, piece_to_colored_and_indexed_str, piece_to_str


class Board:
    PIECE_BIT_WIDTH = 2

    def __init__(self, val=0) -> None:
        self.board = val

    def piece_at(self, pos: int) -> int:
        return (self.board >> pos * self.PIECE_BIT_WIDTH) & 0b11

    def possible_plays(self) -> list[int]:
        return [pos for pos in range(9) if self.piece_at(pos) == EMPTY]

    def clone(self) -> Board:
        return Board(self.board)

    def place_piece_at(self, piece: int, pos: int):
        self.board |= piece << (pos * self.PIECE_BIT_WIDTH)

    def with_play(self, piece: int, pos: int) -> Board:
        board = self.clone()
        board.place_piece_at(piece, pos)
        return board

    def clear(self):
        self.board = 0

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

    def as_key(self) -> int:
        return self.board

    def piece_has_won(self, piece: int) -> bool:
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
            if all(self.piece_at(pos) == piece for pos in list(combo)):
                return True
        return False

    def board_filled(self) -> bool:
        return len(self.possible_plays()) == 0

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
            piece_to_colored_and_indexed_str(self.piece_at(pos), pos)
            for pos in range(9)
        ]
        print("+---+---+---+")
        print(f"| {s[0]} | {s[1]} | {s[2]} |")
        print("|---+---+---|")
        print(f"| {s[3]} | {s[4]} | {s[5]} |")
        print("|---+---+---|")
        print(f"| {s[6]} | {s[7]} | {s[8]} |")
        print("+---+---+---+")
