EMPTY = 0
CROSS = 1
CIRCLE = 2


def piece_to_colored_and_indexed_str(piece: int, pos: int) -> str:
    match piece:
        case 0:
            return f"\x1b[0;37m{pos}\x1b[0m"
        case 1:
            return "\x1b[1;91mX\x1b[0m"
        case 2:
            return "\x1b[1;94mO\x1b[0m"
    raise Exception(f"invalid piece {piece}")


def piece_to_str(piece: int) -> str:
    match piece:
        case 0:
            return " "
        case 1:
            return "X"
        case 2:
            return "O"
    raise Exception(f"invalid piece {piece}")
