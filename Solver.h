//
// Created by Alibaba on 26/09/2024.
//

#ifndef SOLVER_H
#define SOLVER_H

#include <array>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace my_types {
    using cube = std::vector<std::array<std::string, 12> >;
    using side = std::array<std::array<std::string, 3>, 3>;
}

class Solver {
public:
    static const std::array<std::string, 4> white_targets;
    static const std::unordered_map<std::string, int> rotation_dir;

    explicit Solver(my_types::cube &cube) : cube(cube) {}

    void left_algorithm();

    void right_algorithm();

    void change_center(std::string& target);

    void roll_freebies();

    void find_new_freebies();

    std::pair<std::string, std::string> find_last_target();

    int find_targets(my_types::side &side, std::array<std::string, 4> &targets);

    void daisy();

    void white_cross();

    std::vector<int> find_target_pos(my_types::side &matrix, std::string target);

    my_types::side rotate_matrix(my_types::side &matrix);

    std::string find_relative_position(my_types::side &matrix);

    std::pair<std::vector<std::string>, std::string> get_neighbours(std::vector<int>& target_pos);

    bool is_white_done();

    bool is_aligned(std::vector<std::string>& neighbours, std::string);

    void move_to_front(std::string center);

    void change_side_and_rotate(std::string side, int rotation);

    void fix_rogue();

    void white_corners();

    std::pair<std::string, std::array<std::pair<int, int>, 3> > find_edge();

    std::string is_second_done();

    void fix_second(std::string &fix_info);

    void second_layer_algo(std::string &side);

    void second_layer();

    void yellow_cross();

    bool third_layer();

    void solve();

private:
    my_types::cube cube;
};


#endif //SOLVER_H
