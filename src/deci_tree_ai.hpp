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

inline auto color_to_tile(Color color) -> Tile
{
    switch (color) {
        case Color::Red:
            return Tile::Red;
        case Color::Blue:
            return Tile::Blue;
    }
    std::unreachable();
}

using Weight = int;
using Choice = size_t;

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
        auto amount_of_element = model_entries();
        auto estimated_entry_size = sizeof(Choice) + sizeof(Weight);
        auto estimated_byte_size = amount_of_element * estimated_entry_size;
        return estimated_byte_size;
    }

private:
    auto lookup_choice(Board board) -> std::tuple<Choice, Weight>;
    auto choice_is_candidate(int weight, int cand_weight) const -> bool;

    void reward_punish_current_choices(int reward);

    std::unordered_map<size_t, Weight> m_choice_weights {};

    std::vector<size_t> m_current_choices {};

    Color m_color;
};

}

#endif
