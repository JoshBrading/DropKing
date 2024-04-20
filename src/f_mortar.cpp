#include "f_mortar.h"

FMortar::FMortar(const Vector3 position): Entity(position, "assets/projectile.obj", "Mortar")
{
}

void FMortar::draw()
{
    Entity::draw();
}

void FMortar::update()
{
    Entity::update();
}

