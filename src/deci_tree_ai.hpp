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

enum class Color : uint8_t {
    Red,
    Blue,
};

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
    void report_move(Col col);

    inline void incr_depth()
    {
        m_depth += 1;
    }

    void report_win();
    void report_loss();
    void report_draw();

private:
    auto lookup_choice(Board board) -> std::tuple<Choice, Weight>;
    auto choice_is_candidate(int weight, int cand_weight) const -> bool;

    void reward_punish_current_choices(int reward);

    std::unordered_map<size_t, Weight> m_choice_weights {};

    std::vector<size_t> m_current_choices {};
    uint8_t m_depth = 0;

    Color m_color;
};

}

#endif
