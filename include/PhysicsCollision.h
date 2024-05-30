#pragma once
#include <raylib.h>
#include <vector>

#include "PhysicsBody.h"

namespace Physics
{
    struct CollisionData
    {
        Body* body_a;
        Body* body_b;
        Vector2 normal;
        float depth;
        Vector2 contact1;
        Vector2 contact2;
        int contact_count;

        CollisionData(Body* body_a, Body* body_b, Vector2 normal, float depth, Vector2 contact1, Vector2 contact2, int contact_count);
    };
    
    class Collisions
    {
    public:
        void point_segment_distance(Vector2 p, Vector2 a, Vector2 b, float& distance_squared, Vector2&contact_point);
        void find_contact_points(Body body_a, Body body_b, Vector2& contact1, Vector2& contact2, int& contact_count);
        bool check_collision(Body body_a, Body body_b, Vector2& normal, float& depth);
        bool intersect_circle_polygon(Vector2 circle_center, float circle_radius, Vector2 polygon_center, std::vector<Vector2> vertices, Vector2& normal, float& depth);
        bool intersect_polygons(Vector2 center_a, std::vector<Vector2> vertices_a, Vector2 center_b, std::vector<Vector2> vertices_b, Vector2& normal, float& depth);
        bool intersect_circles( Vector2 center_a, float radius_a,  Vector2 center_b, float radius_b, Vector2& normal, float& depth);
    private:
        void find_polygons_contact_points(std::vector<Vector2> vertices_a, std::vector<Vector2> vertices_b, Vector2& contact1, Vector2& contact2, int& contact_count);
        void find_circle_polygon_contact_point( Vector2 circle_center, float circle_radius, Vector2 polygon_center, std::vector<Vector2> polygon_vertices, Vector2&contact_point);
        void find_circles_contact_point(Vector2 center_a, float radius_a, Vector2 center_b, Vector2&contact_point);
        int find_closest_point_on_polygon(Vector2 circle_center, std::vector<Vector2> vertices);
        void project_circle(Vector2 center, float radius, Vector2 axis, float& min, float& max);
        void project_vertices(std::vector<Vector2> vertices, Vector2 axis, float& min, float& max);
    };
}
