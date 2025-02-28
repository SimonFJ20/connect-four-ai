#ifndef MINIMAX_HPP
#define MINIMAX_HPP

#include "board.hpp"
#include <cstdint>
namespace connect_four {

class Minimax {
public:
    Minimax(Color color)
        : m_color(color)
        , m_tile(color_to_tile(color))
    {
    }

    enum class ChoiceType : int16_t {
        Result,
        Pos,
    };
    struct Choice {
        int32_t points;
        uint16_t col;
        // it's placed down here for alignment purposes
        ChoiceType type;
    };

    auto choose(Board board, size_t depth) const -> Col;

private:
    auto find_move(Board board, size_t depth, Color turn) const -> Choice;
    auto after_move(Board board, size_t depth, Color turn, Pos pos) const
        -> Choice;
    auto value_of_board(Board board) const -> int32_t;

    Color m_color;
    Tile m_tile;
};

}

#endif
