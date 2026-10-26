#include <iostream>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
// #include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <map>
#include <random>
#include <cmath>

// Wymiary ukryte 10x40
// Wymiary widoczne 10x20

using namespace std;
using namespace std::chrono_literals;

const int columns = 10;
const int rows = 20;

const array<int, 2> start_spawn_cords = {0, 4};
array<int, 2> live_tet_cords = start_spawn_cords;



class Tetromino {
    private:
        vector<vector<int>> tet_shape;

        // Używamy inline static (od C++17), co pozwala na inicjalizację wewnątrz klasy
        inline static map<string, vector<vector<int>>> tetrominos = { 
            {"I-piece", { {0,0}, {-1,0}, {-2,0}, {-3,0}  } }, 
            {"O-piece", { {0,0}, {0,1},  {-1,0}, {-1,1} } },
            {"T-piece", { {0,0}, {0,-1}, {0,1},  {-1,0}  } },
            {"S-piece", { {0,0}, {0,-1}, {-1,0}, {-1,1}  } },
            {"Z-piece", { {0,0}, {0,1},  {-1,0}, {-1,-1} } },
            {"J-piece", { {0,0}, {0,-1}, {-1,0}, {-2,0}  } },
            {"L-piece", { {0,0}, {0,1},  {-1,0}, {-2,0}  } } 
        };

        // Prywatny konstruktor - tylko Fabryka może tworzyć obiekty
        Tetromino(vector<vector<int>> tet_cords) : tet_shape(tet_cords) {}
        // ~Tetromino() {}

        friend class TetrominoFactory;

    public:
        static const map<string, vector<vector<int>>>& get_shapes() {
            return tetrominos;
        }
        const vector<vector<int>>& get_tet_shape() const {
            return tet_shape;
        }
        const vector<vector<int>> applyLiveTetCords(array<int,2> live_tet_cords) const {
                vector<vector<int>> absolute_cords = tet_shape;
                for (vector<int>& block : absolute_cords) {
                    block[0] += live_tet_cords[0];
                    block[1] += live_tet_cords[1];
                }
            return absolute_cords;
        }

        void testChangeTetShape(vector<vector<int>> new_shape) {
            tet_shape = new_shape;
        }
        void print() const {
            // Prosta funkcja pomocnicza do wypisania współrzędnych klocka
            for (const auto& coord : tet_shape) {
                cout << "[" << coord[0] << "," << coord[1] << "] ";
            }
            cout << "\n";
        }
};

class TetrominoFactory {
    public:
        // Zmiana na pole statyczne, aby pasowało do reszty logiki statycznej w main
        inline static string tetrominoName = "I-piece"; 
        // Zmiana na metodę statyczną, aby można ją było wywołać przez TetrominoFactory::createTetromino()
        static Tetromino createTetromino() {
            // Pobieramy kształty bezpośrednio przez statyczną metodę klasy Tetromino
            return Tetromino(Tetromino::get_shapes().at(tetrominoName));
        }
        // Nowa metoda generująca losowy klocek
        static Tetromino generateRandomTetromino() {
            const auto& shapes = Tetromino::get_shapes();
            // Konfiguracja nowoczesnego losowania w C++
            static random_device rd;
            static mt19937 gen(rd());
            // Ponieważ nie da się łatwo wylosować elementu z mapy po indeksie, 
            // losujemy dystans od początku mapy
            uniform_int_distribution<> distr(0, shapes.size() - 1);
            auto it = shapes.begin();
            advance(it, distr(gen)); // Przesuwamy iterator o wylosowaną liczbę pozycji
            // cout << "Wylosowano: " << it->first << " -> ";
            return Tetromino(it->second);
        }
};

class InputManager {
    private:
        bool wasLeftPressed = false;
        bool wasRightPressed = false;
        bool wasSpacePressed = false;
    public:
        bool leftKeyClicked = false;
        bool rightKeyClicked = false;
        bool spaceKeyClicked = false;

        void update() {
            bool isLeftNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
            bool isRightNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
            bool isSpaceNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
            
            leftKeyClicked = (isLeftNow && !wasLeftPressed);
            rightKeyClicked = (isRightNow && !wasRightPressed);
            spaceKeyClicked = (isSpaceNow && !wasSpacePressed);

            wasLeftPressed = isLeftNow;
            wasRightPressed = isRightNow;
            wasSpacePressed = isSpaceNow;
        }
};

InputManager firstManager;

