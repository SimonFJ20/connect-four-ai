#include "minimax.hpp"
#include "board.hpp"
#include <algorithm>
#include <cstdint>
#include <print>
#include <tuple>
#include <vector>

using namespace connect_four;

auto Minimax::choose(Board board, size_t depth) const -> Col
{
    auto choice = find_move(board, depth, m_color);
    return choice.col;
}

auto Minimax::find_move(Board board, size_t depth, Color turn) const -> Choice
{
    auto moves = std::vector<std::tuple<uint16_t, int32_t>>();
    auto possible_moves = board.possible_moves();
    for (uint16_t col = 0; col < board.width; ++col) {
        if (!possible_moves.at(col))
            continue;
        auto board_clone = board;
        auto pos = board_clone.insert(col, m_tile);
        auto choice = after_move(board_clone, depth, color_opposite(turn), pos);
        moves.push_back(std::tuple { col, choice.points });
    }

    auto [col, points] = turn == m_color
        ? *std::max_element(moves.begin(), moves.end(),
              [](auto a, auto b) { return std::get<1>(a) < std::get<1>(b); })

        : *std::min_element(moves.begin(), moves.end(),
              [](auto a, auto b) { return std::get<1>(a) < std::get<1>(b); });

    return { .points = points, .col = col, .type = ChoiceType::Pos };
}

auto Minimax::after_move(Board board, size_t depth, Color turn,
    [[maybe_unused]] Pos pos) const -> Choice
{
    if (board.is_draw())
        return { .points = 0, .col = 0, .type = ChoiceType::Result };
    auto state = board.game_state();
    switch (state) {
        case GameState::RedWon:
        case GameState::BlueWon:
            return {
                .points = state == color_win_state(m_color) ? 1000 : -1000,
                .col = 0,
                .type = ChoiceType::Result,
            };
        case GameState::Draw:
        case GameState::Ongoing:
            break;
    }

    if (depth == 0) {
        return { .points = 10, .col = 0, .type = ChoiceType::Result };
    }

    return find_move(board, depth - 1, turn);
}
