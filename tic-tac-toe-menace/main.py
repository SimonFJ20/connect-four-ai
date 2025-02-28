from __future__ import annotations
from enum import Enum
import json
import random
import time
from piece import Piece
from raw_board import Board


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

    def viable_choices(self, board: Board) -> list[tuple[int, int]]:
        def candidate_weight(choice: tuple[int, int]) -> int:
            return self.choices[choice[1]]

        def board_with_play(pos: int) -> Board:
            return board.with_raw_play(self.piece.value, pos)

        def interned_board(choice: Board) -> int:
            return self.interned_choice(choice).as_key()

        # maybe a little spooky - a function that returns a function
        # the alternative was some really ugly lambdas that didn't sit right with me
        def candidate_within_tolerance(max_weight: int):
            def within_tolerance(candidate: tuple[int, int]) -> bool:
                CANDIDATE_TOLERANCE = 4
                weight = candidate_weight(candidate)
                return weight >= max_weight - CANDIDATE_TOLERANCE

            return within_tolerance

        def choice_at_position(pos: int) -> tuple[int, int]:
            return (pos, interned_board(board_with_play(pos)))

        candidates = [choice_at_position(pos) for pos in board.possible_plays()]
        max_weight = max(map(candidate_weight, candidates))
        candidates = filter(candidate_within_tolerance(max_weight), candidates)
        candidates = list(candidates)

        if len(candidates) == 0:
            raise Exception(
                "unreachable: board is not filled, so there should be at least one viable play"
            )

        return candidates

    def random_viable_choice(self, board: Board) -> tuple[int, int]:
        candidate_indices = self.viable_choices(board)
        return candidate_indices[random.randint(0, len(candidate_indices) - 1)]

    def make_play(self, board: Board) -> None:
        (choice_pos, choice_key) = self.random_viable_choice(board)
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
    max_iterations = 100_000
    iterations_per_batch = max_iterations / 10
    batch_started_at = time.time()

    for iterations in range(1, max_iterations + 1):
        if iterations % iterations_per_batch == 0:
            iterations_percentage = (iterations * 100) / max_iterations
            batch_ended_at = time.time()
            batch_took = batch_ended_at - batch_started_at
            print(f"{iterations_percentage}% trained in {batch_took} seconds")
            batch_started_at = time.time()
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
