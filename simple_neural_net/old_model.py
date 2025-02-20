from __future__ import annotations
import copy
import random as ra

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
                    outputs[output_idx] += (
                        products[input_idx]
                        * self.weights[layer_idx][input_idx][output_idx])

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

def input_layer(line: str) -> list[float]:
    return [1.0 if ch == "x" else 0.0 for ch in line]
