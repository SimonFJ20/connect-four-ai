from __future__ import annotations
from enum import Enum
import json
from typing import Optional
import random

class Ch(Enum):
    Empty = 0
    Cross = 1
    Circle = 2

def ch_str_fancy(ch: Ch, i = 0) -> str:
    match ch:
        case Ch.Empty:
            return f"\x1b[0;37m{i}\x1b[0m"
        case Ch.Cross:
            return "\x1b[1;91mX\x1b[0m"
        case Ch.Circle:
            return "\x1b[1;94mO\x1b[0m"

def ch_str(ch: Ch) -> str:
    match ch:
        case Ch.Empty:
            return " "
        case Ch.Cross:
            return "X"
        case Ch.Circle:
            return "O"

CH_WIDTH = 2

class Board:
    def __init__(self, val = 0) -> None:
        self.val = val

    def possible_plays(self) -> list[int]:
        return [i for i in range(9) if (self.val >> i * CH_WIDTH & 0b11) == 0]

    def clone(self) -> Board:
        return Board(self.val)

    def play(self, ch: Ch, i: int):
        self.val |= ch.value << i * CH_WIDTH

    def with_play(self, ch: Ch, i: int) -> Board:
        board = self.clone()
        board.play(ch, i)
        return board

    def rotate(self):
        idcs = [6, 3, 0, 7, 4, 1, 8, 5, 2]
        vals = [self.val >> i * CH_WIDTH & 0b11 for i in idcs]
        self.val = 0
        for i, v in enumerate(vals):
            self.val |= v << i * CH_WIDTH

    def flip(self):
        idcs = [6, 7, 8, 3, 4, 5, 0, 1, 2]
        vals = [self.val >> i * CH_WIDTH & 0b11 for i in idcs]
        self.val = 0
        for i, v in enumerate(vals):
            self.val |= v << i * CH_WIDTH

    def key(self) -> int:
        return self.val

    def place(self, i: int) -> Ch:
        return Ch(self.val >> i * CH_WIDTH & 0b11)

    def player_has_won(self, ch: Ch) -> bool:
        combos = [
            (0, 1, 2), (3, 4, 5), (6, 7, 8),
            (0, 3, 6), (1, 4, 7), (2, 5, 8),
            (0, 4, 8), (2, 4, 6),
        ]
        for combo in combos:
            if all(self.place(p) == ch for p in list(combo)):
                return True
        return False

    def is_draw(self) -> bool:
        return len(self.possible_plays()) == 0
                
    def __repr__(self) -> str:
        # print(f"{self.val:b}".rjust(18, "0"))
        return "[" + "".join(ch_str(Ch(self.val >> i * CH_WIDTH & 0b11)) for i in range(9)).replace(" ", ".") + "]"

    def print(self) -> None:
        s = [ch_str_fancy(Ch(self.val >> i * CH_WIDTH & 0b11), i) for i in range(9)]
        print("#############")
        print(f"# {s[0]} | {s[1]} | {s[2]} #")
        print("#---+---+---#")
        print(f"# {s[3]} | {s[4]} | {s[5]} #")
        print("#---+---+---#")
        print(f"# {s[6]} | {s[7]} | {s[8]} #")
        print("#############")

START_WEIGHT = 20
WIN_REWARD = 1
PUNUSHMENT = 1

class AiPlayer:
    def __init__(self, ch: Ch) -> None:
        self.choices: dict[int, int] = {}
        self.current_choices: list[int] = []
        self.ch = ch

    def clear_choices(self):
        self.current_choices = []

    def reward_win(self):
        for choice in self.current_choices:
            self.choices[choice] += WIN_REWARD

    def punish_loss(self):
        for choice in self.current_choices:
            self.choices[choice] -= PUNUSHMENT

    def interned_choice(self, choice: Board) -> Board:
        for _ in range(2):
            for _ in range(4):
                key = choice.key()
                if key in self.choices:
                    return choice
                choice.rotate()
            choice.flip()
        key = choice.key()
        self.choices[key] = START_WEIGHT
        return choice

    def make_play(self, board: Board) -> None:
        possible_choices = [(idx, board.with_play(self.ch, idx)) for idx in board.possible_plays()]
        # candiate_weigth: Optional[int] = None
        candiate_weigth = 0
        candidate_idcs: list[tuple[int, int]] = []
        for idx, choice in possible_choices:
            choice = self.interned_choice(choice)
            key = choice.key()
            if not candiate_weigth or self.choices[key] > candiate_weigth + 4:
                candiate_weigth = self.choices[key]
                candidate_idcs = [(idx, key)]
            elif self.choices[key] == candiate_weigth:
                candidate_idcs.append((idx, key))
        if len(candidate_idcs) == 0:
            raise Exception()
        (choice_idx, choice_key) = candidate_idcs[random.randint(0, len(candidate_idcs) - 1)]
        self.current_choices.append(choice_key)
        board.play(self.ch, choice_idx)

    def save_to_file(self):
        with open("ai.json", "w") as file:
            file.write(json.dumps(self.choices))

    def load_from_file(self):
        with open("ai.json", "r") as file:
            vs = json.loads(file.read())
            self.choices = {}
            for key in vs:
                self.choices[int(key)] = vs[key]

def main():
    p1 = AiPlayer(Ch.Cross)
    p2 = AiPlayer(Ch.Circle)

    games = 0
    p1_wins = 0
    p2_wins = 0
    draws = 0

    print("Training P1 against P2...")

    for _ in range(100000):
        board = Board()
        p1.clear_choices()
        p2.clear_choices()
        while True:
            p1.make_play(board)
            if board.player_has_won(Ch.Cross):
                p1_wins += 1
                p1.reward_win()
                p2.punish_loss()
                break
            if board.is_draw():
                draws += 1
                break
            p2.make_play(board)
            if board.player_has_won(Ch.Circle):
                p2_wins += 1
                p2.reward_win()
                p1.punish_loss()
                break
            if board.is_draw():
                break
        games += 1

    print(f"Games {games}  Score: P1: {p1_wins}, P2: {p2_wins}, draws: {draws}")

    while True:
        print("\nNew game")
        board = Board()
        p1.clear_choices()
        while True:
            print("AI's turn")
            p1.make_play(board)
            board.print()
            if board.player_has_won(Ch.Cross):
                print("AI won!")
                break
            print("Your turn (0..8)")
            if board.is_draw():
                print("Draw!")
                break
            possible_choices = board.possible_plays()
            should_restart = False
            while True:
                text = input("> ")
                if text == ".save":
                    p1.save_to_file()
                    continue
                elif text == ".load":
                    p1.load_from_file()
                    choice = 0
                    should_restart = True
                    break
                elif text == ".restart":
                    choice = 0
                    should_restart = True
                    break
                elif text == "":
                    continue
                choice = int(text)
                if choice not in possible_choices:
                    print("invalid choice")
                else:
                    break
            if should_restart:
                break
            board.play(Ch.Circle, choice)
            board.print()
            if board.player_has_won(Ch.Circle):
                print("Player won!")
                break
        games += 1


try:
    main()
except KeyboardInterrupt:
    pass

