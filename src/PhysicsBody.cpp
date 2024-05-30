#include "PhysicsBody.h"

#include <algorithm>
#include <raymath.h>

#include "PhysicsWorld.h"
#include "PhysicsTransform.h"

namespace Physics
{
    Vector2 Body::get_position()
    {
        return position;
    }

    void Body::set_position(Vector2 position)
    {
        this->position = position;
        transform_update_required = true;
    }

    void Body::rotate(float amount)
    {
        this->angle += amount;
        transform_update_required = true;
    }


    Vector2 Body::get_linear_velocity()
    {
        return linear_velocity;
    }

    void Body::set_linear_velocity(Vector2 linear_velocity)
    {
        this->linear_velocity = linear_velocity;
    }

    float Body::get_angle()
    {
        return angle;
    }

    float Body::get_angular_velocity()
    {
        return angular_velocity;
    }

    void Body::set_angular_velocity(float angular_velocity)
    {
        this->angular_velocity = angular_velocity;
    }

    Body::Body(float density, float mass, float inertia, float restitution, float area, bool is_static, float radius,
               float width, float height, std::vector<Vector2> vertices, ShapeType shape_type)
    {
        this->density = density;
        this->mass = mass;
        this->inverse_mass = 1.0f / mass;
        this->inertia = inertia;
        this->inverse_inertia = 1.0f / inertia;
        this->restitution = restitution;
        this->area = area;
        this->is_static = is_static;
        this->radius = radius;
        this->width = width;
        this->height = height;
        this->vertices = vertices;
        this->shape_type = shape_type;
    }

    std::vector<Vector2> BodyFactory::create_box_vertices(float width, float height)
    {
        float left = -width / 2.0f;
        float right = left + width;
        float bottom = -height / 2.0f;
        float top = bottom + height;

        std::vector<Vector2> vertices;
        vertices.push_back({left, top});
        vertices.push_back({right, top});
        vertices.push_back({right, bottom});
        vertices.push_back({left, bottom});

        return vertices;
    }

    std::vector<Vector2> Body::get_transformed_vertices()
    {
        if(transform_update_required)
        {
            Transform t = Transform(position, angle);

            for(int i = 0; i < vertices.size(); i++)
            {
                Vector2 v = vertices[i];
                transformed_vertices[i] = transform_vector(v, t);
            }

            WorldSettings::TRANSFORM_COUNT++;
        }
        else
        {
            WorldSettings::NO_TRANSFORM_COUNT++;
        }

        transform_update_required = false;
        return transformed_vertices;
    }

    void Body::update(float time, Vector2 gravity, const int iterations)
    {
        if(is_static)
        {
            return;
        }

        time /= (float)iterations;

        // force = mass * acc
        // acc = force / mass;

        //FlatVector acceleration = force / Mass;
        //linearVelocity += acceleration * time;
        
        linear_velocity = Vector2Add(linear_velocity, Vector2Scale(gravity, time));
        position = Vector2Add(position, Vector2Scale(linear_velocity, time));

        angle += angular_velocity * time;

        force = {0, 0};
        transform_update_required = true;
    }

    void Body::translate(Vector2 amount)
    {
        position = Vector2Add(position, amount);
        transform_update_required = true;
    }

    void Body::set_rotation(float angle)
    {
        this->angle = angle;
        transform_update_required = true;
    }

    void Body::add_force(Vector2 amount)
    {
        force = amount;
    }

    bool BodyFactory::create_circle_body(float radius, float density, bool is_static, float restitution, Body& body,
        std::string& error)
    {
        float area = radius * radius * PI;

        if(area < WorldSettings::MIN_BODY_SIZE)
        {
            return false;
        }

        if(area > WorldSettings::MAX_BODY_SIZE)
        {
            return false;
        }

        if (density < WorldSettings::MIN_DENSITY)
        {
            return false;
        }

        if (density > WorldSettings::MAX_DENSITY)
        {
            return false;
        }

        restitution = std::ranges::clamp(restitution, 0.0f, 1.0f);

        float mass = 0.0f;
        float inertia = 0.0f;

        if (!is_static)
        {
            // mass = area * depth * density
            mass = area * density;
            inertia = (1.0f / 2.0f) * mass * radius * radius;
        }

        body = Body(density, mass, inertia, restitution, area, is_static, radius, 0.0f, 0.0f, {}, CIRCLE);
        return true;
    }

    bool BodyFactory::create_box_body(float width, float height, float density, bool is_static, float restitution, Body& body,
        std::string& error)
    {
        float area = width * height;

        if (area < WorldSettings::MIN_BODY_SIZE)
        {
            return false;
        }

        if (area > WorldSettings::MAX_BODY_SIZE)
        {
            return false;
        }

        if (density < WorldSettings::MIN_DENSITY)
        {
            return false;
        }

        if (density > WorldSettings::MAX_DENSITY)
        {
            return false;
        }

        restitution = std::ranges::clamp(restitution, 0.0f, 1.0f);

        float mass = 0.0f;
        float inertia = 0.0f;

        if (!is_static)
        {
            // mass = area * depth * density
            mass = area * density;
            inertia = (1.0f / 12) * mass * (width * width + height * height);
        }

        std::vector<Vector2> vertices = create_box_vertices(width, height);

        body = Body(density, mass, inertia, restitution, area, is_static, 0.0f, width, height, vertices, BOX);
        return true;
    }
}
