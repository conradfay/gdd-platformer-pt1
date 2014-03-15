#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>


typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;


class GameObject
{
    public:
        GameObject() : velX(0.0f), velY(0.0f), maxSpeed(100.0f) { }

        virtual ~GameObject()
        {
            delete shape; 
        }

        float velX, velY; // Object velocity.
        float maxSpeed; // Maximum maxSpeed object may move at.
        sf::Shape* shape; // Abstract Shape.
};


class Player : public GameObject
{
    public:
        Player() : GameObject()
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
};


class Bullet : public GameObject
{
    public:
        Bullet() : GameObject()
        {
            maxSpeed = 10.0f;
            const int BT_RADIUS = 5;
            const int PL_OUTLINE_THICKNESS = 5;
            sf::CircleShape* bulletShape = new sf::CircleShape;
            bulletShape->setRadius(BT_RADIUS);
            bulletShape->setOutlineColor(sf::Color::Black);
            bulletShape->setOutlineThickness(PL_OUTLINE_THICKNESS);
            bulletShape->setOrigin(BT_RADIUS, BT_RADIUS);
            shape = bulletShape;
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
    // Create player at center of the screen.
    Player player;
    player.shape->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    // Maintain lists of game objects.
    std::vector<Bullet*> bullets;

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
                            // Create bullet on the radius of the player.
                            Bullet* bullet = new Bullet;
                            sf::Vector2f bulletPos = mapToRadius(
                                player,
                                event.mouseButton.x,
                                event.mouseButton.y,
                                player.shape->getLocalBounds().width
                            );
                            bullet->shape->setPosition(bulletPos);
                            // Its velocity is a multiple of the vector from the
                            // center of the player to the bullet position.
                            bullet->velX = (bulletPos.x - player.shape->getPosition().x) * bullet->maxSpeed;
                            bullet->velY = (bulletPos.y - player.shape->getPosition().y) * bullet->maxSpeed;
                            bullets.push_back(bullet);
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
            player.velY -= player.maxSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player.velY += player.maxSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            player.velX -= player.maxSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            player.velX += player.maxSpeed;

        // STATE UPDATES/CALCULATIONS
        // Any updates to an object's state goes here. This is also called
        // integration.
        player.shape->move(player.velX * dt, player.velY * dt);
        player.velX = 0;
        player.velY = 0;
        for (std::vector<Bullet*>::iterator it = bullets.begin(); it != bullets.end(); it++)
        {
            Bullet& bullet = **it;
            sf::Shape& shape = *bullet.shape;
            // We will have different calculations to make depending on what
            // kind of object it is.
            shape.move(bullet.velX * dt, bullet.velY * dt);
            // If the bullet goes offscreen, remove it from gameObjects
            // and delete it from memory.
            if (shape.getPosition().x < 0 || shape.getPosition().x > SCREEN_WIDTH || 
                shape.getPosition().y < 0 || shape.getPosition().y > SCREEN_HEIGHT)
            {
                delete &bullet;
                bullets.erase(it);
                // Since the size of the vector was reduced by one,
                // this relocates the iterator to the correct position.
                it--; 
            }
        }

        // RENDER
        // This draws an object to the game window, but remember, it does not
        // actually DISPLAY it on the screen, that's what
        // sf::RenderWindow::display() is for.
        gameWindow.draw(*player.shape);
        for (std::vector<Bullet*>::iterator it = bullets.begin(); it != bullets.end(); it++)
        {
            Bullet& bullet = **it;
            gameWindow.draw(*bullet.shape);
        }

        // Display on screen what has been rendered to the window.
        gameWindow.display();
        // Set window color to white.
        gameWindow.clear(sf::Color::White);
    }

    return 0;
}
