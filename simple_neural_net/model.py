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
            # print("outputs:\n", outputs)
            # print("self.weights[layer_idx]:\n", self.weights[layer_idx])
            # print("self.weights[layer_idx].transpose():\n", self.weights[layer_idx].transpose())
            products = outputs * self.weights[layer_idx].transpose()
            # print("products:\n", products)
            # print("self.biases[layer_idx]:\n", self.biases[layer_idx])
            values = products + self.biases[layer_idx]
            # print("values:\n", values)
            sums = values.sum(axis=1)
            # print("sums:\n", sums)
            normalized = sigmoid(sums)
            # print("normalized:\n", normalized)
            outputs = normalized
            # exit(0)

        return outputs

    def mutate(self) -> None:
        # for layer_weights in self.weights:
        #     mutation = (np.random.random(layer_weights.shape) - 0.5) * 2
        #     sign = mutation / abs(mutation);
        #     mutation = pow(mutation, 2) * sign
        #     layer_weights += mutation

        weight_mag = 1.0
        bias_mag = 1.0

        for layer_weights in self.weights:
            mutation = np.random.random(layer_weights.shape) * weight_mag - (weight_mag / 2)
            layer_weights += mutation

        for layer_bias in self.biases:
            mutation = np.random.random(layer_bias.shape) * bias_mag - (bias_mag / 2)
            layer_bias += mutation


        # i = ra.randint(0, len(self.weights) - 1)
        # (inputs, outputs) = self.weights[i].shape
        #
        # j = ra.randint(0, inputs - 1)
        # k = ra.randint(0, outputs - 1)
        # self.weights[i][j, k] += ra.random() * weight_mag - (weight_mag / 2)
        #
        # l = ra.randint(0, inputs - 1)
        # self.biases[i][l] += ra.random() * bias_mag - (bias_mag / 2)

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
