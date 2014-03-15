#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>


typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;


typedef enum {
    PLAYER,
    TURRET,
    BULLET
} GameObjectType;


class GameObject
{
    public:
        GameObject(sf::Shape* shape) : velX(0.0f), velY(0.0f), maxSpeed(200.0f), shape(shape) { }

        virtual ~GameObject()
        {
            delete shape;
        }

        virtual GameObjectType getType() = 0;

        float velX, velY; // Object velocity.
        float maxSpeed; // Maximum maxSpeed object may move at.
        sf::Shape* shape; // Abstract Shape.
};


class Player : public GameObject
{
    public:
        Player(sf::Shape* shape) : GameObject(shape) { }

        virtual GameObjectType getType()
        {
            return PLAYER; 
        }
};

class Turret : public GameObject
{
    public:
        Turret(sf::Shape* shape) : GameObject(shape) { }

        virtual GameObjectType getType()
        {
            return TURRET; 
        }
};

class Bullet : public GameObject
{
    public:
        Bullet(sf::Shape* shape) : GameObject(shape)
        {
            maxSpeed = 10.0f;
        }

        virtual GameObjectType getType()
        {
            return BULLET; 
        }

        float dmg; // Bullet damage.
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
    std::vector<GameObject*> gameObjects; // Maintain a list of game objects.
    // Player represented by a 50x50px rectangle at location (0,0) with a 5px
    // outline.
    sf::RectangleShape* playerShape; // Shape used to render the player.
    const int PL_SIZE_X = 50;
    const int PL_SIZE_Y = 50;
    const int PL_OUTLINE_THICKNESS = 5;
    playerShape = new sf::RectangleShape;
    playerShape->setSize(sf::Vector2f(PL_SIZE_X, PL_SIZE_Y));
    playerShape->setOutlineColor(sf::Color::Black);
    playerShape->setOutlineThickness(PL_OUTLINE_THICKNESS);
    // Place player at the center of the screen.
    playerShape->setPosition(
            SCREEN_WIDTH / 2 - PL_SIZE_X,
            SCREEN_HEIGHT / 2 - PL_SIZE_Y
    );
    // Set player's origin to its centroid instead of (0,0).
    playerShape->setOrigin(PL_SIZE_X / 2, PL_SIZE_Y / 2);
    Player* player; // Player object that the user controls.
    player = new Player(playerShape);
    gameObjects.push_back(player);


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
                            std::cout << "pew!" << std::endl; 

                            sf::CircleShape* bulletShape;
                            const int BT_RADIUS = 5;
                            const int PL_OUTLINE_THICKNESS = 5;
                            bulletShape = new sf::CircleShape;
                            bulletShape->setRadius(BT_RADIUS);
                            bulletShape->setOutlineColor(sf::Color::Black);
                            bulletShape->setOutlineThickness(PL_OUTLINE_THICKNESS);
                            sf::Vector2f bulletPos = mapToRadius(
                                    *player,
                                    event.mouseButton.x,
                                    event.mouseButton.y,
                                    player->shape->getLocalBounds().width
                            );
                            bulletShape->setPosition(bulletPos.x, bulletPos.y);
                            bulletShape->setOrigin(BT_RADIUS, BT_RADIUS);
                            Bullet* bullet = new Bullet(bulletShape);
                            bullet->velX = (bulletPos.x - player->shape->getPosition().x) * bullet->maxSpeed;
                            bullet->velY = (bulletPos.y - player->shape->getPosition().y) * bullet->maxSpeed;
                            gameObjects.push_back(bullet);

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
            player->velY -= player->maxSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player->velY += player->maxSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            player->velX -= player->maxSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            player->velX += player->maxSpeed;

        // STATE UPDATES/CALCULATIONS
        // Any updates to an object's state goes here. This is also called
        // integration.
        for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); it++)
        {
            GameObject& gameObject = **it;
            sf::Shape& shape = *gameObject.shape;
            // We will have different calculations to make depending on what
            // kind of object it is.
            switch (gameObject.getType())
            {
                case PLAYER:
                    shape.move(gameObject.velX * dt, gameObject.velY * dt);
                    gameObject.velX = 0;
                    gameObject.velY = 0;
                    break;
                case BULLET:
                    shape.move(gameObject.velX * dt, gameObject.velY * dt);
                    // If the bullet goes offscreen, remove it from gameObjects
                    // and delete it from memory.
                    if (shape.getPosition().x < 0 || shape.getPosition().x > SCREEN_WIDTH || shape.getPosition().y < 0 || shape.getPosition().y > SCREEN_HEIGHT)
                    {
                        delete &gameObject;
                        gameObjects.erase(it);
                        // Since the size of the vector was reduced by one,
                        // this relocates the iterator to the correct position.
                        it--; 
                    }
                    break;
            }
        }

        // RENDER
        // This draws an object to the game window, but remember, it does not
        // actually DISPLAY it on the screen, that's what
        // sf::RenderWindow::display() is for.
        for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); it++)
        {
            GameObject& gameObject = **it;
            gameWindow.draw(*gameObject.shape);
        }

        // Display on screen what has been rendered to the window.
        gameWindow.display();
        // Set window color to white.
        gameWindow.clear(sf::Color::White);
    }

    return 0;
}
