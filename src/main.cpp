#include "board.hpp"
#include "console.hpp"
#include "deci_tree_ai.hpp"
#include "minimax.hpp"
#include "nn_model.hpp"
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

    DeciTreeAi m_bot1;
    DeciTreeAi m_bot2;

    Model m_model;

public:
    Program()
        : m_bot1(Tile::Red)
        , m_bot2(Tile::Blue)
        , m_model({ 42, 42, 18, 6 })
    {
    }

    void run()
    {
        run_ais_against_each_other();
        // run_nnmodel_against_user();
        // run_nn_models_against_each_other();

        // auto board = Board();
        // auto minimax_red = Minimax(Color::Red);
        // auto minimax_blue = Minimax(Color::Blue);
        //
        // auto current_color = Color::Red;
        // auto* current = &minimax_red;
        // auto* other = &minimax_blue;
        // while (board.game_state() == GameState::Ongoing) {
        //     Col col = current->choose(board, 7);
        //     board.insert(col, color_to_tile(current_color));
        //     board.print(m_printer);
        //
        //     auto* temp = current;
        //     current = other;
        //     other = temp;
        //     current_color = color_opposite(current_color);
        // }
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

            m_bot1.new_game();

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
        constexpr auto training_iters = 100'000;

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
            if (i % (training_iters / 400) == 0) {
                std::println(
                    "{:3.1f}%", (double)(i + 1) / training_iters * 100);
            }
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

    using Wins = std::unordered_map<Color, int>;

    void run_ais_against_each_other()
    {
        constexpr auto training_iters = 1'000'000;

        auto l = std::locale("en_DK.UTF-8");
        std::cout << std::format(
            l, "Training AIs for {:L} iterations...\n", training_iters);

        auto bot1 = DeciTreeAi(Tile::Red);
        auto bot2 = DeciTreeAi(Tile::Blue);

        auto wins = Wins {
            { bot1.color(), 0 },
            { bot2.color(), 0 },
        };

        for (int i = 0; i < training_iters; ++i) {
            auto board = Board();

            bot1.new_game();
            bot2.new_game();

            auto* current = &bot1;
            auto* other = &bot2;
            while (true) {
                size_t col = current->next_move(board);
                board.insert(col, current->tile());

                if (handle_ai_traning_game_state(board, *current, *other, wins)
                    == ControlFlow::Break)
                    break;

                std::swap(current, other);
            }
        }

        std::println("color\t   entries\t  est. bytes\t   wins");
        std::cout << std::format(l, "{}\t{:10L}\t{:12L}\t{:7}\n",
            bot1.color() == Color::Red ? "  Red" : " Blue",
            bot1.model_entries(), bot1.model_size(), wins.at(bot1.color()));
        std::cout << std::format(l, "{}\t{:10L}\t{:12L}\t{:7}\n",
            bot2.color() == Color::Red ? "  Red" : " Blue",
            bot2.model_entries(), bot2.model_size(), wins.at(bot1.color()));

        bot1.set_exploration(0);
        bot2.set_exploration(0);

        // for (;;) {
        //     auto board = Board();
        //
        //     while (true) {
        //         board.print(m_printer);
        //         std::println("Red's turn");
        //         fgetc(stdin);
        //
        //         size_t col = bot1.next_move(board);
        //         board.insert(col, Tile::Red);
        //
        //         if (check_game_state_and_print(board) == ControlFlow::Break)
        //             break;
        //
        //         board.print(m_printer);
        //         std::println("Blue's turn");
        //         fgetc(stdin);
        //
        //         col = bot1.next_move(board);
        //         board.insert(col, Tile::Blue);
        //
        //         if (check_game_state_and_print(board) == ControlFlow::Break)
        //             break;
        //     }
        //     std::println("Game finished.");
        //     board.print(m_printer);
        //     std::println("\n");
        // }

        while (true) {
            auto board = Board();

            bot1.new_game();

            while (true) {
                std::println();
                std::println("AI's turn");
                board.print(m_printer);

                size_t col = bot1.next_move(board);
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
        Board& board, DeciTreeAi& turnee, DeciTreeAi& other, Wins& wins)
    {
        auto state = board.game_state();
        if (state == color_win_state(turnee.color())) {
            turnee.report_win();
            other.report_loss();
            wins.at(turnee.color()) += 1;
            return ControlFlow::Break;
        }
        if (state == color_lose_state(turnee.color())) {
            turnee.report_loss();
            other.report_win();
            wins.at(other.color()) += 1;
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
