#include "deci_tree_ai.hpp"
#include "board.hpp"
#include <cstdint>
#include <cstdlib>
#include <iostream>

using namespace connect_four;

auto DeciTreeAi::next_move(const Board& board) -> size_t
{
    auto [hash, weights] = lookup_choices(board);
    auto possible_moves = board.possible_moves();

    int cand_weight = INT32_MIN;
    auto candidates = std::array<Col, Board::width>();
    size_t cand_size = 0;

    for (uint8_t col = 0; col < board.width; ++col) {
        if (!possible_moves.at(col))
            continue;

        int weight = weights->at(col);
        if (weight > cand_weight) {
            cand_weight = weight;
            cand_size = 0;
        }
        if (choice_is_candidate(weight, cand_weight)) {
            candidates[cand_size] = col;
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
    auto col = candidates[cand_idx];
    m_current_choices.push_back({ hash, col });
    return col;
}

auto DeciTreeAi::lookup_choices(Board board)
    -> std::tuple<Board::Hash, const ColWeights*>
{
    auto hash = board.hash();
    if (m_choice_weights.contains(hash))
        return { hash, &m_choice_weights.at(hash) };

    auto flipped = board.flipped_hash();
    if (m_choice_weights.contains(flipped))
        return { flipped, &m_choice_weights.at(flipped) };

    m_choice_weights.insert_or_assign(hash, ColWeights { 0 });
    return { hash, &m_choice_weights.at(hash) };
}

auto DeciTreeAi::choice_is_candidate(int weight, int cand_weight) const -> bool
{
    return weight + 2 >= cand_weight;
}

void DeciTreeAi::new_game()
{
    m_current_choices.clear();
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
    for (auto [hash, col] : m_current_choices) {
        m_choice_weights.at(hash).at(col) += reward;
    }
}
