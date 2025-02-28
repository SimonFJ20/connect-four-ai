from __future__ import annotations
from board import Board
import json
import random
from piece import CROSS, CIRCLE
from game import start_game, GameResult, Player

class DTModel(Player):
    def __init__(self, piece: int):
        self.piece = piece

        self.choices: dict[int, int] = {}
        self.current_choices: list[int] = []

    def clear_choices(self):
        self.current_choices = []

    def reward(self):
        for choice in self.current_choices:
            self.choices[choice] += 1

    def punish(self):
        for choice in self.current_choices:
            self.choices[choice] -= 1

    def interned_choice(self, choice: Board) -> Board:
        flips = 2
        for _ in range(flips):
            rotations = 4
            for _ in range(rotations):
                if choice.as_key() in self.choices:
                    return choice
                choice.rotate()
            choice.flip()
        self.choices[choice.as_key()] = 0
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

    def make_choice(self, board: Board) -> int:
        (choice_pos, choice_key) = self.random_viable_choice(self.piece, board)
        self.current_choices.append(choice_key)
        return choice_pos

    def save_to_file(self):
        with open("ai.json", "w") as file:
            file.write(json.dumps(self.choices))

    def load_from_file(self):
        with open("ai.json", "r") as file:
            vs = json.loads(file.read())
            self.choices = {}
            for key in vs:
                self.choices[int(key)] = vs[key]

def train(model: DTModel, iterations: int) -> None:
    turnee = model
    other = DTModel(CROSS if turnee.piece == CIRCLE else CIRCLE)

    print(f"Training for {iterations:_} iterations...")

    for iteration in range(1, iterations + 1):
        if iteration % (iterations / 100) == 0:
            progress = (iteration + 1) / iterations
            print(f"{progress * 100:3.0f}%")

        result = start_game(p1=turnee, p2=other, quiet=True)

        match result:
            case GameResult.P1Won:
                turnee.reward()
                other.punish()
            case GameResult.P2Won:
                turnee.punish()
                other.reward()
            case GameResult.Draw:
                pass

        turnee.clear_choices()
        other.clear_choices()

        temp = turnee
        turnee = other
        other = temp

    print(f"Done.")
