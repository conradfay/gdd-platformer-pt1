#include "game_object_manager.h"
#include "game_object.h"

GameObjectManager::GameObjectManager() { }

GameObjectManager::~GameObjectManager() { }

void GameObjectManager::add(GameObject& gameObject)
{
    gameObjects.push_back(&gameObject); 
}

void GameObjectManager::remove(std::vector<GameObject*>::iterator it)
{
    gameObjects.erase(it); 
    delete *it;
}

std::vector<GameObject*>& GameObjectManager::getAll()
{
    return gameObjects;
}
