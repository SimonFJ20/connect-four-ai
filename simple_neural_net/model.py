from __future__ import annotations
import numpy as np
import random as ra

# weight = inputs[in] * weights[in][out]

Arr = np.typing.NDArray[np.float64]

def sigmoid(x: Arr) -> Arr:
    return 1 / (1 + np.exp(-x))

def sigmoid_deriv(x: Arr) -> Arr:
    return x * (1 - x)

class Model:
    def __init__(self, layer_sizes: list[int], biases: list[Arr], weights: list[Arr]) -> None:
        self.layer_sizes = layer_sizes
        self.biases = biases
        self.weights = weights

        self.learning_rate = 0.1
        self.values_means: list[Arr] = [np.zeros(shape=(size,)) for size in self.layer_sizes[1:]]
        self.runs = 0

    def guess(self, inputs: Arr) -> Arr:
        outputs = inputs

        for layer_idx in range(len(self.layer_sizes) - 1):
            l0 = outputs
            l1 = l0 * self.weights[layer_idx].T
            l2 = l1.T + self.biases[layer_idx]
            l3 = l2.T.sum(axis=1)
            l4 = sigmoid(l3)

            self.values_means[layer_idx] += l4 / (self.runs + 1)
            outputs = l4

        self.runs += 1
        return outputs

    def train(self, data: list[tuple[Arr, Arr]]) -> float:
        for arr in self.values_means:
            arr *= 0
        self.runs = 0

        error_acc = np.zeros(shape=(1,), dtype=np.float64)
        square_error_acc = np.zeros(shape=(1,), dtype=np.float64)

        for input, correct in data:
            guess = self.guess(input) # -> self.layer_values
            error = correct - guess
            error_acc += error
            square_error_acc += np.square(error)

        mean_error = error_acc / len(data)
        mean_square_error = square_error_acc / len(data)

        self.backpropagate(mean_square_error)
        return mean_square_error.item()
        
    def backpropagate(self, loss: Arr) -> None:
        self.backpropagate_layer(loss, len(self.layer_sizes) - 2)

    def backpropagate_layer(self, loss: Arr, layer_idx: int) -> None:
        delta = loss * sigmoid_deriv(self.values_means[layer_idx])
        if layer_idx > 0:
            next_error = np.dot(delta, self.weights[layer_idx].T)
            self.backpropagate_layer(next_error, layer_idx - 1)
        self.weights[layer_idx] += np.dot(self.values_means[layer_idx].T, delta) * self.learning_rate
        self.biases[layer_idx] += np.sum(delta, axis=0, keepdims=True) * self.learning_rate

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
        for i in range(len(layers) - 1):
            layer_biases = np.random.random(size=(layers[i + 1]))
            # layer_biases = np.zeros(shape=(layers[i + 1]))
            biases.append(layer_biases)

        weights: list[Arr] = []
        for i in range(len(layers) - 1):
            layer_weights = np.random.random(size=(layers[i], layers[i + 1]))
            # layer_weights = np.zeros(shape=(layers[i], layers[i + 1]))
            weights.append(layer_weights)

        return Model(layers, biases, weights)

def input_layer(line: str) -> Arr:
    return np.array([1.0 if ch == "x" else 0.0 for ch in line])
