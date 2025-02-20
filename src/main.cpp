#include "board.hpp"
#include "console.hpp"
#include "deci_tree_ai.hpp"
#include "printer.hpp"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <format>
#include <iostream>
#include <utility>

using namespace connect_four;

enum class ControlFlow {
    Continue,
    Break,
};

class Program {
private:
    ConsolePrinter printer;

public:
    void run()
    {
        run_ai(printer);
    }

private:
    ControlFlow check_game_state(Board& board)
    {
        switch (board.game_state()) {
            case GameState::RedWon:
                return ControlFlow::Break;
            case GameState::BlueWon:
                return ControlFlow::Break;
            case GameState::Draw:
                return ControlFlow::Break;
            case GameState::Ongoing:
                return ControlFlow::Continue;
        }
        std::unreachable();
    }

    ControlFlow check_game_state_and_print(Board& board)
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
            if (!possible_moves.at(v)) {
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

            if (check_game_state_and_print(board) == ControlFlow::Break)
                break;

            std::println("\x1b[1;94mBlue\x1b[0m's turn");
            board.print(printer);
            col = get_move_from_user(board);
            board.insert(col, Tile::Blue);

            if (check_game_state_and_print(board) == ControlFlow::Break)
                break;
        }
    }

    void run_ai(Printer& printer)
    {
        auto ai1 = DeciTreeAi(Tile::Red);
        auto ai2 = DeciTreeAi(Tile::Blue);

        std::println("Training AIs...");

        for (int i = 0; i < 10; ++i) {
            auto board = Board();

            while (true) {
                size_t col = ai1.next_move(board);
                board.insert(col, Tile::Red);

                if (check_game_state(board) == ControlFlow::Break)
                    break;

                col = ai2.next_move(board);
                board.insert(col, Tile::Blue);

                if (check_game_state(board) == ControlFlow::Break)
                    break;
            }
        }

        while (true) {
            auto board = Board();

            while (true) {
                std::println();
                std::println("AI's turn");
                board.print(printer);

                size_t col = ai1.next_move(board);
                board.insert(col, Tile::Red);

                if (check_game_state_and_print(board) == ControlFlow::Break)
                    break;

                std::println();
                std::println("Your turn");
                board.print(printer);

                col = get_move_from_user(board);
                board.insert(col, Tile::Blue);

                if (check_game_state_and_print(board) == ControlFlow::Break)
                    break;
            }
        }
    }
};

int main()
{
    std::srand(static_cast<uint32_t>(std::time(nullptr)));
    auto program = Program();
    program.run();
}
