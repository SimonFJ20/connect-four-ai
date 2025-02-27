from __future__ import annotations
from enum import Enum
import json
import random
import time


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


PIECE_BIT_WIDTH = 2


# the `raw_piece` methods avoids allocating a Piece as an object
# usually you would allocate an object, because `def piece_at(self, pos: int) -> Piece` is more readable, than `def piece_at(self, pos: int) -> int`
# however, we care about performance too much, and it's about a 3x slower using Piece in the code
# because allocating an object is comparatively really, really slow, when done thousands of times in a hot loop
# the `raw` name is just a naming convention i picked to represent that it's not actually an `int` we're passing about,
# semantically, it's a `Piece`, and has all the same constraints as a `Piece` does. (namely either being 0 for Empty, 1 for Cross, or 2 for Circle)
class Board:
    def __init__(self, val=0) -> None:
        self.board = val

    def raw_piece_at(self, pos: int) -> int:
        return (self.board >> pos * PIECE_BIT_WIDTH) & 0b11

    # manually inlined for performance
    # python function calls are super expensive in a hot loop, and there isn't a way to inline functions in python
    # so i've manually inlined it. this saves about ~0.5-1.0 seconds per 10_000 iterations
    # usually i would not do this, but it's relatively easy pickings for performance
    def possible_plays(self) -> list[int]:
        # return [pos for pos in range(9) if self.raw_piece_at(pos) == Piece.Empty.value]
        return [
            pos for pos in range(9) if (self.board >> pos * PIECE_BIT_WIDTH) & 0b11 == 0
        ]

    def clone(self) -> Board:
        return Board(self.board)

    def place_raw_piece_at(self, piece: int, pos: int):
        self.board |= piece << pos * PIECE_BIT_WIDTH

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


class AiPlayer:
    START_WEIGHT = 20
    REWARD = 1
    PUNISHMENT = 1

    def __init__(self, piece: Piece) -> None:
        self.choices: dict[int, int] = {}
        self.current_choices: list[int] = []
        self.piece = piece

    def clear_choices(self):
        self.current_choices = []

    def reward(self):
        for choice in self.current_choices:
            self.choices[choice] += self.REWARD

    def punish(self):
        for choice in self.current_choices:
            self.choices[choice] -= self.PUNISHMENT

    def interned_choice(self, choice: Board) -> Board:
        flips = 2
        for _ in range(flips):
            rotations = 4
            for _ in range(rotations):
                if choice.as_key() in self.choices:
                    return choice
                choice.rotate()
            choice.flip()
        self.choices[choice.as_key()] = self.START_WEIGHT
        return choice

    def make_play(self, board: Board) -> None:
        possible_choices = [
            (pos, board.with_raw_play(self.piece.value, pos))
            for pos in board.possible_plays()
        ]
        candidate_weight = 0
        candidate_indices: list[tuple[int, int]] = []
        for pos, choice in possible_choices:
            choice = self.interned_choice(choice)
            key = choice.as_key()
            if not candidate_weight or self.choices[key] > candidate_weight + 4:
                candidate_weight = self.choices[key]
                candidate_indices = [(pos, key)]
            elif self.choices[key] == candidate_weight:
                candidate_indices.append((pos, key))
        if len(candidate_indices) == 0:
            raise Exception(
                "unreachable: board is not filled, so there should be at least one viable play"
            )
        (choice_pos, choice_key) = candidate_indices[
            random.randint(0, len(candidate_indices) - 1)
        ]
        self.current_choices.append(choice_key)
        board.place_raw_piece_at(self.piece.value, choice_pos)

    def save_to_file(self):
        with open("ai.json", "w") as file:
            file.write(json.dumps(self.choices))

    def load_from_file(self):
        with open("ai.json", "r") as file:
            vs = json.loads(file.read())
            self.choices = {}
            for key in vs:
                self.choices[int(key)] = vs[key]


class InteractiveGameResult(Enum):
    Restarted = 0
    Draw = 1
    PlayerWon = 2
    AiWon = 3


def start_interactive_game(ai: AiPlayer) -> InteractiveGameResult:
    board = Board()
    ai.clear_choices()
    while True:
        print("AI's turn")
        ai.make_play(board)
        board.print()

        if board.piece_has_won(Piece.Cross):
            return InteractiveGameResult.AiWon
        elif board.board_filled():
            return InteractiveGameResult.Draw

        print("Your turn (0..8)")
        possible_choices = board.possible_plays()
        while True:
            text = input("> ").strip()
            if text == ".save":
                ai.save_to_file()
                continue
            elif text == ".load":
                ai.load_from_file()
                return InteractiveGameResult.Restarted
            elif text == ".restart":
                return InteractiveGameResult.Restarted
            elif text == "":
                continue
            choice = int(text)
            if choice not in possible_choices:
                print("invalid choice")
                continue
            break

        board.place_raw_piece_at(Piece.Circle.value, choice)
        board.print()
        if board.piece_has_won(Piece.Circle):
            return InteractiveGameResult.PlayerWon
        elif board.board_filled():
            return InteractiveGameResult.Draw


def train_ai() -> AiPlayer:
    p1 = AiPlayer(Piece.Cross)
    p2 = AiPlayer(Piece.Circle)

    p1_wins = 0
    p2_wins = 0
    draws = 0

    print("Training P1 against P2...")
    num_iterations = 100_000
    time_when_starting = time.time()
    for iterations in range(1, num_iterations + 1):
        if iterations % (num_iterations / 10) == 0:
            print(
                f"{(iterations * 100) / num_iterations}% trained in {time.time() - time_when_starting} seconds"
            )
            time_when_starting = time.time()
        board = Board()
        p1.clear_choices()
        p2.clear_choices()
        while True:
            p1.make_play(board)
            if board.piece_has_won(Piece.Cross):
                p1_wins += 1
                p1.reward()
                p2.punish()
                break
            if board.board_filled():
                draws += 1
                break
            p2.make_play(board)
            if board.piece_has_won(Piece.Circle):
                p2_wins += 1
                p2.reward()
                p1.punish()
                break
            if board.board_filled():
                draws += 1
                break

    print(
        f"Games: {p1_wins + p2_wins + draws}\tScore: p1: {p1_wins}, p2: {p2_wins}, draws: {draws}"
    )

    return p1


def main():
    ai = train_ai()

    while True:
        print("\nNew game")
        result = start_interactive_game(ai)
        match result:
            case InteractiveGameResult.PlayerWon:
                print("Player won!")
            case InteractiveGameResult.AiWon:
                print("AI won!")
            case InteractiveGameResult.Draw:
                print("Draw!")
            case InteractiveGameResult.Restarted:
                continue


try:
    main()
except KeyboardInterrupt:
    pass
