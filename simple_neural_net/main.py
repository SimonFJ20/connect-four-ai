from __future__ import annotations
import random as ra
import math
import numpy as np

# from old_model import * 
from model import *
from progbar import Progbar
from plotter import ModelPlotter


# duplicate finder

def clamp(val: float, min_: float, max_: float) -> float:
    return min(max(val, min_), max_);


LINE_LEN = 6

def rand_line() -> str:
    line = []
    for _ in range(LINE_LEN):
        line.append("x" if ra.randint(0, 2) == 2 else "-")
    return "".join(line)

def has_dups(line: str) -> bool:
    return "xx" in line

DataEntry = tuple[str, float]
Data = list[DataEntry]

def make_data(n: int) -> Data:
    return [(line, 1.0 if has_dups(line) else 0.0) for line in [rand_line() for _ in range(n)]]

def train_best_of_2(origo: Model, test_data: Data, plotter: ModelPlotter) -> Model:
    model1 = origo.clone()
    model2 = origo.clone()
    model2.mutate()

    ai_1_acc_err = 0
    ai_2_acc_err = 0
    for line, correct in test_data:
        ai_1_result = model1.guess(input_layer(line))[0]
        ai_2_result = model2.guess(input_layer(line))[0]

        ai_1_acc_err += pow(correct - ai_1_result, 2)
        ai_2_acc_err += pow(correct - ai_2_result, 2)

    ai_1_mse = ai_1_acc_err / len(test_data)
    ai_2_mse = ai_2_acc_err / len(test_data)

    ai_1_won = ai_1_mse < ai_2_mse

    plotter.log_loss(min(ai_1_mse, ai_2_mse))

    if ai_1_won:
        return model1
    else:
        return model2

# builder = ModelBuilder(LINE_LEN, 1)
# builder.add_layer(12)
# builder.add_layer(12)
# model = builder.build()
#
# model.run(input_layer("--xx--"))
#
# exit(0)

print("Training...")
training_data = make_data(100)

builder = ModelBuilder(LINE_LEN, 1)
builder.add_layer(12)
model = builder.build()

plotter = ModelPlotter()

training_iterations = 100

# bar = Progbar(100, hidden=False)
# bar.print_initial()
# for i in range(100):
#     bar.print_iter(i / 100);
#     for _ in range(math.floor(training_iterations / 100)):
#         model = train_best_of_2(model, training_data, plotter)
#
# bar.print_finished();

bar = Progbar(100, hidden=False)
bar.print_initial()
for i in range(100):
    bar.print_iter(i / 100);
    for _ in range(math.ceil(training_iterations / 100)):
        data = [
            (input_layer(line), np.array([correct], dtype=np.float64))
                for line, correct in training_data
        ]
        mse = model.train(data)
        plotter.log_loss(mse)

bar.print_finished();

print("Testing:")
test_data = make_data(10_000)
print("Line\tGuess\tCorrect\tGuess%\tCorrect%\tError")
acc_err = 0
fails = 0
for i, [line, correct] in enumerate(test_data):
    guess = model.guess(input_layer(line))[0]
    error = abs(correct - clamp(float(guess), 0.0, 1.0))
    acc_err += error

    if error >= 0.5:
        fails += 1

    if i >= 30:
        continue

    if error >= 0.5:
        print("\x1b[91m", end="")
    else:
        print("\x1b[92m", end="")

    print(f"{line}\t{"Yes" if guess >= 0.5 else "No"}\t{"Yes" if correct >= 0.5 else "No"}\t{guess:.2f}\t{correct:.2f}\t\t{error:.2f}\x1b[0m")

mean_err = acc_err / len(test_data)
fail_rate = fails / len(test_data)
print(f"total:\t{len(test_data)}")
print(f"fails:\t{fails}")
print(f"fail rate: {fail_rate:.5f}")
print(f"mean error: {mean_err:.5f}")

plotter.show_loss_curve()

