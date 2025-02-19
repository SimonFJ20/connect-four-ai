#ifndef TILE_HPP
#define TILE_HPP

#include <cstddef>

namespace connect_four {

enum class Tile : size_t {
    Empty,
    Red,
    Blue,
};

[[maybe_unused]] static constexpr const size_t tile_size = 2;
[[maybe_unused]] static constexpr const size_t tile_mask = 0b11;

}

#endif
