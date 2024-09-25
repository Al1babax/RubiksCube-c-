#include <iostream>
#include <array>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <random>

namespace my_types {
    using cube = std::vector<std::array<std::string, 12> >;
    using side = std::array<std::array<std::string, 3>, 3>;
}

class Colors {
public:
    // Terminal color codes for color dictionary
    const std::unordered_map<std::string, std::pair<std::string, std::string> > color_dict = {
        {"orange", {"\033[30;48;5;208m", "\033[0m"}}, // Bright orange background (custom), black foreground
        {"red", {"\033[30;48;5;196m", "\033[0m"}}, // Red background, black foreground
        {"green", {"\033[30;48;5;46m", "\033[0m"}}, // Green background, black foreground
        {"yellow", {"\033[30;48;5;226m", "\033[0m"}}, // Bright yellow background, black foreground
        {"blue", {"\033[30;48;5;39m", "\033[0m"}}, // Blue background, black foreground
        {"white", {"\033[30;107m", "\033[0m"}} // Bright white background, black foreground
    };

    // Mapping square characters to colors
    const std::unordered_map<char, std::string> square_to_color = {
        {'O', "orange"},
        {'R', "red"},
        {'G', "green"},
        {'Y', "yellow"},
        {'B', "blue"},
        {'W', "white"}
    };

    std::string color_to_text(const std::string &text, const char &color) const {
        const auto color_name = square_to_color.at(color);
        const auto ansi_ends = color_dict.at(color_name);
        const std::string colored_text = ansi_ends.first + text + ansi_ends.second;
        return colored_text;
    }
};


class Cube {
public:
    // Colors
    const Colors colorer;

    // memory
    my_types::cube cube;
    std::uint8_t back_updates = 0;

    const std::array<char, 7> colors = {'O', 'G', 'W', 'B', 'Y', 'R', 'Y'};
    const std::array<std::string, 7> all_sides = {"top", "left", "front", "right", "back", "bottom", "back"};
    const std::unordered_map<std::string, std::pair<uint8_t, uint8_t> > sides_dict = {
        {"top", {0, 1}},
        {"left", {1, 0}},
        {"front", {1, 1}},
        {"right", {1, 2}},
        {"back1", {1, 3}},
        {"bottom", {2, 1}},
        {"back2", {3, 1}}
    };

    explicit Cube(const bool debug = false) : debug(debug) {
        // init cube
        create_cube();
    };

