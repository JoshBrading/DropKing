#include "f_mortar.h"

f_mortar::f_mortar(const Vector3 position): entity(position, "assets/projectile.obj", "Mortar")
{
}

void f_mortar::update()
{
    entity::update();
    DrawText("Mortar Update Function", 199, 299, 10, BLUE);
}

