#ifndef DECITREE_AI_HPP
#define DECITREE_AI_HPP

#include "board.hpp"
#include <unordered_map>

namespace connect_four {

/// AI using decision tree strategy, like the one used for tic tac toe
class DeciTreeAi {
public:
private:
    std::unordered_map<size_t, int> remembered_choices {};
};

}

#endif
