#include "board.hpp"
#include "console.hpp"
#include "printer.hpp"
#include <format>
#include <iostream>
#include <utility>

using namespace connect_four;

enum class ControlFlow {
    Continue,
    Break,
};

ControlFlow check_game_state(Board& board, Printer& printer)
{
    switch (board.game_state()) {
        case GameState::RedWon:
            board.print(printer);
            std::println("\x1b[1;91mRed won!\x1b[0m");
            return ControlFlow::Break;
        case GameState::BlueWon:
            board.print(printer);
            std::println("\x1b[1;94mBlue won!\x1b[0m");
            return ControlFlow::Break;
        case GameState::Draw:
            board.print(printer);
            std::println("\x1b[1;95mDraw!\x1b[0m");
            return ControlFlow::Break;
        case GameState::Ongoing:
            return ControlFlow::Continue;
    }
    std::unreachable();
}

size_t get_move_from_user(Board& board)
{
    std::cout << std::format("Enter move (0..7)\n> ");
    while (true) {
        size_t v;
        std::cin >> v;

        auto possible_moves = board.possible_moves();
        if ((possible_moves >> v & 1) == 0) {
            std::cout << std::format("invalid input\n> ");
            continue;
        }

        return v;
    }
}

void run_pvp(Printer& printer)
{

    auto board = Board();

    while (true) {
        std::println("\x1b[1;91mRed\x1b[0m's turn");
        board.print(printer);
        size_t col;
        col = get_move_from_user(board);
        board.insert(col, Tile::Red);

        if (check_game_state(board, printer) == ControlFlow::Break)
            break;

        std::println("\x1b[1;94mBlue\x1b[0m's turn");
        board.print(printer);
        col = get_move_from_user(board);
        board.insert(col, Tile::Blue);

        if (check_game_state(board, printer) == ControlFlow::Break)
            break;
    }
}

int main()
{
    auto printer = ConsolePrinter();
    run_pvp(printer);
}
