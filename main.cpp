#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main(int argc, char** args)
{
    // Initialize variables.
    bool quit = false; // If set to true, game will quit immediately.
    sf::RenderWindow gameWindow; // Game window that shows up on the screen.
    sf::Event event; // Event recieved by SFML popping from the event queue.

    // Initialize 1024x768 SFML game window with 32 bit pixel depth.
    gameWindow.create(sf::VideoMode(768, 480, 32), "I am a game window.");

    // GAME LOOP
    while (!quit)
    {
        // HANDLE INPUT
        // Poll input until nothing remains on the queue.
        while (gameWindow.pollEvent(event))
        {
            switch (event.type)
            {
                // If SFML recieves the close window event, quit the game.
                case sf::Event::Closed:
                    std::cout << "Recieved event to close window." << std::endl;
                    quit = true;
                    break;
                
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        // If SFML recieves the event that the "Q" key was
                        // pressed, quit the game.
                        case sf::Keyboard::Q:
                            std::cout << "Recieved event that Q key was pressed."
                                << std::endl;
                            quit = true; 
                            break;
                        case sf::Keyboard::Z:
                            std::cout << "Recieved event that Z key was pressed."
                                << std::endl;
                            break;

                    }
                    break;
            } 
        }
        // Check what keys are being pressed in real-time.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        {
            std::cout << "X key pressed." << std::endl;
        }

        // CALCULATIONS

        // RENDER
        // Set window color to white.
        gameWindow.clear(sf::Color(255, 255, 255));
        // Display on screen what has been rendered to the window.
        gameWindow.display();
    }

    return 0;
}
