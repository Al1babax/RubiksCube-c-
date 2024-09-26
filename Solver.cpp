//
// Created by Alibaba on 26/09/2024.
//

#include "Solver.h"

// Initialize static members
const std::array<std::string, 4> Solver::white_targets = {"W2", "W4", "W6", "W8"};

const std::unordered_map<std::string, int> Solver::rotation_dir = {
    {"left", 270},
    {"top", 180},
    {"right", 90},
    {"bottom", 0}
};
