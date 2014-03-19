#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H

#include "game_object.h"
#include <vector>

class GameObjectManager
{
    public:
        GameObjectManager();
        ~GameObjectManager();

        void add(GameObject& gameObject);

        void remove(std::vector<GameObject*>::iterator it);

        std::vector<GameObject*>& getAll();

    private:
        std::vector<GameObject*> gameObjects;
};

#endif /* GAME_OBJECT_MANAGER_H */
