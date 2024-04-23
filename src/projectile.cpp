#include "Projectile.h"
#include <raymath.h>

Projectile::Projectile(const Vector3 start_position, const Vector3 velocity, const Model& model)
{
    this->position = start_position;
    this->velocity = velocity;
    this->projectile_model = model;
}

void Projectile::update() {
    // Set position
    velocity.y -= 1.8f * GetFrameTime();
    position.x += velocity.x * GetFrameTime();
    position.y += velocity.y * GetFrameTime();
    position.z += velocity.z * GetFrameTime();

    // Set rotation
    const Vector3 direction = Vector3Normalize(velocity);
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

    projectile_model.transform = MatrixMultiply(rotation_matrix, MatrixTranslate(0,0,0));
}

void Projectile::draw() const
{
    DrawModel(projectile_model, position, 1.0f, WHITE);
}
