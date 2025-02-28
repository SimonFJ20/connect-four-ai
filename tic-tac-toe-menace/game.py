from enum import Enum
from board import Board

class Player:
    piece: int

    def make_choice(self, board: Board) -> int:
        _ = board
        raise NotImplementedError()


class Human(Player):
    def __init__(self, piece: int):
        self.piece = piece

    def make_choice(self, board: Board) -> int:
        print("Your turn (0..8)...")
        possible_choices = board.possible_plays()
        while True:
            text = input("> ").strip()
            if text == "":
                continue
            choice = int(text)
            if choice not in possible_choices:
                print("Invalid choice.")
                continue
            break
        board.place_piece_at(self.piece, choice)
        return choice

class GameResult(Enum):
    Draw = 0
    P1Won = 1
    P2Won = 2

def start_game(p1: Player, p2: Player, quiet: bool) -> GameResult:
    board = Board()

    turnee = p1
    other = p2

    while True:
        choice = turnee.make_choice(board)
        board.place_piece_at(turnee.piece, choice)

        if not quiet:
            board.print()

        if board.piece_has_won(turnee.piece):
            if turnee.piece == p1.piece:
                return GameResult.P1Won
            elif turnee.piece == p2.piece:
                return GameResult.P2Won
            else:
                raise Exception(f"invalid turn piece {turnee.piece}")
        elif board.board_filled():
            return GameResult.Draw

        temp = turnee
        turnee = other
        other = temp
