from enum import Enum
from board import Board


class GameResult(Enum):
    Draw = 0
    Player1Won = 1
    Player2Won = 2


class Player:
    piece: int

    def make_play(self, board: Board) -> None:
        _ = board
        raise Exception("should not call abstract class method")


class HumanPlayer(Player):
    def __init__(self, piece: int):
        self.piece = piece

    def make_play(self, board: Board) -> None:
        print("Your turn (0..8)")
        possible_choices = board.possible_plays()
        while True:
            text = input("> ").strip()
            if text == "":
                continue
            choice = int(text)
            if choice not in possible_choices:
                print("invalid choice")
                continue
            break
        board.place_piece_at(self.piece, choice)


def start_game(player_1: Player, player_2: Player, quiet: bool) -> GameResult:
    board = Board()
    turn = player_1
    other = player_2
    while True:
        turn.make_play(board)

        if not quiet:
            board.print()

        if board.piece_has_won(turn.piece):
            if turn.piece == player_1.piece:
                return GameResult.Player1Won
            elif turn.piece == player_2.piece:
                return GameResult.Player2Won
            else:
                raise Exception(f"invalid turn piece {turn.piece}")
        elif board.board_filled():
            return GameResult.Draw

        temp_turn = turn
        turn = other
        other = temp_turn
