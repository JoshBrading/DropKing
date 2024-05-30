#pragma once

#include <raylib.h>
#include <string>
#include <vector>

namespace Physics
{
    class Body;

    enum ShapeType
    {
        CIRCLE,
        BOX,
        POLYGON
    };

    struct AABB
    {
        Vector2 min;
        Vector2 max;

        AABB(Vector2 min, Vector2 max) : min(min), max(max) {}
    };

    namespace BodyFactory
    {
        bool create_circle_body(float radius, float density, bool is_static, float restitution, Body& body, std::string& error);
        bool create_box_body(float width, float height, float density, bool is_static, float restitution, Body& body, std::string& error);
        static std::vector<Vector2> create_box_vertices(float width, float height);
    }

    class Body
    {
    public:
        ShapeType shape_type;
        float density;
        float mass;
        float inverse_mass;
        float restitution;
        float area;
        float inertia;
        float inverse_inertia;
        bool is_static;
        float radius;
        float width;
        float height;
        float static_friction = 0.5f;
        float dynamic_friction = 0.7f;

    private:
        std::vector<Vector2> vertices;
        std::vector<Vector2> transformed_vertices;

        bool transform_update_required = false;

        Vector2 position = {};
        Vector2 linear_velocity = {};
        float angle = 0;
        float angular_velocity = 0;
        Vector2 force = {};

    public:
        Vector2 get_position();
        void set_position(Vector2 position);

        Vector2 get_linear_velocity();
        void set_linear_velocity(Vector2 linear_velocity);

        float get_angle();

        float get_angular_velocity();
        void set_angular_velocity(float angularVelocity);
        Body() = default;
        Body(float density, float mass, float inertia, float restitution, float area, bool is_static, float radius,
                 float width, float height, std::vector<Vector2> vertices, ShapeType shape_type);

        
        std::vector<Vector2> get_transformed_vertices();

        void update(float time, Vector2 gravity, const int iterations);

        void translate(Vector2 amount);

        void rotate(float amount);

        void set_rotation(float angle);

        void add_force(Vector2 amount);

    };
}
