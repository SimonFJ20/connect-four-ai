#ifndef BOARD_HPP
#define BOARD_HPP

#include "printer.hpp"
#include "tile.hpp"
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <print>
#include <tuple>
#include <utility>
#include <vector>

namespace connect_four {

using uint128_t = __uint128_t;

struct Pos {
    size_t x;
    size_t y;

    inline auto operator+(Pos& other) -> Pos
    {
        return Pos { x + other.x, y + other.y };
    }
};

static constexpr const size_t tile_size = 2;
static constexpr const size_t tile_mask = 0b11;

enum class GameState {
    RedWon,
    BlueWon,
    Draw,
    Ongoing,
};

static constexpr const size_t board_width = 7;
static constexpr const size_t board_height = 6;

class Board {
public:
    inline auto insert(size_t col, Tile tile)
    {
        for (int64_t y = board_height - 1; y >= 0; --y) {
            auto pos = Pos { .x = col, .y = static_cast<size_t>(y) };
            if (this->tile(pos) == Tile::Empty) {
                set_tile(pos, tile);
                return;
            }
        }
    }

    inline auto possible_moves() -> size_t
    {
        size_t res = 0;
        for (size_t x = 0; x < board_width; ++x) {
            if (tile({ x, 0 }) == Tile::Empty)
                res |= 1 << x;
        }
        return res;
    }

    inline auto hash() -> size_t
    {
        size_t hash = m_val & 0xFFFFFFFFFFFFFFFF;
        hash ^= m_val >> 64;
        return hash;
    }

    inline auto game_state() -> GameState
    {
        // clang-format off
        auto patterns = std::array {
            std::tuple {
                Pos { 1, 4 },
                std::array { Pos { 0, 0 }, Pos { 0, 1 }, Pos { 0, 2 }, Pos { 0, 3 } },
            },
            std::tuple {
                Pos { 4, 1 },
                std::array { Pos { 0, 0 }, Pos { 1, 0 }, Pos { 2, 0 }, Pos { 3, 0 } },
            },
            std::tuple {
                Pos { 4, 4 },
                std::array { Pos { 0, 0 }, Pos { 1, 1 }, Pos { 2, 2 }, Pos { 3, 3 } },
            },
            std::tuple {
                Pos { 1, 4 },
                std::array { Pos { 0, 3 }, Pos { 1, 2 }, Pos { 2, 1 }, Pos { 3, 0 } },
            },
        };
        // clang-format on

        for (auto& [pos, tiles] : patterns) {
            for (size_t x = 0; x < board_width; ++x) {
                for (size_t y = 0; y < board_height; ++y) {
                    bool has_won = true;
                    auto first_tile = tile(Pos { x, y } + tiles[0]);

                    if (first_tile == Tile::Empty) {
                        continue;
                    }

                    for (auto [tx, ty] : tiles) {
                        auto current_tile = tile(Pos { x + tx, y + ty });

                        if (first_tile != current_tile) {
                            has_won = false;
                            break;
                        }
                    }

                    if (has_won) {
                        return first_tile == Tile::Red ? GameState::RedWon
                                                       : GameState::BlueWon;
                    }
                }
            }
        }

        return this->is_draw() ? GameState::Draw : GameState::Ongoing;
    }

    inline auto is_draw() -> bool
    {
        for (size_t x = 0; x < board_width; ++x) {
            if (this->tile({ .x = x, .y = board_height - 1 }) == Tile::Empty) {
                return false;
            }
        }

        return true;
    }

    template <typename PrinterT>
        requires std::derived_from<PrinterT, Printer>
    void print(PrinterT& printer)
    {
        auto board = std::vector<Tile>();
        board.reserve(board_width * board_height);
        for (size_t y = 0; y < board_height; ++y) {
            for (size_t x = 0; x < board_width; ++x) {
                board.push_back(tile({ .x = x, .y = y }));
            }
        }
        printer.print_board(board, board_width, board_height);
    }

private:
    inline auto tile(Pos pos) -> Tile
    {
        return static_cast<Tile>(m_val >> offset(pos) * tile_size & tile_mask);
    }

    inline void set_tile(Pos pos, Tile tile)
    {
        m_val |= static_cast<uint128_t>(std::to_underlying(tile))
            << offset(pos) * tile_size;
    }

    inline auto offset(Pos pos) -> size_t
    {
        return pos.x * board_height + pos.y;
    }

    uint128_t m_val { 0 };
};

}

#endif
