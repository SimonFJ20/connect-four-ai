#ifndef BOARD_HPP
#define BOARD_HPP

#include "printer.hpp"
#include "tile.hpp"
#include <cstddef>
#include <print>
#include <utility>

namespace connect_four {

using uint128_t = __uint128_t;

using Row = size_t;
using Col = size_t;

struct Pos {
    Col col;
    Row row;

    inline auto operator+(const Pos& other) -> Pos
    {
        return Pos { col + other.col, row + other.row };
    }
};

class PossibleMoves {
public:
    explicit PossibleMoves(size_t raw)
        : m_raw(raw)
    {
    }

    inline auto at(size_t i) const -> bool
    {
        return m_raw >> i & 1;
    }

private:
    size_t m_raw;
};

enum class GameState {
    RedWon,
    BlueWon,
    Draw,
    Ongoing,
};

enum class Color : uint8_t {
    Red,
    Blue,
};

[[maybe_unused]] inline auto color_win_state(Color color) -> GameState
{
    return color == Color::Red ? GameState::RedWon : GameState::BlueWon;
}

[[maybe_unused]] inline auto color_lose_state(Color color) -> GameState
{
    return color == Color::Red ? GameState::BlueWon : GameState::RedWon;
}

class Board {
public:
    static constexpr const size_t width = 7;
    static constexpr const size_t height = 6;

    auto possible_moves() const -> PossibleMoves;
    void insert(Col col, Tile tile);
    auto is_draw() const -> bool;
    auto game_state() const -> GameState;
    void print(Printer& printer) const;

    using Hash = size_t;
    auto hash() const -> Hash;
    auto flipped_hash() const -> size_t;

private:
    auto col_hash(Col col) const -> size_t;

    inline auto tile(Pos pos) const -> Tile
    {
        return static_cast<Tile>(m_val >> offset(pos) * tile_size & tile_mask);
    }

    inline void set_tile(Pos pos, Tile tile)
    {
        m_val |= static_cast<uint128_t>(std::to_underlying(tile))
            << offset(pos) * tile_size;
    }

    inline auto offset(Pos pos) const -> size_t
    {
        return pos.col * height + pos.row;
    }

    uint128_t m_val { 0 };
};

}

#endif
