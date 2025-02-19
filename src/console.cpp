#include "console.hpp"
#include <iostream>

using namespace connect_four;

void ConsolePrinter::print_board(
    std::span<Tile> board, size_t width, size_t height)
{
    for (size_t y = 0; y < height; ++y) {
        std::cout << "\x1b[0;90m| ";
        for (size_t x = 0; x < width; ++x) {

            switch (board[y * width + x]) {
                case Tile::Empty:
                    std::cout << " ";
                    break;
                case Tile::Red:
                    std::cout << "\x1b[1;91mO";
                    break;
                case Tile::Blue:
                    std::cout << "\x1b[1;94mO";
                    break;
            }

            std::cout << "\x1b[0;90m | ";
        }

        std::cout << "\n";
    }
    std::cout << "\x1b[0;90m" << std::string(width * 4 + 1, '#') << "\x1b[0m\n";
}
