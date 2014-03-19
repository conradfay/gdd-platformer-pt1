#ifndef HELPERS_H
#define HELPERS_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

/**
 * Map a pair of coordinates from anywhere on the screen to coordinates
 * that lie on the radius of the object (returns in world coordinates).
 */
sf::Vector2f mapToRadius(GameObject& gameObject, float x, float y, float radiusSize);

#endif /* HELPERS_H */
