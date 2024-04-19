#include "Projectile.h"
#include <raymath.h>

Projectile::Projectile(const Vector3 start_pos, const Vector3 vel, const Model& model)
{
    position_ = start_pos;
    velocity_ = vel;
    projectile_model_ = model;
}

void Projectile::update() {
    // Set position
    velocity_.y -= 1.8f * GetFrameTime();
    position_.x += velocity_.x * GetFrameTime();
    position_.y += velocity_.y * GetFrameTime();
    position_.z += velocity_.z * GetFrameTime();

    // Set rotation
    const Vector3 direction = Vector3Normalize(velocity_);
    Vector3 up = {0.0f, 1.0f, 0.0f};

    // Needs more research..
    up = Vector3Normalize(Vector3Subtract(up, Vector3Scale(direction, Vector3DotProduct(up, direction))));
    Vector3 right = Vector3CrossProduct(up, direction);
    right = Vector3Normalize(right);
    up = Vector3CrossProduct(direction, right);
    
    const Matrix rotation_matrix = {
        right.x, up.x, direction.x, 0.0f,
        right.y, up.y, direction.y, 0.0f,
        right.z, up.z, direction.z, 0.0f,
        0.0f,    0.0f, 0.0f,        1.0f
    };

    projectile_model_.transform = MatrixMultiply(rotation_matrix, MatrixTranslate(0,0,0));
}

void Projectile::draw() const
{
    DrawModel(projectile_model_, position_, 1.0f, WHITE);
}
