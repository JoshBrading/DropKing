#ifndef COLLISION_H
#define COLLISION_H

#include <raylib.h>

bool ray_intersects_plane(const Ray& ray, const Vector3 plane_point, const Vector3 plane_normal, float* out_distance);

#endif // COLLISION_H
