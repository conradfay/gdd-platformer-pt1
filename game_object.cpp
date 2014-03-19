#include "game_object.h"
#include <iostream>
#include <math.h>

GameObject::GameObject(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager) : 
    gameWindow(&gameWindow),
    gameObjectManager(&gameObjectManager),
    velX(0.0f), 
    velY(0.0f),
    maxSpeed(100.0f),
    destroyed(false)
{

}

GameObject::~GameObject()
{
    delete shape; 
}

void GameObject::render()
{
    gameWindow->draw(*shape);
}

void GameObject::destroy()
{
    // Object to be deleted externally later.
    destroyed = true; 
}

PlayerBullet::PlayerBullet(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager) :
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

PlayerBullet::~PlayerBullet()
{

}

void PlayerBullet::update(float dt)
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



Player::Player(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager) :
    GameObject(gameWindow, gameObjectManager)
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

Player::~Player()
{

}

void Player::update(float dt)
{
    shape->move(velX * dt, velY * dt);
    velX = 0;
    velY = 0;
}

// Move in specified direction.
void Player::move(Direction direction)
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
void Player::shoot(float x, float y)
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
    gameObjectManager->add(*playerBullet);
}
