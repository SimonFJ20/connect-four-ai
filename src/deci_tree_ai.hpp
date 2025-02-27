#ifndef DECITREE_AI_HPP
#define DECITREE_AI_HPP

#include "board.hpp"
#include "tile.hpp"
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace connect_four {

using Weight = int16_t;
[[maybe_unused]] static const constexpr Weight weight_max = INT16_MAX;
[[maybe_unused]] static const constexpr Weight weight_min = INT16_MIN;
using ColWeights = std::array<Weight, Board::width>;
using Choice = std::tuple<Board::Hash, Col>;

/// AI using decision tree strategy, like the one used for tic tac toe
class DeciTreeAi {
public:
    DeciTreeAi(Tile color)
        : m_color(color == Tile::Blue ? Color::Blue : Color::Red)
    {
    }

    auto next_move(const Board& board) -> size_t;

    void new_game();

    void report_win();
    void report_loss();
    void report_draw();

    auto color() const -> Color
    {
        return m_color;
    }

    auto model_entries() const -> size_t
    {
        return m_choice_weights.size();
    }

    auto model_size() const -> size_t
    {
        auto estimated_entry_size = sizeof(Board::Hash) + sizeof(ColWeights);
        return model_entries() * estimated_entry_size;
    }

    auto set_exploration(Weight exploration)
    {
        m_exploration = exploration;
    }

private:
    auto lookup_choices(Board board)
        -> std::tuple<Board::Hash, const ColWeights*>;
    auto choice_is_candidate(Weight weight, Weight cand_weight) const -> bool;

    void reward_punish_current_choices(Weight reward);

    std::unordered_map<Board::Hash, ColWeights> m_choice_weights {};

    std::vector<Choice> m_current_choices {};

    Color m_color;

    Weight m_exploration = 3;
};

}

#endif
