from __future__ import annotations
import random as ra
import math
import numpy as np

# from old_model import * 
from model import *
from progbar import Progbar


# duplicate finder

def clamp(val: float, min_: float, max_: float) -> float:
    return min(max(val, min_), max_);


LINE_LEN = 6

def rand_line() -> str:
    line = []
    for _ in range(LINE_LEN):
        line.append("x" if ra.randint(0, 3) == 3 else "-")
    return "".join(line)

def has_dups(line: str) -> bool:
    return "xx" in line

DataEntry = tuple[str, float]
Data = list[DataEntry]

def make_data(n: int) -> Data:
    return [(line, 1.0 if has_dups(line) else 0.0) for line in [rand_line() for _ in range(n)]]

def train_best_of_2(origo: Model, test_data: Data) -> Model:
    model1 = origo.clone()
    model2 = origo.clone()
    model2.mutate()

    ai_1_acc_err = 0
    ai_2_acc_err = 0
    for line, correct in test_data:
        ai_1_result = model1.run(input_layer(line))
        ai_2_result = model2.run(input_layer(line))

        ai_1_acc_err += pow(correct - ai_1_result, 2)
        ai_2_acc_err += pow(correct - ai_2_result, 2)

    ai_1_mse = ai_1_acc_err / len(test_data)
    ai_2_mse = ai_2_acc_err / len(test_data)

    ai_1_won = ai_1_mse < ai_2_mse

    if ai_1_won:
        return model1
    else:
        return model2

print("Training...")
training_data = make_data(100)

builder = ModelBuilder(LINE_LEN, 1)
builder.add_layer(12)
builder.add_layer(12)
model = builder.build()

training_iterations = 1000

bar = Progbar(100)
bar.print_initial()
for i in range(100):
    bar.print_iter(i / 100);
    for _ in range(math.floor(training_iterations / 100)):
        model = train_best_of_2(model, training_data)

bar.print_finished();

print("Testing:")
test_data = make_data(30)
print("Line\tGuess\tCorrect\tGuess%\tCorrect%\tError")
acc_err = 0
fails = 0
for line, correct in test_data:
    guess = model.run(input_layer(line))
    error = abs(correct - clamp(float(guess), 0.0, 1.0))
    acc_err += error

    if error >= 0.5:
        print("\x1b[91m", end="")
        fails += 1
    else:
        print("\x1b[92m", end="")

    print(f"{line}\t{"Yes" if guess >= 0.5 else "No"}\t{"Yes" if correct >= 0.5 else "No"}\t{guess:.2f}\t{correct:.2f}\t\t{error:.2f}\x1b[0m")

mean_err = acc_err / len(test_data)
fail_rate = fails / len(test_data)
print(f"total:\t{len(test_data)}")
print(f"fails:\t{fails}")
print(f"fail rate: {fail_rate:.2f}")
print(f"mean error: {mean_err:.2f}")

