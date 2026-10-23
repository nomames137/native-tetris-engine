#include <iostream>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <random>
// #include <SFML/Audio.hpp>
// #include <SFML/Graphics.hpp>
// #include <SFML/Window/VideoMode.hpp>
#include <map>
#include <typeinfo>

using namespace std;
using namespace std::chrono_literals;


class Tetromino {
    private:
        vector<vector<int>> tet_shape;

        // Używamy inline static (od C++17), co pozwala na inicjalizację wewnątrz klasy
        inline static map<string, vector<vector<int>>> tetrominos = { 
            {"I-piece", { {0,0}, {-1,0}, {-2,0}, {-3,0}  } }, 
            {"O-piece", { {0,0}, {0,1},  {-1,0}, {-1,-1} } },
            {"T-piece", { {0,0}, {0,-1}, {0,1},  {-1,0}  } },
            {"S-piece", { {0,0}, {0,-1}, {-1,0}, {-1,1}  } },
            {"Z-piece", { {0,0}, {0,1},  {-1,0}, {-1,-1} } },
            {"J-piece", { {0,0}, {0,-1}, {-1,0}, {-2,0}  } },
            {"L-piece", { {0,0}, {0,1},  {-1,0}, {-2,0}  } } 
        };

        // Prywatny konstruktor - tylko Fabryka może tworzyć obiekty
        Tetromino(vector<vector<int>> tet_cords) : tet_shape(tet_cords) {}

        friend class TetrominoFactory;

    public:
        static const map<string, vector<vector<int>>>& get_shapes() {
            return tetrominos;
        }
        const vector<vector<int>>& get_tet_shape() const {
            return tet_shape;
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
            return (it->second);
        }
};

int main() {
    cout << "--- Tworzenie klocków po nazwie ---\n";
    for (const auto& tetName : Tetromino::get_shapes()) {
        TetrominoFactory::tetrominoName = tetName.first;
        // Teraz wywołanie statyczne działa poprawnie:
        Tetromino t = TetrominoFactory::createTetromino();
        cout << tetName.first << ": ";
        t.print();
    }

    cout << "\n--- Generowanie losowych klocków ---\n";
    for (int i = 0; i < 3; ++i) {
        Tetromino randomTet = TetrominoFactory::generateRandomTetromino();
        randomTet.print();
        cout << typeid(randomTet.get_tet_shape()).name() << endl;
    }

    return 0;
}