    static my_types::side create_side(const char symbol) {
        // create 3x3 array of the side with symbol
        my_types::side temp_side;

        const std::string symbol_str(1, symbol);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (symbol == '-') {
                    temp_side[i][j] = symbol_str;
                } else {
                    temp_side[i][j] = symbol_str + std::to_string((3 * i) + (j + 1));
                }
            }
        }

        return temp_side;
    }

    void create_cube() {
        // Construct the cube into memory
        std::vector<my_types::side> side_arrays;
        side_arrays.reserve(16);
        int color_pointer = 0;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if ((i == 0 || i == 2 || i == 3) && (j == 0 || j == 2 || j == 3)) {
                    side_arrays.push_back(create_side('-'));
                } else {
                    side_arrays.push_back(create_side(colors[color_pointer]));
                    color_pointer++;
                }
            }
        }

        // populate the main cube vector with the sides created taking 4 at a time

        // 4 Side rows
        for (int big_side_row_i = 0; big_side_row_i < 4; big_side_row_i++) {
            // 3 rows in each side
            for (int side_row_i = 0; side_row_i < 3; side_row_i++) {
                std::array<std::string, 12> new_row;
                // Loop over 4 sides at once
                for (int side_i = 0; side_i < 4; side_i++) {
                    // Loop over each value on these sides
                    for (int square_i = 0; square_i < 3; square_i++) {
                        new_row[(side_i * 3) + square_i] = side_arrays[(big_side_row_i * 4) + side_i][side_row_i][
                            square_i];
                    }
                }
                cube.push_back(new_row);
            }
        }

        // sync back
        back_updates = 1;
        sync_back();
    }

    void show() const {
        // print the cube to console
        for (auto &row: cube) {
            for (auto &col: row) {
                if (col == "-") {
                    std::cout << " -  ";
                } else {
                    const std::string text = " " + col + " ";
                    const std::string colored_text = colorer.color_to_text(text, col[0]);
                    if (debug) {
                        std::cout << text;
                    } else {
                        std::cout << colored_text;
                    }
                }
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }

    my_types::side get_side(const std::string &side) const {
        // Using map sides_dict return array of side 3x3
        my_types::side side_array;
        std::pair<uint8_t, uint8_t> coordinate_pair = sides_dict.at(side);
        uint8_t row_start = coordinate_pair.first * 3;
        uint8_t col_start = coordinate_pair.second * 3;

        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                side_array[row][col] = cube[row_start + row][col_start + col];
            }
        }

        return side_array;
    }

    void set_side(const std::string &side, const my_types::side &side_array) {
        std::pair<uint8_t, uint8_t> coordinate_pair = sides_dict.at(side);
        uint8_t row_start = coordinate_pair.first * 3;
        uint8_t col_start = coordinate_pair.second * 3;

        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                cube[row_start + row][col_start + col] = side_array[row][col];
            }
        }
    }

    void sync_back() {
        if (back_updates == 0) {
            return;
        }

        // get real side and side to be updated
        const std::string side_to_update = back_updates == 2 ? "back1" : "back2";
        const std::string real_side = back_updates == 2 ? "back2" : "back1";
        const my_types::side updated_side = get_side(real_side);

        // Mirror everything
        my_types::side new_side;

        for (int row = 2; row > -1; row--) {
            for (int col = 2; col > -1; col--) {
                new_side[2 - row][2 - col] = updated_side[row][col];
            }
        }

        set_side(side_to_update, new_side);
        back_updates = 0;
    }

    static std::array<std::string, 3> slice_array(const std::array<std::string, 12> &row, int start, int stop) {
        std::array<std::string, 3> new_slice;

        if (start < 0) {
            start = 12 + start;
        }
        if (stop < 0) {
            stop = 12 + stop;
        }
        int items_added = 0;
        for (int i = 0; i < 12; i++) {
            if (start <= i && i <= stop) {
                new_slice[items_added] = row[i];
                items_added++;
            }
        }

        return new_slice;
    }


    void slide_long(const std::string &direction, const int row, const int col) {
        if (direction == "right") {
            std::array<std::string, 12> new_line;

            for (int i = 0; i < 12; i++) {
                if (i < 3) {
                    new_line[i] = cube[row][i + 9];
                } else {
                    new_line[i] = cube[row][i - 3];
                }
            }

            cube[row] = new_line;

            if (row == 3) {
                rotate_side("top", "counter-clockwise");
            } else if (row == 5) {
                rotate_side("bottom", "clockwise");
            }
        } else if (direction == "left") {
            std::array<std::string, 12> new_line;

            for (int i = 0; i < 12; i++) {
                if (i < 9) {
                    new_line[i] = cube[row][i + 3];
                } else {
                    new_line[i] = cube[row][i - 9];
                }
            }

            cube[row] = new_line;

            if (row == 3) {
                rotate_side("top", "clockwise");
            } else if (row == 5) {
                rotate_side("bottom", "counter-clockwise");
            }
        } else if (direction == "up") {
            std::array<std::string, 12> new_line;

            for (int i = 0; i < 12; i++) {
                if (i < 9) {
                    new_line[i] = cube[i + 3][col];
                } else {
                    new_line[i] = cube[i - 9][col];
                }
            }

            for (int i = 0; i < 12; i++) {
                cube[i][col] = new_line[i];
            }

            if (col == 3) {
                rotate_side("left", "counter-clockwise");
            } else if (col == 5) {
                rotate_side("right", "clockwise");
            }
        } else if (direction == "down") {
            std::array<std::string, 12> new_line;

            for (int i = 0; i < 12; i++) {
                if (i < 3) {
                    new_line[i] = cube[i + 9][col];
                } else {
                    new_line[i] = cube[i - 3][col];
                }
            }

            for (int i = 0; i < 12; i++) {
                cube[i][col] = new_line[i];
            }

            if (col == 3) {
                rotate_side("left", "clockwise");
            } else if (col == 5) {
                rotate_side("right", "counter-clockwise");
            }
        } else {
            throw std::runtime_error("Slide long function failed with direction: " + direction);
        }

        // Manage back sync
        if (direction == "up" || direction == "down") {
            back_updates = 2;
        } else if (direction == "left" || direction == "right") {
            back_updates = 1;
        }

        sync_back();
    }

    void rotate_side(const std::string &side, const std::string &direction) {
        my_types::side temp_side = get_side(side);
        my_types::side new_side;

        for (int col = 0; col < 3; col++) {
            for (int row = 0; row < 3; row++) {
                if (direction == "clockwise") {
                    new_side[col][row] = temp_side[2 - row][col];
                } else if (direction == "counter-clockwise") {
                    new_side[col][row] = temp_side[row][2 - col];
                }
            }
        }

        set_side(side, new_side);
    }

    static void print_side(const my_types::side &side) {
        for (auto &row: side) {
            for (auto &col: row) {
                std::cout << col << " ";
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }

    void rotate_big(const std::string &side) {
        if (side == "back1" || side == "back2") {
            change_perspective("right");
            change_perspective("right");
        }

        change_perspective("right");
        slide_long("up", 5, 5);
        change_perspective("left");

        if (side == "back1" || side == "back2") {
            change_perspective("right");
            change_perspective("right");
        }
    }

    void change_perspective(const std::string &direction) {
        if (direction == "up") {
            for (int i = 0; i < 3; i++) {
                slide_long("up", 0, 3 + i);
            }
        } else if (direction == "down") {
            for (int i = 0; i < 3; i++) {
                slide_long("down", 0, 3 + i);
            }
        } else if (direction == "right") {
            for (int i = 0; i < 3; i++) {
                slide_long("right", 3 + i, 0);
            }
        } else if (direction == "left") {
            for (int i = 0; i < 3; i++) {
                slide_long("left", 3 + i, 0);
            }
        }
    }

    void rotate_whole() {
        for (const auto &key: sides_dict) {
            const std::string &k = key.first;
            if (k == "back2") {
                continue;
            }
            if (k == "back1") {
                rotate_side(k, "counter-clockwise");
            } else {
                rotate_side(k, "clockwise");
            }
        }

        // Sync back
        back_updates = 1;
        sync_back();

        my_types::side temp = get_side("top");
        set_side("top", get_side("left"));
        set_side("left", get_side("bottom"));
        set_side("bottom", get_side("right"));
        set_side("right", temp);
    }

    void random_shuffle(const int amount) {
        const std::array<std::string, 4> directions = {"up", "down", "left", "right"};
        constexpr std::array<int8_t, 3> numbers = {3, 4, 5};
        const std::array<std::string, 3> actions = {"rotate", "slide", "p_change"};

        std::random_device rd; // Random device to seed the generator
        std::mt19937 gen(rd()); // Mersenne Twister generator
        std::uniform_int_distribution<> dis(0, 2); // Distribution range [0, 2]
        std::uniform_int_distribution<> dis2(0, 3);

        // Memory slot for random choices
        std::string random_action;
        random_action.reserve(10);

        uint8_t rotate_amount = 0;

        std::string random_direction;
        random_direction.reserve(10);

        uint8_t random_roc = 0;

        for (int i = 0; i < amount; i++) {
            random_action = actions[dis(gen)];

            if (random_action == "rotate") {
                rotate_amount = dis(gen) + 1;

                for (int j = 0; j < rotate_amount; j++) {
                    rotate_big("front");
                }
            } else if (random_action == "slide") {
                random_direction = directions[dis2(gen)];
                random_roc = numbers[dis(gen)];
                slide_long(random_direction, random_roc, random_roc);
            } else if (random_action == "p_change") {
                random_direction = directions[dis2(gen)];
                change_perspective(random_direction);
            }
        }
    }

    void controls() {
        const std::unordered_map<char, std::string> direction_shorts = {
            {'u', "up"},
            {'d', "down"},
            {'l', "left"},
            {'r', "right"},
        };

        while (true) {
            show();
            std::string user_input;
            std::cout <<
                    "Select select row/col and u/d/l/r (up/down/left/right)(q to quit) or change perspective u/d/l/r or rotate front f or ra = righty algo, la = lefty algo, wr = whole rotate, wc = white cross, sl = second layer: \n";
            std::cin >> user_input;

            if (user_input == "q") {
                break;
            }

            if (std::isdigit(user_input[0]) && user_input.size() == 2) {
                const std::string &direction = direction_shorts.at(user_input[1]);
                const int roc = user_input[0] - '0';
                slide_long(direction, roc, roc);
            } else if (user_input == "u" || user_input == "d" || user_input == "l" || user_input == "r") {
                const std::string &direction = direction_shorts.at(user_input[0]);
                change_perspective(direction);
            } else if (user_input == "wr") {
                rotate_whole();
            } else if (user_input == "f") {
                rotate_big("front");
            } else {
                std::cout << "Invalid user input\n";
            }
        }
    }

private:
    bool debug;
};

int main() {
    const auto start = std::chrono::high_resolution_clock::now();

    Cube my_cube(false);
    my_cube.random_shuffle(1000);
    my_cube.controls();

    const auto end = std::chrono::high_resolution_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

    std::cout << "Runtime: " << duration << " s" << std::endl;


    return 0;
}
