#include "deci_tree_ai.hpp"
#include "board.hpp"
#include <cstdint>
#include <cstdlib>
#include <iostream>

using namespace connect_four;

auto DeciTreeAi::next_move(const Board& board) -> size_t
{
    auto possible_moves = board.possible_moves();

    int cand_weight = 0;
    auto candidates = std::array<std::tuple<Choice, Col>, 8>();
    size_t cand_size = 0;

    for (uint8_t col = 0; col < board.width; ++col) {
        if (!possible_moves.at(col))
            continue;

        auto possible_board = board;
        possible_board.insert(col, color_to_tile(m_color));
        auto [choice, weight] = lookup_choice(possible_board);
        if (weight > cand_weight) {
            cand_weight = weight;
            cand_size = 0;
        }
        if (choice_is_candidate(weight, cand_weight)) {
            candidates[cand_size] = { choice, col };
            cand_size += 1;
        }
    }
    if (cand_size == 0) {
        std::cerr << "no candidates\n";
        std::exit(EXIT_FAILURE);
    }
    auto cand_idx = cand_size > 1
        ? static_cast<size_t>(std::rand()) % (cand_size - 1)
        : 0;
    auto [choice, col] = candidates[cand_idx];
    m_current_choices.push_back(choice);
    return col;
}

void report_move(Col col)
{
    (void)col;
}

auto DeciTreeAi::lookup_choice(Board board) -> std::tuple<Choice, Weight>
{
    auto choice = board.hash();
    if (m_choice_weights.contains(choice))
        return { choice, m_choice_weights.at(choice) };

    auto flipped = board.flipped_hash();
    if (m_choice_weights.contains(flipped))
        return { choice, m_choice_weights.at(flipped) };

    m_choice_weights.insert_or_assign(choice, 0);
    return { choice, 0 };
}

auto DeciTreeAi::choice_is_candidate(int weight, int cand_weight) const -> bool
{
    return weight + 2 >= cand_weight;
}

void DeciTreeAi::report_win()
{
    reward_punish_current_choices(2);
}

void DeciTreeAi::report_loss()
{
    reward_punish_current_choices(-2);
}

void DeciTreeAi::report_draw()
{
    reward_punish_current_choices(std::rand() % 5 - 2);
}

void DeciTreeAi::reward_punish_current_choices(int reward)
{
    for (auto choice : m_current_choices) {
        m_choice_weights.at(choice) += reward;
    }
}
