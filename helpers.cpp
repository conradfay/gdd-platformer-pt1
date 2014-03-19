#include "helpers.h"
#include "game_object.h"
#include <math.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

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