bool blockIsValid ( const vector<vector<int>>& action_grid,
                    const vector<int>& block) {
    bool isValid = true;
    int block_row = block[0];
    int block_col = block[1];
    if (block_row >= 0 && block_row < action_grid.size() &&
        block_col >= 0 && block_col < action_grid[0].size()) {
        return isValid;
    } else {
        return !isValid;
    }
}

// trzeba dodac walidacje tego czy blok jest valid i czy moze byc displayed czy nie
void moveTet(   InputManager& manager,
                vector<vector<int>> action_grid,
                const Tetromino& tetromino,
                array<int,2>& live_tet_cords) {

    vector<vector<int>> blocks = tetromino.applyLiveTetCords(live_tet_cords);

    if (manager.leftKeyClicked) {
        bool canMoveLeft = all_of(blocks.begin(), blocks.end(), [action_grid, blocks](vector<int> block) {
            if (block[0] < 0) return false;
            if (block[1] > 0) {
                return  action_grid[block[0]][block[1] - 1] != 1 ||
                        find(blocks.begin(), blocks.end(), vector<int> {block[0], block[1] - 1}) != blocks.end();
            } else { return false; }
        });
        if (canMoveLeft) {
            live_tet_cords[1]--;
        }
    }
    if (manager.rightKeyClicked) {
        bool canMoveRight = all_of(blocks.begin(), blocks.end(), [action_grid, blocks](vector<int> block) {
            if (block[0] < 0) return false;
            if (block[1] < action_grid[0].size() - 1) {
                return  action_grid[block[0]][block[1] + 1] != 1 ||
                        find(blocks.begin(), blocks.end(), vector<int> {block[0], block[1] + 1}) != blocks.end();
            } else { return false; }
        });
        if (canMoveRight) {
            live_tet_cords[1]++;
        }
        
    }
}

void rotateTet( InputManager& manager,
                vector<vector<int>> action_grid,
                Tetromino& tetromino,
                array<int,2>& live_tet_cords) {
    const double PI = 3.14159265358979323846;
    const int sin90 = 1;
    const int cos90 = 0;

    

    vector<vector<int>> blocks = tetromino.get_tet_shape();

    // It is rotation matrix but for vector is less syntax
    vector<int> rotation_vector = {cos90, -sin90, sin90, cos90};

    //multiplying matrixes
    vector<vector<int>> new_blocks = blocks; 
    vector<vector<int>> rotated_tet;

    if (manager.spaceKeyClicked) {
    
        for_each(new_blocks.begin(), new_blocks.end(), [&](vector<int> new_block) {
                vector<int> test_block = {};
                test_block.push_back(new_block[0] * rotation_vector[0] + new_block[1] * rotation_vector[2]);
                test_block.push_back(new_block[0] * rotation_vector[1] + new_block[1] * rotation_vector[3]);
                rotated_tet.push_back(test_block);
        });
        tetromino.testChangeTetShape(rotated_tet);
        // tetromino.testChangeTetShape(tetromino.applyLiveTetCords(live_tet_cords));           
    // for (const auto& coord : rotated_tet) {
    //             cout << "[" << coord[0] << "," << coord[1] << "] ";
    //         }
    //         cout << "\n";
    
    }
}

// Grid for manipulation
vector<vector<int>> action_grid(rows, vector<int>(columns, 0));


