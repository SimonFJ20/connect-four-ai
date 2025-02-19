#include "board.hpp"
#include "console.hpp"

using namespace connect_four;

int main()
{
    auto printer = ConsolePrinter();

    auto board = Board();
    // board.print(printer);

    board.insert(0, Tile::Red);
    board.insert(0, Tile::Red);
    board.insert(0, Tile::Red);
    board.insert(0, Tile::Red);
    board.insert(0, Tile::Red);
    board.insert(0, Tile::Red);
    board.insert(3, Tile::Red);
    board.insert(3, Tile::Blue);
    board.print(printer);
}
