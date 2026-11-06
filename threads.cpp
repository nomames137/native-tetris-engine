#include <SFML/Window.hpp>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Chcę mieć 2 wątki:  thread-render: ma interwał tyle ile trwa spadanie
//                     thread-logic: dziala niezaleznie od render i odpowiada za sterowanie


std::atomic<bool> running{true};
std::mutex mState;
GameState state;


void logicThread() {
    while (running) {
        firstManager.update();
        {
            std::lock_guard<std::mutex> lock(mGame);
            moveTet(firstManager, action_grid, liveRandomTet, live_tet_cords);
            rotateTet(firstManager, action_grid, liveRandomTet, live_tet_cords);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void renderThread() {
    auto lastDrop = std::chrono::steady_clock::now();
    while (running) {
        {
            std::lock_guard<std::mutex> lock(mGame);

            // drop interval: only for 500ms in example
            if (std::chrono::steady_clock::now() - lastDrop >= std::chrono::milliseconds(500)) {
                live_tet_cords[0]++;
                detect_collision_and_generate_next_tet(liveRandomTet);
                lastDrop = std::chrono::steady_clock::now();
            }

            // rendering: every cycle, i.e. 16 ms
            put_tet_on_action_grid(action_grid, liveRandomTet, live_tet_cords);
            display_visible_grid();
        }        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main() {
    clear_screen_once();
    bool running = true;
        cout << "\n";
        Tetromino liveRandomTet = TetrominoFactory::generateRandomTetromino();
        // liveRandomTet.print();
        
    while (running) {
        
        
        // action_grid[live_tet_cords[0]][live_tet_cords[1]] = 1;
        
    // cout << generate_dot_line(columns) << "\n";
    }
    return 0;
}