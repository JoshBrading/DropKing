#include <raylib.h>
#include <raymath.h>
#include <cmath>

bool ray_intersects_plane(const Ray& ray, const Vector3 plane_point, const Vector3 plane_normal, float* out_distance)
{
    // Check if the ray and the plane are parallel
    const float dot = Vector3DotProduct(ray.direction, plane_normal);
    if (std::fabs(dot) < 0.0001f) // If almost parallel
        return false; // No intersection, the ray is parallel to the plane

    // Calculate the distance along the ray where it intersects the plane
    const float t = Vector3DotProduct(Vector3Subtract(plane_point, ray.position), plane_normal) / dot;

    // Check if the intersection point is behind the ray's origin
    if (t < 0)
        return false; // No intersection, intersection point is behind the ray's origin

    // Store the distance if the caller requested it
    if (out_distance != nullptr)
        *out_distance = t;

    return true; // Intersection found
}
