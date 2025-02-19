#ifndef DECITREE_AI_HPP
#define DECITREE_AI_HPP

#include "board.hpp"
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

using Weight = int;

using Choice = uint16_t;

[[maybe_unused]]
static inline auto choice_from(Color color, Col col, uint8_t depth) -> Choice
{
    return static_cast<Choice>(
        static_cast<uint64_t>(std::to_underlying(color)) << 15 | col << 8
        | depth);
}

[[maybe_unused]]
static inline auto choice_color(Choice choice) -> Color
{
    return static_cast<Color>(choice >> 15);
}

[[maybe_unused]]
static inline auto choice_col(Choice choice) -> uint8_t
{
    return choice >> 8;
}

[[maybe_unused]]
static inline auto choice_depth(Choice choice) -> uint8_t
{
    return choice & 0xFF;
}

[[maybe_unused]]
static inline auto choice_flipped(Choice choice) -> Choice
{
    return choice_from(choice_color(choice),
        Board::width - choice_col(choice) - 1, choice_depth(choice));
}

/// AI using decision tree strategy, like the one used for tic tac toe
class DeciTreeAi {
public:
    DeciTreeAi(Tile color)
        : m_color(color == Tile::Blue ? Color::Blue : Color::Red)
    {
    }

    auto next_move(const Board& board) -> size_t;
    void report_move(Col col);

    inline void incr_depth() { m_depth += 1; }

    void report_win();
    void report_loss();
    void report_draw();

private:
    auto lookup_choice(Col col) -> std::tuple<Choice, Weight>;
    auto choice_is_candidate(int weight, int cand_weight) const -> bool;

    void check_choice_flip();
    auto flipped_more_populated() const -> bool;
    void flip_current_choices();
    void reward_punish_current_choices(int reward);

    std::unordered_map<Choice, Weight> m_choice_weights {};

    std::vector<Choice> m_current_choices {};
    uint8_t m_depth = 0;

    Color m_color;
};

}

#endif
