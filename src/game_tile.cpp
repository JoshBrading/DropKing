#include "game_tile.h"

GameTile::GameTile(const Vector3 position, const Vector3 rotation): Entity(position, rotation, "assets/GroundTile.obj", "Game Tile")
{
}

void GameTile::update()
{
    Entity::update();
}


void GameTile::draw()
{
    Entity::draw();
}
