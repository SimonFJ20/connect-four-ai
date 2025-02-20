#include "board.hpp"
#include "tile.hpp"
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

void Board::insert(Col col, Tile tile)
{
    for (int64_t y = height - 1; y >= 0; --y) {
        auto pos = Pos { .col = col, .row = static_cast<size_t>(y) };
        if (this->tile(pos) == Tile::Empty) {
            set_tile(pos, tile);
            return;
        }
    }
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

auto Board::hash() const -> size_t
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
