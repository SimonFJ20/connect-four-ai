import matplotlib.pyplot as plt

class ModelPlotter:
    def __init__(self) -> None:
        self.loss_entries = []

    def log_loss(self, loss: float) -> None:
        self.loss_entries.append(loss)

    def show_loss_curve(self) -> None:
        fig, ax = plt.subplots()

        ax.plot(range(len(self.loss_entries)), self.loss_entries, label="Loss")
        ax.set(xlabel="Iterations", ylabel="Loss", title="Loss over time", yscale="log")
        ax.grid()

        plt.show()


