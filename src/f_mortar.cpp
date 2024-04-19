#include "f_mortar.h"

FMortar::FMortar(const Vector3 position): Entity(position, "assets/projectile.obj", "Mortar")
{
}

void FMortar::update()
{
    Entity::update();
    DrawText("Mortar Update Function", 199, 299, 10, BLUE);
}