void put_tet_on_action_grid(vector<vector<int>>& action_grid, 
                            const Tetromino& tetromino,
                            array<int, 2> live_tet_cords) {
    for (const vector<int>& block : tetromino.applyLiveTetCords(live_tet_cords)) {
        int block_row = block[0];
        int block_col = block[1];
        if (blockIsValid(action_grid, block)) {
            action_grid[block_row][block_col] = 1;
        }
    }
}
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
                        const Tetromino& tetromino,
                        const array<int, 2>& live_tet_cords ) {
    bool collisionDetected = true;
    vector<vector<int>> blocks = tetromino.applyLiveTetCords(live_tet_cords);
    for (const vector<int>& block : blocks) {
        int block_row = block[0];
        int block_col = block[1];
            vector<int> block_beneath = {block_row + 1, block_col};
            if (block_row >= action_grid.size()) {
                return collisionDetected;
            }
            if (action_grid[block_row][block_col] == 1 &&
                find(blocks.begin(), blocks.end(), block_beneath) == blocks.end()) {
                    return collisionDetected;
                }
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

void move_rows_after_clear( const vector<int> full_rows,
                            vector<vector<int>>& action_grid) {
    if (!full_rows.empty()) {
        for_each(full_rows.rbegin(), full_rows.rend(), [&](const auto &row_id) {
            for (int row = action_grid.size() - 1; row >= 0; row--) {
                if (row_id == row) {
                    for (int row_above = row_id - 1; row_above >= 0; row_above--) {
                        action_grid[row_above + 1] = action_grid[row_above];
                    }
                
                }
            }
            action_grid[0] = vector<int>(action_grid[0].size(), 0);
        }); 
    }
}


// To jest propozycja AI jak mozna rozwiazac funkcje przesuwania wierszy po usunieciu wiersza

/*
void move_rows_after_clear(const vector<int> full_rows,
                            vector<vector<int>>& action_grid) {
    if (full_rows.empty()) return;

    // 1. Tworzymy nową, tymczasową planszę wypełnioną pustymi wierszami
    int wiersze = action_grid.size();
    int kolumny = action_grid[0].size();
    vector<vector<int>> nowa_plansza(wiersze, vector<int>(kolumny, 0));

    // Indeks, pod który będziemy wstawiać klocki w nowej planszy (zaczynamy od samego dołu)
    int target_row = wiersze - 1;

    // 2. Przechodzimy przez starą planszę od dołu do góry
    for (int row = wiersze - 1; row >= 0; row--) {
        
        // Sprawdzamy, czy obecny wiersz znajduje się na liście do usunięcia
        bool czy_pelny = false;
        for (int f_row : full_rows) {
            if (f_row == row) {
                czy_pelny = true;
                break;
            }
        }

        // 3. JEŚLI WIERSZ NIE BYŁ PEŁNY -> przepisz go na dół nowej planszy
        if (!czy_pelny) {
            nowa_plansza[target_row] = action_grid[row];
            target_row--; // Przesuwamy wskaźnik zapisu nowej planszy o jeden wyżej
        }
        // Jeśli był pełny, po prostu go omijamy (wiersz znika, a wszystko z góry naturalnie opadnie!)
    }

    // 4. Nadpisujemy starą planszę nową, prawidłowo ułożoną planszą
    action_grid = nowa_plansza;
}
*/



void clear_full_rows(   const vector<int> full_rows,
                        vector<vector<int>>& action_grid) {
    for (int row_id : full_rows) {
        for_each(action_grid[row_id].begin(), action_grid[row_id].end(), [](int &x) { x = 0;});
    }
    //Wbudowanie funkcji raczej tymczasowe, ale działa
    move_rows_after_clear(full_rows, action_grid);
}

int main() {
    clear_screen_once();
    bool running = true;
        cout << "\n";
        Tetromino liveRandomTet = TetrominoFactory::generateRandomTetromino();
        // liveRandomTet.print();
        
    while (running) {
        
        firstManager.update();
        moveTet(firstManager, action_grid, liveRandomTet, live_tet_cords);
        rotateTet(firstManager, action_grid, liveRandomTet, live_tet_cords);
        // action_grid[live_tet_cords[0]][live_tet_cords[1]] = 1;
        put_tet_on_action_grid(action_grid, liveRandomTet, live_tet_cords);
        
        display_visible_grid();
        
        this_thread::sleep_for(200ms);
        live_tet_cords[0]++;
        
        

        if (detect_collision(action_grid, liveRandomTet, live_tet_cords)) {
            live_tet_cords = start_spawn_cords;
            cout << "Collision detected" << endl;
            // Tetromino newLiveRandomTet = TetrominoFactory::generateRandomTetromino();
            liveRandomTet = TetrominoFactory::generateRandomTetromino();
            // liveRandomTet.reset(new TetrominoFactory::generateRandomTetromino());
        } else {
            for (const vector<int>& block : liveRandomTet.applyLiveTetCords(live_tet_cords)) {
                int block_row = block[0];
                int block_col = block[1];
                if (block_row - 1 >= 0 && block_row - 1 <= action_grid.size() && block_col >= 0 && block_col < action_grid[0].size()) {
                    action_grid[block_row - 1][block_col] = 0;
                }
                
            cout << "Collision detected false" << endl;
            // action_grid[live_tet_cords[0] - 1][live_tet_cords[1]] = 0;
        }
        clear_full_rows(detect_full_rows(action_grid), action_grid);
        display_visible_grid();
        

        // Ta funkcja została wbudowana w funkcje clear_full_rows()
        // move_rows_after_clear(detect_full_rows(action_grid), action_grid);
        }
    // cout << generate_dot_line(columns) << "\n";
    
    }
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