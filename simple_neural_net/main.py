from __future__ import annotations
import random as ra
import math
import numpy as np

# from old_model import * 
from model import * 


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

# progbar_size = 50
# print("[" + " " * progbar_size +  "]   0%", end="", flush=True)

for i in range(100):

    # k = math.floor(i / (100 / progbar_size))
    # print("\b" * (progbar_size + 10) + "[" + "#" * k + " " * (progbar_size - k) + f"] {i:3.0f}%  ", end="", flush=True)

    for _ in range(math.floor(training_iterations / 100)):
        model = train_best_of_2(model, training_data)

# print("\b" * (progbar_size + 10) +"[" + "#" * progbar_size +  "] 100%")

print("Testing:")
test_data = make_data(20)
print("Line\tGuess\tCorrect\tGuess%\tCorrect%\tError")
for line, correct in test_data:
    guess = model.run(input_layer(line))
    error = abs(correct - clamp(float(guess), 0.0, 1.0))

    if error >= 0.5:
        print("\x1b[91m", end="")
    else:
        print("\x1b[92m", end="")

    print(f"{line}\t{"Yes" if guess >= 0.5 else "No"}\t{"Yes" if correct >= 0.5 else "No"}\t{guess:.2f}\t{correct:.2f}\t\t{error:.2f}\x1b[0m")

