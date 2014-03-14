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

        /**
         * Any updates to an object's state go here. This is also called
         * integration.
         */
        virtual void update(float dt)
        {
            shape->move(velX * dt, velY * dt);
            velX = 0;
            velY = 0;
        }

        /**
         * This draws an object to the game window, but remember, it does not
         * actually DISPLAY it on the screen, that's what
         * sf::RenderWindow::display() is for.
         */
        virtual void render(sf::RenderWindow& gameWindow)
        {
            gameWindow.draw(*shape);
        }

        void move(Direction direction)
        {
            switch (direction)
            {
                case UP:
                    velY -= speed;
                    break;
                case DOWN:
                    velY += speed;
                    break;
                case LEFT:
                    velX -= speed;
                    break;
                case RIGHT:
                    velX += speed;
                    break;
            }
        }

    protected:
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
    bool quit = false; // If set to true, game will quit immediately.
    sf::RenderWindow gameWindow; // Game window that shows up on the screen.
    sf::Event event; // Event recieved by SFML popping from the event queue.
    std::vector<GameObject*> gameObjects; // Maintain a list of game objects. 
    sf::RectangleShape* playerShape; // Shape used to render the player.
    Player* player; // Player object that the user controls.
    const unsigned int FPS = 60;
    sf::Clock clock;

    // Initialize 1024x768 SFML game window with 32 bit pixel depth.
    gameWindow.create(sf::VideoMode(768, 480, 32), "I am a game window.");
    gameWindow.setFramerateLimit(FPS); // Yep, it's that easy.

    // Initialize game objects.
    // Player represented by a 50x50px rectangle at location (0,0) with a 5px
    // outline.
    playerShape = new sf::RectangleShape;
    playerShape->setPosition(50, 50);
    playerShape->setSize(sf::Vector2f(50, 50));
    playerShape->setOutlineColor(sf::Color::Black);
    playerShape->setOutlineThickness(5);
    player = new Player(
        playerShape
    );
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
            } 
        }
        // Check what keys are being pressed in real-time.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        {
            std::cout << "X key pressed." << std::endl;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            player->move(UP);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            player->move(DOWN);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            player->move(LEFT);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player->move(RIGHT);

        // Using an iterator means we can store game objects in any STL
        // container down the road.
        std::vector<GameObject*>::iterator gameObjectIt;
        for (gameObjectIt = gameObjects.begin(); gameObjectIt != gameObjects.end(); gameObjectIt++)
        {
            // Just a convention, so I don't have to do (*gameObjectIt)->update()
            GameObject* gameObject = *gameObjectIt; 

            // STATE UPDATES/CALCULATIONS
            gameObject->update(dt); // Update object state.

            // RENDER
            gameObject->render(gameWindow); // Render object to window.
        }

        // Display on screen what has been rendered to the window.
        gameWindow.display();
        // Set window color to white.
        gameWindow.clear(sf::Color::White);
    }

    return 0;
}
