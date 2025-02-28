from __future__ import annotations
from piece import CROSS, CIRCLE
from game import HumanPlayer, start_game, GameResult
from ai import train_ai, AiPlayer


def main():
    ai = train_ai()

    ai = AiPlayer(CROSS, ai)
    player = HumanPlayer(CIRCLE)

    while True:
        print("\nNew game")
        result = start_game(ai, player, quiet=False)
        match result:
            case GameResult.Player1Won:
                print("AI won!")
            case GameResult.Player2Won:
                print("Player won!")
            case GameResult.Draw:
                print("Draw!")


try:
    main()
except KeyboardInterrupt:
    pass
