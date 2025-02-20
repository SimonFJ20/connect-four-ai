import math

class Progbar:
    def __init__(self, size: int) -> None:
        self.size = size

    def print_initial(self) -> None:
        print("[" + " " * self.size +  "]   0%", end="", flush=True)

    def print_iter(self, progress: float) -> None:
        k = math.floor(progress * self.size)
        print("\b" * (self.size + 10) + "[" + "#" * k + " " * (self.size - k) + f"] {progress*100:3.0f}%  ", end="", flush=True)

    def print_finished(self) -> None:
        print("\b" * (self.size + 10) +"[" + "#" * self.size +  "] 100%")


