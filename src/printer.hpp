#ifndef PRINTER_HPP
#define PRINTER_HPP

#include "tile.hpp"
#include <cstddef>
#include <span>

namespace connect_four {

class Printer {
public:
    virtual ~Printer() = default;

    virtual void print_board(std::span<Tile> board, size_t width, size_t height)
        = 0;
};

}

#endif
