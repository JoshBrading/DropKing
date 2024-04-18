#include "Projectile.h"
#include <raymath.h>

projectile::projectile(const Vector3 start_pos, const Vector3 vel, const Model& model)
    : position_(start_pos), velocity_(vel), projectile_model_(model) {
}
void projectile::update() {
    // Set position
    velocity_.y -= 1.8f * GetFrameTime();
    position_.x += velocity_.x * GetFrameTime();
    position_.y += velocity_.y * GetFrameTime();
    position_.z += velocity_.z * GetFrameTime();

    // Set rotation
    const Vector3 direction = Vector3Normalize(velocity_);
    Vector3 up = {0.0f, 1.0f, 0.0f}; // Y is up, whyyyy :[

    // Needs more research..
    up = Vector3Normalize(Vector3Subtract(up, Vector3Scale(direction, Vector3DotProduct(up, direction))));
    Vector3 right = Vector3CrossProduct(up, direction);
    right = Vector3Normalize(right);
    up = Vector3CrossProduct(direction, right);
    
    // I hate quaternions
    const Matrix rotation_matrix = {
        right.x, up.x, direction.x, 0.0f,
        right.y, up.y, direction.y, 0.0f,
        right.z, up.z, direction.z, 0.0f,
        0.0f,    0.0f, 0.0f,        1.0f
    };

    projectile_model_.transform = MatrixMultiply(rotation_matrix, MatrixTranslate(0,0,0)); // Note to self: This is 0,0,0 because the local position should not change.
}

void projectile::draw() const
{
    DrawModel(projectile_model_, position_, 1.0f, WHITE);
}
