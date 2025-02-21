from __future__ import annotations
import numpy as np
import random as ra

# weight = inputs[in] * weights[in][out]

Arr = np.typing.NDArray[np.float64]

def sigmoid(x: Arr) -> Arr:
    return 1 / (1 + np.exp(-x))

class Model:
    def __init__(self, layer_sizes: list[int], biases: list[Arr], weights: list[Arr]) -> None:
        self.layer_sizes = layer_sizes
        self.biases = biases
        self.weights = weights

    def run(self, inputs: Arr) -> Arr:
        outputs = inputs

        for layer_idx in range(len(self.layer_sizes) - 1):
            products = outputs * self.weights[layer_idx].transpose()
            values = products + self.biases[layer_idx]
            sums = values.sum(axis=1)
            normalized = sigmoid(sums)
            outputs = normalized

        return outputs

    def mutate(self) -> None:
        weight_mag = 1.0
        for layer_weights in self.weights:
            mutation = np.random.random(layer_weights.shape) * weight_mag - (weight_mag / 2)
            layer_weights += mutation

        bias_mag = 2.0
        for layer_bias in self.biases:
            mutation = np.random.random(layer_bias.shape) * bias_mag - (bias_mag / 2)
            layer_bias += mutation

    def clone(self) -> Model:
        return Model(
            self.layer_sizes,
            [bias.copy() for bias in self.biases],
            [weight.copy() for weight in self.weights],
        )


class ModelBuilder:
    def __init__(self, inputs: int, outputs: int) -> None:
        self.inputs = inputs
        self.outputs = outputs
        self.layers: list[int] = []

    def add_layer(self, n: int):
        self.layers.append(n)

    def build(self) -> Model:
        layers = [self.inputs, *self.layers, self.outputs]

        biases: list[Arr] = []
        for layer in layers:
            layer_biases = np.random.random(size=(layer))
            # layer_biases = np.zeros(shape=(layer))
            biases.append(layer_biases)

        weights: list[Arr] = []
        for i in range(len(layers) - 1):
            layer_weights = np.random.random(size=(layers[i], layers[i + 1]))
            weights.append(layer_weights)

        return Model(layers, biases, weights)

def input_layer(line: str) -> Arr:
    return np.array([1.0 if ch == "x" else 0.0 for ch in line])
