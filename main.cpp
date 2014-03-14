#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>


typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;


class GameObject
{
    public:
        GameObject(sf::Shape* shape) : velX(0.0f), velY(0.0f), speed(200.0f), shape(shape) { }

        virtual ~GameObject()
        {
            delete shape;
        }

        float velX, velY; // Object velocity.
        float speed; // Maximum speed object may move at.
        sf::Shape* shape; // Abstract Shape.
};


class Player : public GameObject
{
    public:
        Player(sf::Shape* shape) : GameObject(shape) { }
};


int main(int argc, char** args)
{
    // Initialize variables. I like to do this at the top, so at a glance I can
    // see exactly what's in scope.
    const unsigned int SCREEN_WIDTH = 768; // Screen width.
    const unsigned int SCREEN_HEIGHT = 480; // Screen height.
    bool quit = false; // If set to true, game will quit immediately.
    sf::RenderWindow gameWindow; // Game window that shows up on the screen.
    sf::Event event; // Event recieved by SFML popping from the event queue.
    std::vector<GameObject*> gameObjects; // Maintain a list of game objects. 
    sf::RectangleShape* playerShape; // Shape used to render the player.
    Player* player; // Player object that the user controls.
    const unsigned int FPS = 60;
    sf::Clock clock;

    // Initialize 768x480 SFML game window with 32 bit pixel depth.
    gameWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32), "I am a game window.");
    gameWindow.setFramerateLimit(FPS); // Yep, it's that easy.

    // Initialize game objects.
    // Player represented by a 50x50px rectangle at location (0,0) with a 5px
    // outline.
    const int PL_SIZE_X = 50;
    const int PL_SIZE_Y = 50;
    const int PL_OUTLINE_THICKNESS = 5;
    playerShape = new sf::RectangleShape;
    playerShape->setSize(sf::Vector2f(50, 50));
    playerShape->setOutlineColor(sf::Color::Black);
    playerShape->setOutlineThickness(PL_OUTLINE_THICKNESS);
    playerShape->setPosition(
            SCREEN_WIDTH / 2 - PL_SIZE_X, // Center player horizontally.
            SCREEN_HEIGHT - PL_SIZE_Y - PL_OUTLINE_THICKNESS // Put player on bottom of the screen.
    );
    player = new Player(playerShape);
    gameObjects.push_back(player);

    // GAME LOOP
    while (!quit)
    {
        // Get change in time since last frame.
        float dt = clock.restart().asSeconds();

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

                // If the window is resized, resize the view, otherwise the
                // rendering will be distorted.
                case sf::Event::Resized:
                    gameWindow.setView(sf::View(sf::FloatRect(
                        0.0f,
                        0.0f,
                        gameWindow.getSize().x,
                        gameWindow.getSize().y))
                    );
                    break;
            } 
        }
        // Check what keys are being pressed in real-time.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        {
            std::cout << "X key pressed." << std::endl;
        }
        // Movement.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            player->velY -= player->speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            player->velY += player->speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            player->velX -= player->speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player->velX += player->speed;
        // Jump.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
        }

        // Using an iterator means we can store game objects in any STL
        // container down the road.
        std::vector<GameObject*>::iterator gameObjectIt;
        for (gameObjectIt = gameObjects.begin(); gameObjectIt != gameObjects.end(); gameObjectIt++)
        {
            // Just a convention, so I don't have to do (*gameObjectIt)->update()
            GameObject* gameObject = *gameObjectIt; 

            // STATE UPDATES/CALCULATIONS
            // Any updates to an object's state goes here. This is also called
            // integration.
            gameObject->shape->move(gameObject->velX * dt, gameObject->velY * dt);
            gameObject->velX = 0;
            gameObject->velY = 0;

            // RENDER
            // This draws an object to the game window, but remember, it does not
            // actually DISPLAY it on the screen, that's what
            // sf::RenderWindow::display() is for.
            gameWindow.draw(*(gameObject->shape));
        }

        // Display on screen what has been rendered to the window.
        gameWindow.display();
        // Set window color to white.
        gameWindow.clear(sf::Color::White);
    }

    return 0;
}
