#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"

class Projectile {
public:
    Projectile(Vector3 start_position, Vector3 velocity, const Model& model);

    void update();
    void draw() const;

private:
    Vector3 position;
    Vector3 velocity;
    Model projectile_model;
};

#endif // PROJECTILE_H
