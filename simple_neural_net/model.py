from __future__ import annotations
import numpy as np
import random as ra

# weight = inputs[in] * weights[in][out]

Arr = np.typing.NDArray[np.float64]

def sigmoid(x: Arr) -> Arr:
    return 1 / (1 + np.exp(-x))

class Model:
    def __init__(self, layer_sizes: list[int], weights: list[Arr]) -> None:
        self.layer_sizes = layer_sizes
        self.weights = weights

    def run(self, inputs: Arr) -> Arr:
        outputs = inputs

        for layer_idx in range(len(self.layer_sizes) - 1):
            sum = (outputs * self.weights[layer_idx].transpose()).sum()
            # outputs = 1 / (1 - np.exp(sum))
            outputs = sum

        return outputs

    def mutate(self) -> None:
        # for layer_weights in self.weights:
        #     mutation = (np.random.random(layer_weights.shape) - 0.5) * 2
        #     sign = mutation / abs(mutation);
        #     mutation = pow(mutation, 2) * sign
        #     layer_weights += mutation

        # for layer_weights in self.weights:
        #     mutation = np.random.random(layer_weights.shape) * 0.4 - 0.2
        #     layer_weights += mutation

        i = ra.randint(0, len(self.weights) - 1)
        (inputs, outputs) = self.weights[i].shape

        j = ra.randint(0, inputs - 1)
        k = ra.randint(0, outputs - 1)

        self.weights[i][j, k] += ra.random() * 0.4 - 0.2

    def clone(self) -> Model:
        return Model(self.layer_sizes, [w.copy() for w in self.weights])


class ModelBuilder:
    def __init__(self, inputs: int, outputs: int) -> None:
        self.inputs = inputs
        self.outputs = outputs
        self.layers: list[int] = []

    def add_layer(self, n: int):
        self.layers.append(n)

    def build(self) -> Model:
        layers = [self.inputs, *self.layers, self.outputs]

        weights: list[Arr] = []
        for i in range(len(layers) - 1):
            layer_weights = np.random.random(size=(layers[i], layers[i + 1]))
            weights.append(layer_weights)

        return Model(layers, weights)

def input_layer(line: str) -> Arr:
    return np.array([1.0 if ch == "x" else 0.0 for ch in line])
