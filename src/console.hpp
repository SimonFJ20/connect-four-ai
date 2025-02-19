#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include "printer.hpp"
namespace connect_four {

class ConsolePrinter : public Printer {
public:
    void print_board(
        std::span<Tile> board, size_t width, size_t height) override;
};

}

#endif
