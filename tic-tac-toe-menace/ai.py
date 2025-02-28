from __future__ import annotations
from board import Board
import json
import time
import random
from piece import CROSS, CIRCLE
from game import start_game, GameResult, Player


class AiPlayer(Player):
    ai: Ai

    def __init__(self, piece: int, ai: Ai):
        self.ai = ai
        self.piece = piece

    def make_play(self, board: Board) -> None:
        self.ai.make_play(self.piece, board)


class Ai:
    START_WEIGHT = 20
    REWARD = 1
    PUNISHMENT = 1

    def __init__(self) -> None:
        self.choices: dict[int, int] = {}
        self.current_choices: list[int] = []

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

    def viable_choices(self, piece: int, board: Board) -> list[tuple[int, int]]:
        def candidate_weight(choice: tuple[int, int]) -> int:
            return self.choices[choice[1]]

        def board_with_play(pos: int) -> Board:
            return board.with_play(piece, pos)

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

    def random_viable_choice(
        self,
        piece: int,
        board: Board,
    ) -> tuple[int, int]:
        candidate_indices = self.viable_choices(piece, board)
        return candidate_indices[random.randint(0, len(candidate_indices) - 1)]

    def make_play(self, piece: int, board: Board) -> None:
        (choice_pos, choice_key) = self.random_viable_choice(piece, board)
        self.current_choices.append(choice_key)
        board.place_piece_at(piece, choice_pos)

    def save_to_file(self):
        with open("ai.json", "w") as file:
            file.write(json.dumps(self.choices))

    def load_from_file(self):
        with open("ai.json", "r") as file:
            vs = json.loads(file.read())
            self.choices = {}
            for key in vs:
                self.choices[int(key)] = vs[key]


def train_ai() -> Ai:
    ai1 = Ai()
    ai2 = Ai()

    ai1_player = AiPlayer(CROSS, ai1)
    ai2_player = AiPlayer(CIRCLE, ai2)

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

        result = start_game(ai1_player, ai2_player, quiet=True)
        ai1.clear_choices()
        ai2.clear_choices()

        match result:
            case GameResult.Player1Won:
                p1_wins += 1
                ai1.reward()
                ai2.punish()
            case GameResult.Player2Won:
                p2_wins += 1
                ai2.reward()
                ai1.punish()
            case GameResult.Draw:
                draws += 1

    print(
        f"Games: {p1_wins + p2_wins + draws}\tScore: p1: {p1_wins}, p2: {p2_wins}, draws: {draws}"
    )

    ai1.clear_choices()

    return ai1
