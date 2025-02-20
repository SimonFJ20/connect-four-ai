from __future__ import annotations
import random as ra
from math import exp, floor
import numpy as np
from numpy.typing import NDArray
import copy

# duplicate finder

def sigmoid(n: float) -> float:
    return 1 / (1 + exp(-n))

def clamp(val: float, min_: float, max_: float) -> float:
    return min(max(val, min_), max_);

M = NDArray

# weight = inputs[in] * weights[in][out]

class Model:
    def __init__(self, layer_sizes: list[int], weights: list[list[list[float]]]) -> None:
        self.layer_sizes = layer_sizes
        self.weights = weights

    def run(self, inputs: list[float]) -> list[float]:
        products = inputs

        for layer_idx in range(len(self.layer_sizes) - 1):
            input_size = self.layer_sizes[layer_idx]
            output_size = self.layer_sizes[layer_idx + 1]

            outputs = [0.0] * output_size

            for input_idx in range(input_size):
                for output_idx in range(output_size):
                    outputs[output_idx] += products[input_idx] * self.weights[layer_idx][input_idx][output_idx]

            products = outputs

        return products

    def clone(self) -> Model:
        return Model(copy.deepcopy(self.layer_sizes), copy.deepcopy(self.weights))

    def mutate(self):
        i = ra.randint(0, len(self.weights) - 1)
        j = ra.randint(0, len(self.weights[i]) - 1)
        k = ra.randint(0, len(self.weights[i][j]) - 1)

        self.weights[i][j][k] += ra.random() * 0.4 - 0.2


class ModelBuilder:
    def __init__(self, inputs: int, outputs: int) -> None:
        self.inputs = inputs
        self.outputs = outputs
        self.layers: list[int] = []

    def add_layer(self, n: int):
        self.layers.append(n)

    def build(self) -> Model:
        layers: list[int] = []
        layers.append(self.inputs)

        for layer in self.layers:
            layers.append(layer)
        layers.append(self.outputs)

        weights: list[list[list[float]]] = []
        for i in range(len(layers) - 1):
            layer_weights: list[list[float]] = []
            for _ in range(layers[i]):
                layer_input_weights: list[float] = []
                for _ in range(layers[i + 1]):
                    layer_input_weights.append(ra.random())
                layer_weights.append(layer_input_weights)
            weights.append(layer_weights)

        return Model(layers, weights)

def rand_line() -> str:
    line = []
    for _ in range(6):
        line.append("x" if ra.randint(0, 3) == 3 else "-")
    return "".join(line)

def has_dups(line: str) -> bool:
    return "xx" in line

DataEntry = tuple[str, float]
Data = list[DataEntry]

def make_data(n: int) -> Data:
    return [(line, 1.0 if has_dups(line) else 0.0)for line in [rand_line() for _ in range(n)]]

def input_layer(line: str) -> list[float]:
    return [1.0 if ch == "x" else 0.0 for ch in line]

def train_best_of_2(origo: Model, test_data: Data) -> Model:
    model1 = origo.clone()
    model2 = origo.clone()
    model2.mutate()

    ai_1_acc_err = 0
    ai_2_acc_err = 0
    for line, correct in test_data:
        ai_1_result = model1.run(input_layer(line))
        ai_2_result = model2.run(input_layer(line))

        ai_1_acc_err += pow(correct - ai_1_result[0], 2)
        ai_2_acc_err += pow(correct - ai_2_result[0], 2)

    ai_1_mse = ai_1_acc_err / len(test_data)
    ai_2_mse = ai_2_acc_err / len(test_data)

    ai_1_won = ai_1_mse < ai_2_mse

    if ai_1_won:
        return model1
    else:
        return model2

print("Training...")
training_data = make_data(100)

builder = ModelBuilder(6, 1)
builder.add_layer(12)
builder.add_layer(12)
model = builder.build()

training_iterations = 10000

progbar_size = 50
print("[" + " " * progbar_size +  "]   0%", end="", flush=True)

for i in range(100):

    k = floor(i / (100 / progbar_size))
    print("\b" * (progbar_size + 10) + "[" + "#" * k + " " * (progbar_size - k) + f"] {i:3.0f}%  ", end="", flush=True)

    for _ in range(floor(training_iterations / 100)):
        model = train_best_of_2(model, training_data)

print("\b" * (progbar_size + 10) +"[" + "#" * progbar_size +  "] 100%")

print("Testing:")
test_data = make_data(20)
print("Line\tGuess\tCorrect\tGuess%\tCorrect%\tError")
for line, correct in test_data:
    [guess] = model.run(input_layer(line))
    error = abs(correct - clamp(guess, 0.0, 1.0))

    if error >= 0.5:
        print("\x1b[91m", end="")
    else:
        print("\x1b[92m", end="")

    print(f"{line}\t{"Yes" if guess >= 0.5 else "No"}\t{"Yes" if correct >= 0.5 else "No"}\t{guess:.2f}\t{correct:.2f}\t\t{error:.2f}")

