#ifndef GAME_TILE_H
#define GAME_TILE_H

#include "entity.h"

class GameTile final : public Entity
{
public:
    explicit GameTile(const Vector3 position, const Vector3 rotation);
    virtual void draw() override;
    virtual void update() override;

    bool set_child(Entity* entity); // True on success, false if tile is occupied
};

#endif // GAME_TILE_H
