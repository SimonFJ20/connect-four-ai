#include "board.hpp"
#include "tile.hpp"
#include <utility>
#include <vector>

using namespace connect_four;

auto Board::possible_moves() const -> PossibleMoves
{
    size_t res = 0;
    for (size_t x = 0; x < width; ++x) {
        if (tile({ x, 0 }) == Tile::Empty)
            res |= 1 << x;
    }
    return PossibleMoves(res);
}

auto Board::win_possibilities_at_pos(Color color, uint16_t col, uint16_t row) const -> size_t
{
    if (col >= width || row >= height) {
        return 0;
    }

    auto directions = std::array<std::tuple<size_t, size_t>, 4> {
        std::tuple { 1, -1 },
        std::tuple { 1, 0 },
        std::tuple { 0, 1 },
        std::tuple { 1, 1 },
    };

    size_t result = 0;

    for (auto direction : directions) {
        for (size_t i = 0; i <= 3; ++i) {
            bool can_win = true;

            for (size_t j = i - 3; j <= i; ++j) {
                auto x = col + std::get<0>(direction) * j;
                auto y = row + std::get<1>(direction) * j;

                if (col > width || row > height) {
                    return 0;
                }

                if (tile({ x, y }) != Tile::Empty && tile({ x, y }) != color_to_tile(color)) {
                    can_win = false;
                }
            }

            if (can_win) {
                result++;
            }
        }
    }

    return result;
}

auto Board::insert(Col col, Tile tile) -> Pos
{
    for (int64_t y = height - 1; y >= 0; --y) {
        auto pos = Pos { .col = col, .row = static_cast<size_t>(y) };
        if (this->tile(pos) == Tile::Empty) {
            set_tile(pos, tile);
            return pos;
        }
    }
    std::unreachable();
}

auto Board::is_draw() const -> bool
{
    for (size_t col = 0; col < width; ++col) {
        if (this->tile({ .col = col, .row = 0 }) == Tile::Empty) {
            return false;
        }
    }

    return true;
}

constexpr auto make_patterns()
    -> std::array<std::tuple<Pos, std::array<Pos, 4>>, 4>
{
    return std::array {
        std::tuple {
            Pos { 1, 4 },
            std::array {
                Pos { 0, 0 }, Pos { 0, 1 }, Pos { 0, 2 }, Pos { 0, 3 } },
        },
        std::tuple {
            Pos { 4, 1 },
            std::array {
                Pos { 0, 0 }, Pos { 1, 0 }, Pos { 2, 0 }, Pos { 3, 0 } },
        },
        std::tuple {
            Pos { 4, 4 },
            std::array {
                Pos { 0, 0 }, Pos { 1, 1 }, Pos { 2, 2 }, Pos { 3, 3 } },
        },
        std::tuple {
            Pos { 4, 4 },
            std::array {
                Pos { 0, 3 }, Pos { 1, 2 }, Pos { 2, 1 }, Pos { 3, 0 } },
        },
    };
}

auto Board::game_state() const -> GameState
{
    constexpr auto patterns = make_patterns();

    for (auto& [pos, tiles] : patterns) {
        for (size_t x = 0; x < width; ++x) {
            for (size_t y = 0; y < height; ++y) {
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

void Board::print(Printer& printer) const
{
    auto board = std::vector<Tile>();
    board.reserve(width * height);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            board.push_back(tile({ .col = x, .row = y }));
        }
    }
    printer.print_board(board, width, height);
}

auto Board::hash() const -> Hash
{
    static_assert(sizeof(size_t) == sizeof(uint64_t));

    size_t res = 0;
    for (size_t col = 0; col < width; ++col) {
        auto col_res = col_hash(col);
        res |= col_res << 9 * col;
    }
    return res;
}

auto Board::flipped_hash() const -> size_t
{
    static_assert(sizeof(size_t) == sizeof(uint64_t));

    size_t res = 0;
    for (size_t col = width; col < width; ++col) {
        auto col_res = col_hash(col);
        res |= col_res << 9 * (width - col - 1);
    }
    return res;
}

auto Board::col_hash(Col col) const -> size_t
{
    size_t col_height = 6;
    size_t hash = 0;

    for (size_t row = 0; row < height; ++row) {
        switch (tile({ col, row })) {
            case Tile::Empty:
                col_height -= 1;
                break;
            case Tile::Red:
                hash |= 1 << row;
                break;
            case Tile::Blue:
                break;
        }
    }
    hash |= col_height << 6;
    return hash;
}

auto Board::as_mx1() const -> Mx1
{
    auto v = [](Tile tile) {
        return tile == Tile::Empty ? 0.5 : tile == Tile::Blue ? 0.0 : 1.0;
    };

    auto m = Mx1(width * height);
    for (size_t row = 0; row < width; ++row) {
        for (size_t col = 0; col < height; ++col) {
            m[row * height + col] = v(tile({ row, col }));
        }
    }
    return m;
}
