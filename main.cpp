#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include <algorithm>

typedef unsigned int GameObjectHandle;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef enum {
    PLAYER,
    PLAYER_BULLET,
    ENEMY,
    ENEMY_BULLET
} ObjectID;

class GameObject;
class PlayerBullet;
class Player;

typedef std::vector<GameObject*> GameObjectManager;

class GameObject
{
    public:
        GameObject(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager) : 
            gameWindow(&gameWindow), gameObjectManager(&gameObjectManager), velX(0.0f), 
            velY(0.0f), maxSpeed(100.0f), destroyed(false) { }

        virtual ~GameObject()
        {
            delete shape; 
        }

        virtual ObjectID getID() = 0;

        virtual void update(float dt) = 0;
        
        virtual void render()
        {
            gameWindow->draw(*shape);
        }

        virtual void destroy()
        {
            // Object to be deleted externally later.
            destroyed = true; 
        }

        sf::RenderWindow* gameWindow;
        GameObjectManager* gameObjectManager;
        float velX, velY; // Object velocity.
        float maxSpeed; // Maximum speed object may move at.
        sf::Shape* shape; // Abstract Shape.
        bool destroyed; // Flag for delayed deletion.
        int collisionLayer;
        std::vector<std::string> tags;
};

class PlayerBullet : public GameObject
{
    public:
        PlayerBullet(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager) :
            GameObject(gameWindow, gameObjectManager)
        {
            maxSpeed = 10.0f;
            const int BT_RADIUS = 5;
            const int BT_OUTLINE_THICKNESS = 5;
            sf::CircleShape* playerBulletShape = new sf::CircleShape;
            playerBulletShape->setRadius(BT_RADIUS);
            playerBulletShape->setOutlineColor(sf::Color::Black);
            playerBulletShape->setOutlineThickness(BT_OUTLINE_THICKNESS);
            playerBulletShape->setOrigin(BT_RADIUS, BT_RADIUS);
            shape = playerBulletShape;
        }

        virtual ~PlayerBullet() { }

        virtual ObjectID getID()
        {
            return PLAYER_BULLET; 
        }

        virtual void update(float dt)
        {
            shape->move(velX * dt, velY * dt);
            // If the playerBullet goes offscreen, remove it from gameObjectManager
            // and delete it from memory.
            if (shape->getPosition().x < 0 || shape->getPosition().x > gameWindow->getSize().x || 
                shape->getPosition().y < 0 || shape->getPosition().y > gameWindow->getSize().y)
            {
                destroy();
            }
        }
};



class Player : public GameObject
{
    public:
        Player(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager) : GameObject(gameWindow, gameObjectManager)
        {
            // Player represented by a 20x20 rectangle with a 5px outline.
            const int PL_SIZE_X = 20;
            const int PL_SIZE_Y = 20;
            const int PL_OUTLINE_THICKNESS = 5;
            // Shape used to render the player.
            sf::RectangleShape* playerShape = new sf::RectangleShape;
            playerShape->setSize(sf::Vector2f(PL_SIZE_X, PL_SIZE_Y));
            playerShape->setOutlineColor(sf::Color::Black);
            playerShape->setOutlineThickness(PL_OUTLINE_THICKNESS);
            // Set player's local origin to its centroid instead of (0,0).
            playerShape->setOrigin(PL_SIZE_X / 2, PL_SIZE_Y / 2);
            shape = playerShape;
        }

        virtual ~Player()
        {
        }

        virtual ObjectID getID()
        {
            return PLAYER; 
        }

        virtual void update(float dt)
        {
            shape->move(velX * dt, velY * dt);
            velX = 0;
            velY = 0;
        }

        // Move in specified direction.
        void move(Direction direction)
        {
            switch (direction)
            {
                case UP:
                    velY -= maxSpeed;
                    break;
                case DOWN:
                    velY += maxSpeed;
                    break;
                case LEFT:
                    velX -= maxSpeed;
                    break;
                case RIGHT:
                    velX += maxSpeed;
                    break;
            } 
        }

        // Shoot towards specified coordinates.
        void shoot(float x, float y)
        {
            std::cout << "pew!" << std::endl; 
            // Create playerBullet on the radius of the player.
            PlayerBullet* playerBullet = new PlayerBullet(*gameWindow, *gameObjectManager);

            // Get center coordinate of object.
            sf::Vector2f objectCenter = shape->getPosition();
            // Vector from objects center to the input coordinates.
            sf::Vector2f inVector(x - objectCenter.x, y - objectCenter.y);  
            // Calculate normalized vector.
            float magnitude = sqrt(pow(inVector.x, 2) + pow(inVector.y, 2));
            sf::Vector2f playerBulletPos = sf::Vector2f(inVector / magnitude) * shape->getLocalBounds().width + objectCenter;

            playerBullet->shape->setPosition(playerBulletPos);
            // Its velocity is a multiple of the vector from the
            // center of the player to the playerBullet position.
            playerBullet->velX = (playerBulletPos.x - shape->getPosition().x) * playerBullet->maxSpeed;
            playerBullet->velY = (playerBulletPos.y - shape->getPosition().y) * playerBullet->maxSpeed;
            gameObjectManager->push_back(playerBullet);
        }
};


