from __future__ import annotations
import json
import random
import time

from piece import Piece

# an example of an implementation board that isn't working with raw integers, i.e., how you'd do it in an ideal world where performance didn't matter (or where python wasn't terrible at optimizing)
# this is about ~15 seconds per 10_000 iterations on my machine, as opposed to ~5-6 seconds for the other implementation of `Board`





class Board:
    PIECE_BIT_WIDTH = 2

    def __init__(self, val=0) -> None:
        self.board = val

    def piece_at(self, pos: int) -> Piece:
        return Piece((self.board >> pos * self.PIECE_BIT_WIDTH) & 0b11)

    def possible_plays(self) -> list[int]:
        return [pos for pos in range(9) if self.piece_at(pos) == Piece.Empty]

    def clone(self) -> Board:
        return Board(self.board)

    def place_piece_at(self, piece: Piece, pos: int):
        self.board |= (piece.value) << (pos * PIECE_BIT_WIDTH)

    def with_play(self, piece: Piece, pos: int) -> Board:
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
            if all(self.piece_at(pos) == piece for pos in list(combo)):
                return True
        return False

    def board_filled(self) -> bool:
        return len(self.possible_plays()) == 0

    def __repr__(self) -> str:
        return (
            "["
            + "".join(
                Piece(self.piece_at(pos)).to_str() for pos in range(9)
            ).replace(" ", ".")
            + "]"
        )

    def print(self) -> None:
        s = [
            Piece(self.piece_at(pos)).to_colored_and_indexed_str(pos)
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
            (pos, board.with_play(self.piece, pos))
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
        board.place_piece_at(self.piece, choice_pos)

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

        board.place_piece_at(Piece.Circle.value, choice)
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
