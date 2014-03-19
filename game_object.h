#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class GameObjectManager;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;


class GameObject
{
    public:
        GameObject(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager);
        virtual ~GameObject();
        virtual void update(float dt) = 0;
        virtual void render();
        virtual void destroy();
        virtual void isDestroyed();
        sf::RenderWindow* gameWindow;
        GameObjectManager* gameObjectManager;
        float velX, velY; // Object velocity.
        float maxSpeed; // Maximum speed object may move at.
        sf::Shape* shape; // Abstract Shape.
        bool destroyed; // Flag for delayed deletion.
};


class PlayerBullet : public GameObject
{
    public:
        PlayerBullet(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager);
        virtual ~PlayerBullet();
        virtual void update(float dt);
};


class Player : public GameObject
{
    public:
        Player(sf::RenderWindow& gameWindow, GameObjectManager& gameObjectManager);
        virtual ~Player();
        virtual void update(float dt);
        void move(Direction direction);
        void shoot(float x, float y);
};

#endif /* GAME_OBJECT_H */
