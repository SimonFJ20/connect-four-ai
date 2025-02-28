from enum import Enum

class Piece(Enum):
    Empty = 0
    Cross = 1
    Circle = 2

    def to_colored_and_indexed_str(self, pos: int) -> str:
        match self:
            case Piece.Empty:
                return f"\x1b[0;37m{pos}\x1b[0m"
            case Piece.Cross:
                return "\x1b[1;91mX\x1b[0m"
            case Piece.Circle:
                return "\x1b[1;94mO\x1b[0m"

    def to_str(self) -> str:
        match self:
            case Piece.Empty:
                return " "
            case Piece.Cross:
                return "X"
            case Piece.Circle:
                return "O"

