import json
import random
import sys
from board import Board
from piece import CROSS, CIRCLE
from game import start_game, GameResult, Player

class DTModel(Player):
    def __init__(self, piece: int):
        self.piece = piece

        self.choices: dict[int, int] = {}
        self.current_choices: list[int] = []

    def clear_choices(self):
        self.current_choices = []

    def report_win(self):
        for choice in self.current_choices:
            self.choices[choice] += 4

    def report_loss(self):
        for choice in self.current_choices:
            self.choices[choice] -= 4

    def report_draw(self):
        consequence =  random.randint(-1, 1)
        for choice in self.current_choices:
            self.choices[choice] += consequence

    def make_choice(self, board: Board) -> int:
        choice_hashes = [
            (pos, self.pruned_choice(board.with_play(self.piece, pos)))
            for pos in board.possible_plays()
        ]
        choices = [(p, hash, self.choices[hash]) for p, hash in choice_hashes]
        max_weight = max(weight for _, _, weight in choices)
        candidates = [(p, h) for p, h, w in choices if w >= max_weight - 4]

        assert len(candidates) > 0
        (pos, hash) = candidates[random.randint(0, len(candidates) - 1)]
        self.current_choices.append(hash)
        return pos

    def pruned_choice(self, choice: Board) -> int:
        hash = 0
        for _ in range(2):
            for _ in range(4):
                hash = choice.hash()
                if hash in self.choices:
                    return hash
                choice.rotate()
            choice.flip()
        self.choices[hash] = 0
        return hash

    def size(self) -> tuple[int, int]:
        return len(self.choices), sys.getsizeof(self.choices)


    def save_to_file(self, path: str):
        with open(path, "w") as file:
            file.write(json.dumps(self.choices, indent=4))

    def load_from_file(self, path: str):
        with open(path, "r") as file:
            data = json.loads(file.read())
            self.choices = { int(key): data[key] for key in data }


def train_dt_model(model: DTModel, iterations: int) -> None:
    turnee = model
    other = DTModel(CROSS if turnee.piece == CIRCLE else CIRCLE)

    print(f"Training for {iterations:_} iterations...")

    for i in range(iterations):
        if i % (iterations / 100) == 0:
            progress = (i + 1) / iterations
            print(f"{progress * 100:3.0f}%")

        result = start_game(p1=turnee, p2=other, quiet=True)

        match result:
            case GameResult.P1Won:
                turnee.report_win()
                other.report_loss()
            case GameResult.P2Won:
                turnee.report_loss()
                other.report_win()
            case GameResult.Draw:
                turnee.report_draw()
                other.report_draw()
                pass

        turnee.clear_choices()
        other.clear_choices()

        temp = turnee
        turnee = other
        other = temp

    (entries, size) = model.size()
    print(f"Done! Size: {entries:_} entries, {size:_} bytes total")
