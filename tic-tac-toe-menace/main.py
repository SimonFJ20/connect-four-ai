from piece import CROSS, CIRCLE
from game import Human, start_game, GameResult
from ai import DTModel, train_dt_model


def main():
    model = DTModel(CROSS)
    train_dt_model(model, iterations=100_000)

    player = Human(CIRCLE)

    while True:
        print("\nNew game started")

        result = start_game(p1=model, p2=player, quiet=False)

        match result:
            case GameResult.P1Won:
                print("AI won!")
            case GameResult.P2Won:
                print("Player won!")
            case GameResult.Draw:
                print("Draw!")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
