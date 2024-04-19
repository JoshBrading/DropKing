#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"

class Projectile {
public:
    Projectile(Vector3 start_pos, Vector3 vel, const Model& model);

    void update();
    void draw() const;

private:
    Vector3 position_;
    Vector3 velocity_;
    Model projectile_model_;
};

#endif // PROJECTILE_H
