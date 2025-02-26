#include "board.hpp"
#include "console.hpp"
#include "deci_tree_ai.hpp"
#include "nn_model.hpp"
#include "printer.hpp"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <format>
#include <iostream>
#include <print>
#include <utility>
#include <vector>

using namespace connect_four;

enum class ControlFlow {
    Continue,
    Break,
};

class Program {
private:
    ConsolePrinter m_printer;

    DeciTreeAi ai1;
    DeciTreeAi ai2;

    Model m_model;

public:
    Program()
        : ai1(Tile::Red)
        , ai2(Tile::Blue)
        , m_model({ 42, 42, 18, 6 })
    {
    }

    void run()
    {
        run_ais_against_each_other();
        // run_nnmodel_against_user();
        // run_nn_models_against_each_other();
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
                board.print(m_printer);
                std::println("\x1b[1;91mRed won!\x1b[0m");
                return ControlFlow::Break;
            case GameState::BlueWon:
                board.print(m_printer);
                std::println("\x1b[1;94mBlue won!\x1b[0m");
                return ControlFlow::Break;
            case GameState::Draw:
                board.print(m_printer);
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

    void run_pvp()
    {

        auto board = Board();

        while (true) {
            std::println("\x1b[1;91mRed\x1b[0m's turn");
            board.print(m_printer);
            size_t col;
            col = get_move_from_user(board);
            board.insert(col, Tile::Red);

            if (check_game_state_and_print(board) == ControlFlow::Break)
                break;

            std::println("\x1b[1;94mBlue\x1b[0m's turn");
            board.print(m_printer);
            col = get_move_from_user(board);
            board.insert(col, Tile::Blue);

            if (check_game_state_and_print(board) == ControlFlow::Break)
                break;
        }
    }

    void run_nnmodel_against_user()
    {
        while (true) {
            auto board = Board();

            ai1.new_game();

            while (true) {
                std::println();
                std::println("AI's turn");
                board.print(m_printer);

                size_t model_col = model_select_col(board, m_model);
                board.insert(model_col, Tile::Red);
                // model.mutate();

                if (check_game_state_and_print(board) == ControlFlow::Break)
                    break;

                std::println();
                std::println("Your turn");
                board.print(m_printer);

                auto col = get_move_from_user(board);
                board.insert(col, Tile::Blue);

                if (check_game_state_and_print(board) == ControlFlow::Break)
                    break;
            }
        }
    }

    size_t model_select_col(Board& board, Model& model)
    {

        auto inputs = board.as_mx1();
        auto outputs = model.feed(inputs);

        auto possible_moves = board.possible_moves();

        size_t selected_col = 0;
        double max = 0;
        for (size_t col = 0; col < 7; ++col) {
            if (outputs[col] > max && possible_moves.at(col)) {
                max = outputs[col];
                selected_col = col;
            }
        }
        return selected_col;
    }

    void run_nn_models_against_each_other()
    {
        constexpr auto training_iters = 1000'000;

        auto l = std::locale("en_DK.UTF-8");
        std::cout << std::format(
            l, "Training AIs for {:L} iterations...\n", training_iters);

        auto clone = m_model;
        clone.mutate();

        for (int i = 0; i < training_iters; ++i) {
            auto board = Board();

            while (true) {
                size_t col = model_select_col(board, m_model);
                board.insert(col, Tile::Red);
                // board.print(m_printer);

                auto should_break = false;
                switch (board.game_state()) {
                    case GameState::RedWon:
                        clone = m_model;
                        clone.mutate();
                        should_break = true;
                        // std::cout << "RED WON!!!\n";
                        break;
                    case GameState::BlueWon:
                        m_model = clone;
                        clone.mutate();
                        should_break = true;
                        // std::cout << "BLUE WON!!!\n";
                        break;
                    case GameState::Draw:
                        clone.mutate();
                        should_break = true;
                        break;
                    case GameState::Ongoing:
                        break;
                }
                if (should_break) {
                    // board.print(m_printer);
                    break;
                }

                col = model_select_col(board, clone);
                board.insert(col, Tile::Blue);
                // board.print(m_printer);

                should_break = false;
                switch (board.game_state()) {
                    case GameState::RedWon:
                        m_model = clone;
                        clone.mutate();
                        should_break = true;
                        // std::cout << "RED WON!!!\n";
                        break;
                    case GameState::BlueWon:
                        clone = m_model;
                        clone.mutate();
                        should_break = true;
                        // std::cout << "BLUE WON!!!\n";
                        break;
                    case GameState::Draw:
                        clone.mutate();
                        should_break = true;
                        break;
                    case GameState::Ongoing:
                        break;
                }
                if (should_break) {
                    // board.print(m_printer);
                    break;
                }
            }
            std::println("{:3.1f}%", (double)(i + 1) / training_iters * 100);
        }

        std::println("done");

        for (;;) {
            auto board = Board();

            while (true) {
                board.print(m_printer);
                std::println("Red's turn");
                fgetc(stdin);

                size_t col = model_select_col(board, m_model);
                board.insert(col, Tile::Red);

                if (check_game_state_and_print_2(board) == ControlFlow::Break)
                    break;

                board.print(m_printer);
                std::println("Blue's turn");
                fgetc(stdin);

                col = model_select_col(board, clone);
                board.insert(col, Tile::Blue);

                if (check_game_state_and_print_2(board) == ControlFlow::Break)
                    break;
            }
            std::println("Game finished.");
            board.print(m_printer);
            std::println("\n");
        }
    }

    ControlFlow check_game_state_and_print_2(Board& board)
    {
        switch (board.game_state()) {
            case GameState::RedWon:
                board.print(m_printer);
                std::println("\x1b[1;91mRed won!\x1b[0m");
                return ControlFlow::Break;
            case GameState::BlueWon:
                board.print(m_printer);
                std::println("\x1b[1;94mBlue won!\x1b[0m");
                return ControlFlow::Break;
            case GameState::Draw:
                board.print(m_printer);
                std::println("\x1b[1;95mDraw!\x1b[0m");
                return ControlFlow::Break;
            case GameState::Ongoing:
                return ControlFlow::Continue;
        }
        std::unreachable();
    }

    void run_ais_against_each_other()
    {
        constexpr auto training_iters = 10'000'000;

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

        // std::exit(0);

        // for (;;) {
        //     auto board = Board();
        //
        //     ai1.new_game();
        //     ai2.new_game();
        //
        //     while (true) {
        //         board.print(m_printer);
        //         std::println("Red's turn");
        //         fgetc(stdin);
        //
        //         size_t col = ai1.next_move(board);
        //         board.insert(col, Tile::Red);
        //
        //         if (check_game_state_and_print(board) == ControlFlow::Break)
        //             break;
        //
        //         board.print(m_printer);
        //         std::println("Blue's turn");
        //         fgetc(stdin);
        //
        //         col = ai2.next_move(board);
        //         board.insert(col, Tile::Blue);
        //
        //         if (check_game_state_and_print(board) == ControlFlow::Break)
        //             break;
        //     }
        //     std::println("Game finished.");
        //     board.print(m_printer);
        //     std::println("\n");
        // }

        ai1.set_exploration(0);

        while (true) {
            auto board = Board();

            ai1.new_game();

            while (true) {
                std::println();
                std::println("AI's turn");
                board.print(m_printer);

                size_t col = ai1.next_move(board);
                board.insert(col, Tile::Red);

                if (check_game_state_and_print(board) == ControlFlow::Break)
                    break;

                std::println();
                std::println("Your turn");
                board.print(m_printer);

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
