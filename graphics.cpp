#include <iostream>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <map>
#include <random>
#include <cmath>
#include <mutex>
#include <string>
#include <array>

class Grid;

class GameWindow {
private:
    static constexpr sf::Vector2u SIZE = {800, 600};
    sf::RenderWindow window;

    // sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML window");
    GameWindow() : window(sf::VideoMode(SIZE), "Tetris") {}

public:
    static GameWindow& getInstance() {
        static GameWindow instance;
        return instance;
    }
    
    const sf::Vector2u getSize() {
        return getInstance().SIZE;
    }
        
    
    GameWindow(const GameWindow&) = delete;
    GameWindow& operator = (const GameWindow&) = delete;

    sf::RenderWindow& get() {return window;}

    void run(Grid& grid);
    // {
    //     while (window.isOpen()) {
    //         while (const auto event = window.pollEvent()) {
    //             if (event->is<sf::Event::Closed>()) 
    //                 window.close();
                
    //         }
    //         window.clear();
    //         grid.draw(window);
    //         window.display();
    //     }
    // }


friend class Grid;
};



        



class Grid {
private:
    sf::Context grid;
    // sf::View view;

    inline static sf::Vector2u getViewSize() {
        sf::Vector2u win = GameWindow::getInstance().getSize();
        unsigned height = win.y * 9 / 10;
        unsigned width = win.y / 2;
        return {width, height};
    }
    inline static sf::Vector2u getViewPosition() {
        sf::Vector2u win = GameWindow::getInstance().getSize();
        unsigned border_width{static_cast<unsigned> (win.y * 0.5)};
        sf::Vector2u view_position = {border_width, border_width};
        return view_position;
    }

    const sf::Vector2f VIEW_SIZE = sf::Vector2f(Grid::getViewSize());
    const sf::Vector2f VIEW_POS = sf::Vector2f(Grid::getViewPosition());
    sf::View view;

public:
    Grid() : view(sf::FloatRect({0.f, 0.f}, VIEW_SIZE))
    {
        sf::Vector2f win = sf::Vector2f(GameWindow::getInstance().getSize());
        sf::Vector2f pos = sf::Vector2f(VIEW_POS);
        sf::Vector2f size = sf::Vector2f(VIEW_SIZE);

        view.setViewport(sf::FloatRect(
            {pos.x / win.x,  pos.y / win.y},
            {size.x / win.x, size.y / win.y}
        ));
    }


    void draw(sf::RenderWindow& window) const {
        window.setView(view);

        sf::RectangleShape bg(VIEW_SIZE);
        bg.setPosition({0.f, 0.f});
        bg.setFillColor(sf::Color(20, 20, 30));
        window.draw(bg);

        window.setView(window.getDefaultView());

    }



    float getBlockSize() {
        float size;
        return size;
    }

// friend class BlockFactory;
friend class GameWindow;

};

void GameWindow::run(Grid& grid) {
        while (window.isOpen()) {
            while (const auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) 
                    window.close();
                
            }
            window.clear();
            grid.draw(window);
            window.display();
        }
    }


// class Block : public sf::Shape::Drawable {
//     private:
//         sf::RectangleShape rectangle;
//         inline static std::array<sf::Color, 7> colors = {
//             sf::Color::Cyan,
//             sf::Color::Yellow,
//             sf::Color(128, 0, 128),
//             sf::Color::Green,
//             sf::Color::Red,
//             sf::Color::Blue,
//             sf::Color(255, 165, 0)
//         };
//         // <> blockColor = 
//         explicit Block( sf::Vector2f position,
//                         sf::Color color,
//                         float size
//                         )
//                         : rectangle({size, size}) 
//                         {
//                             rectangle.setPosition(position);
//                             rectangle.setSize({size, size});
//                             rectangle.setFillColor(color);
//                         }

//     public:
//         const void color(int x) {
//             switch(x) {
//                 case 0 ... 6:
//                     rectangle.setFillColor(colors[x]);
//                     break;
//                 default:
//                     std::cout << "Number for color has to be between (0 and 6)  inc." << std::endl;
//                     break;
//             }
//         }

//         // void draw() {
            
//         //     // Block

//         //     return 
//         // }
//     friend class BlockFactory;
        
// };

// class BlockFactory {
//     private:

//     public:
//         static Block createBlock(   sf::Vector2f position,
//                                     float size,
//                                     sf::Color color) {
//             return Block(position, color, size);

//         }
//     friend class Grid;

// };

class Score {

};

int main() {
    Grid grid;
    GameWindow::getInstance().run(grid);
    // grid.draw(GameWindow::getInstance())
    
    // grid.draw(GameWindow::get());
    // game.run();

    // sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML window");
    // const sf::Texture texture("background-image.jpg");
    // sf::Sprite sprite(texture);
    // const sf::Font font("ARIAL.TTF");
    // sf::Text text(font, "Hello SFML", 50);
    // sf::Music music("nice_music.mp3");

    // music.play();

    // while (window.isOpen()) {
        
        // }
        // window.clear();
        // window.draw(sprite);
        // window.draw(text);
        // window.display();
        
    return 0;
}