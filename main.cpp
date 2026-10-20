#include <iostream>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>

// Wymiary ukryte 10x40
// Wymiary widoczne 10x20

using namespace std;
using namespace std::chrono_literals;

const int columns = 10;
const int rows = 20;

const array<int, 2> start_spawn_cords = {0, 4};
array<int, 2> live_tet_cords = start_spawn_cords;

class InputManager {
    private:
        bool wasLeftPressed = false;
        bool wasRightPressed = false;
    public:
        bool leftKeyClicked = false;
        bool rightKeyClicked = false;

        void update() {
            bool isLeftNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
            bool isRightNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
            
            leftKeyClicked = (isLeftNow && !wasLeftPressed);
            rightKeyClicked = (isRightNow && !wasRightPressed);

            wasLeftPressed = isLeftNow;
            wasRightPressed = isRightNow;
        }
};

InputManager firstManager;

void moveTet(InputManager& manager) {
    if (manager.leftKeyClicked) {
        live_tet_cords[1]--;
    }
    if (manager.rightKeyClicked) {
        live_tet_cords[1]++;
    }
}


vector<std::string> line(int cols) {
    vector<std::string> line_vector(cols, " .");
    return line_vector;
}

string generate_dot_line(int cols) {
    string result = "";
    for (int i = 0; i < cols; i++) {
        result += " .";
    }
    return result;
}
// Grid for manipulation
vector<vector<int>> action_grid(rows, vector<int>(columns, 0));
// Grid for copying manipulating grid
vector<vector<string>> get_visible_grid(const vector<vector<int>>& action_grid) {
    vector<vector<string>> output_visible_grid(action_grid.size(), vector<string>(action_grid[0].size()));
    for (size_t i = 0; i < action_grid.size(); i++) {
        for (size_t j = 0; j < action_grid[i].size(); j++) {
            switch(action_grid[i][j]) {
                case 0:
                    output_visible_grid[i][j] = " .";
                    break;
                case 1:
                    output_visible_grid[i][j] = "[]";
                    break;
                default:
                    output_visible_grid[i][j] = " ?";
                    break;
            }
        }
    }
    return output_visible_grid;
}

void clear_screen() {
    // \033[2J - czyści cały ekran
    // \033[H  - przesuwa kursor w lewy górny róg
    std::cout << "\033[2J" << std::flush;
}
void reset_cursor() {
    std::cout << "\033[H" << std::flush;
}
void clear_screen_once() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void display_visible_grid() {
    reset_cursor();
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            cout << get_visible_grid(action_grid)[i][j] << " ";
        }
        cout << "\n";
    }
}

bool detect_collision(  const vector<vector<int>>& action_grid,
                        const array<int, 2>& live_tet_cords ) {
    bool collisionDetected = true;
    int row = live_tet_cords[0];
    int col = live_tet_cords[1];
    // Case when tet hits the ground
    if (row >= action_grid.size()) {
        return collisionDetected;
    }
    // Case when tet hits other tet in the same column
    if (action_grid[row][col] == 1) {
        return collisionDetected;
    }
    return !collisionDetected;
}

vector<int> detect_full_rows(const vector<vector<int>>& action_grid) {
    vector<vector<int>> grid = action_grid;
    vector<int> full_rows;
    for (int row = 0; row < grid.size(); row++) {
        if (all_of(grid[row].begin(), grid[row].end(), [](int x) { return x == 1; })) {
            full_rows.push_back(row);
        }
    }
    return full_rows;
}

void clear_full_rows(   const vector<int>& full_rows,
                        vector<vector<int>>& action_grid) {
    // vector<vector<int>> grid = action_grid;x
    for (int row_id : full_rows) {
        for_each(action_grid[row_id].begin(), action_grid[row_id].end(), [](int &x) { x = 0;});
    }
}



int main() {
    clear_screen_once();
    bool running = true;
        cout << "\n";
    while (running) {
        firstManager.update();
        moveTet(firstManager);
        action_grid[live_tet_cords[0]][live_tet_cords[1]] = 1;
        display_visible_grid();
        this_thread::sleep_for(100ms);
        live_tet_cords[0]++;

        if (detect_collision(action_grid, live_tet_cords)) {
            live_tet_cords = start_spawn_cords;
        } else {
            action_grid[live_tet_cords[0] - 1][live_tet_cords[1]] = 0;
        }
        clear_full_rows(detect_full_rows(action_grid), action_grid);
    }
    

    // cout << generate_dot_line(columns) << "\n";

    

    return 0;
}

// int windowSize[2] = {800, 600};

// int main() {
//     sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML window");
//     const sf::Texture texture("background_image.jpg");
//     sf::Sprite sprite(texture);
//     const sf::Font font("arial.ttf");
//     sf::Text text(font, "Hello SFML", 50);
//     sf::Music music("nice_music.mp3");

//     music.play();

//     while (window.isOpen()) {
//         while (const std::optional event = window.pollEvent()) {
//             if (event->is<sf::Event::Closed>())
//                 window.close();
//         }
//         window.clear();
//         window.draw(sprite);
//         window.draw(text);
//         window.display();
//     }
//     return 0;
// }