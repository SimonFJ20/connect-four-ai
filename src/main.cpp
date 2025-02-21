#include "board.hpp"
#include "console.hpp"
#include "deci_tree_ai.hpp"
#include "printer.hpp"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <format>
#include <iostream>
#include <ostream>
#include <print>
#include <utility>

using namespace connect_four;

enum class ControlFlow {
    Continue,
    Break,
};

class Program {
private:
    ConsolePrinter printer;

    DeciTreeAi ai1;
    DeciTreeAi ai2;

public:
    Program()
        : ai1(Tile::Red)
        , ai2(Tile::Blue)
    {
    }

    void run()
    {
        run_ais_against_each_other(printer);
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

    void run_ais_against_each_other(Printer& printer)
    {
        constexpr auto training_iters = 1'000'000;

        auto l = std::locale("en_DK.UTF-8");
        std::cout << std::format(
            l, "Training AIs for {:L} iterations...\n", training_iters);

        for (int i = 0; i < training_iters; ++i) {
            auto board = Board();

            ai1.new_game();
            ai2.new_game();

            while (true) {
                size_t col = ai1.next_move(board);
                board.insert(col, Tile::Red);

                if (handle_ai_traning_game_state(board, ai1, ai2)
                    == ControlFlow::Break)
                    break;

                col = ai2.next_move(board);
                board.insert(col, Tile::Blue);

                if (handle_ai_traning_game_state(board, ai2, ai1)
                    == ControlFlow::Break)
                    break;
            }
        }

        std::println("ai nr\tsize in entries\tsize in bytes");
        std::cout << std::format(
            l, "1\t{:L}\t\t{:L}\n", ai1.model_entries(), ai1.model_size());
        std::cout << std::format(
            l, "2\t{:L}\t\t{:L}\n", ai2.model_entries(), ai2.model_size());

        std::exit(0);

        for (;;) {
            auto board = Board();

            ai1.new_game();
            ai2.new_game();

            while (true) {
                board.print(printer);
                std::println("Red's turn");
                fgetc(stdin);

                size_t col = ai1.next_move(board);
                board.insert(col, Tile::Red);

                if (check_game_state_and_print(board) == ControlFlow::Break)
                    break;

                board.print(printer);
                std::println("Blue's turn");
                fgetc(stdin);

                col = ai2.next_move(board);
                board.insert(col, Tile::Blue);

                if (check_game_state_and_print(board) == ControlFlow::Break)
                    break;
            }
            std::println("Game finished.");
            board.print(printer);
            std::println("\n");
        }

        while (true) {
            auto board = Board();

            ai1.new_game();

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

    ControlFlow handle_ai_traning_game_state(
        Board& board, DeciTreeAi& turnee, DeciTreeAi& other)
    {
        auto state = board.game_state();
        if (state == color_win_state(turnee.color())) {
            turnee.report_win();
            other.report_loss();
            return ControlFlow::Break;
        }
        if (state == color_lose_state(turnee.color())) {
            turnee.report_loss();
            other.report_win();
            return ControlFlow::Break;
        }
        if (state == GameState::Draw) {
            turnee.report_draw();
            other.report_draw();
            return ControlFlow::Break;
        }
        return ControlFlow::Continue;
    }
};

int main()
{
    std::srand(static_cast<uint32_t>(std::time(nullptr)));
    auto program = Program();
    program.run();
}