class Enemy : public GameObject
{
    public:
        Enemy(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager) : GameObject(gameWindow, gameObjectManager)
        {
            // Player represented by a 20x20 rectangle with a 5px outline.
            const int PL_SIZE_X = 20;
            const int PL_SIZE_Y = 20;
            const int PL_OUTLINE_THICKNESS = 5;
            // Shape used to render the player.
            sf::RectangleShape* playerShape = new sf::RectangleShape;
            playerShape->setSize(sf::Vector2f(PL_SIZE_X, PL_SIZE_Y));
            playerShape->setOutlineColor(sf::Color::Red);
            playerShape->setOutlineThickness(PL_OUTLINE_THICKNESS);
            // Set player's local origin to its centroid instead of (0,0).
            playerShape->setOrigin(PL_SIZE_X / 2, PL_SIZE_Y / 2);
            shape = playerShape;
        }

        virtual ~Enemy()
        {
        }

        virtual ObjectID getID()
        {
            return ENEMY; 
        }

        virtual void update(float dt)
        {
            shape->move(velX * dt, velY * dt);
            velX = 0;
            velY = 0;
        }
};


/**
 * Map a pair of coordinates from anywhere on the screen to coordinates
 * that lie on the radius of the object (returns in world coordinates).
 */
sf::Vector2f mapToRadius(GameObject& gameObject, float x, float y, float radiusSize)
{
    // Get center coordinate of object.
    sf::Vector2f objectCenter = gameObject.shape->getPosition();
    // Vector from objects center to the input coordinates.
    sf::Vector2f inVector(x - objectCenter.x, y - objectCenter.y);  
    // Calculate normalized vector.
    float magnitude = sqrt(pow(inVector.x, 2) + pow(inVector.y, 2));
    sf::Vector2f outVector = sf::Vector2f(inVector / magnitude) * radiusSize + objectCenter;
    return outVector;
}


int main(int argc, char** args)
{
    // Initialize variables. I like to do this at the top, so at a glance I can
    // see exactly what's in scope.
    const unsigned int SCREEN_WIDTH = 768; // Screen width.
    const unsigned int SCREEN_HEIGHT = 480; // Screen height.
    const unsigned int FPS = 60;

    // Initialize 768x480 SFML game window with 32 bit pixel depth.
    sf::RenderWindow gameWindow; // Game window that shows up on the screen.
    gameWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32), "I am a game window.");
    gameWindow.setFramerateLimit(FPS); // Yep, it's that easy.

    // Initialize game objects.
    GameObjectManager gameObjectManager;
    // Create player at center of the screen.
    Player player(gameWindow, gameObjectManager);
    player.shape->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    gameObjectManager.push_back(&player);

    // GAME LOOP
    bool quit = false; // If set to true, game will quit immediately.
    sf::Clock clock; // Used to maintain fps.
    while (!quit)
    {
        // Get change in time since last frame.
        float dt = clock.restart().asSeconds();

        // HANDLE INPUT
        // Poll input until nothing remains on the queue.
        sf::Event event; // Event recieved by SFML popping from the event queue.
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

                // Register as an event since we want to shoot once when the
                // mouse is pressed.
                case sf::Event::MouseButtonPressed:
                    switch (event.mouseButton.button)
                    {
                        case sf::Mouse::Left:
                            player.shoot(event.mouseButton.x, event.mouseButton.y);
                            break;
                    }

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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            player.move(UP);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player.move(DOWN);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            player.move(LEFT);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            player.move(RIGHT);

        // STATE UPDATES/CALCULATIONS
        // Any updates to an object's state goes here. This is also called
        // integration.
        int count = 0;
        for (std::vector<GameObject*>::iterator it = gameObjectManager.begin(); it != gameObjectManager.end(); it++)
        {
            GameObject& gameObject = **it;
            gameObject.update(dt);
            if (gameObject.destroyed) {
                gameObjectManager.erase(it);
                it--;
            }
        }

        // RENDER
        // This draws an object to the game window, but remember, it does not
        // actually DISPLAY it on the screen, that's what
        // sf::RenderWindow::display() is for.
        for (std::vector<GameObject*>::iterator it = gameObjectManager.begin(); it != gameObjectManager.end(); it++)
        {
            GameObject& gameObject = **it;
            gameObject.render();
        }

        // Display on screen what has been rendered to the window.
        gameWindow.display();
        // Set window color to white.
        gameWindow.clear(sf::Color::White);
    }

    return 0;
}
