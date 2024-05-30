#pragma once

#include <raylib.h>
#include <vector>

#include "PhysicsBody.h"
#include "PhysicsCollision.h"


namespace Physics
{
    namespace WorldSettings
    {
        inline int TRANSFORM_COUNT = 0;
        inline int NO_TRANSFORM_COUNT = 0;

        inline float MIN_BODY_SIZE = 0.01f * 0.01f;
        inline float MAX_BODY_SIZE = INFINITY;

        inline float MIN_DENSITY = 0.5f;
        inline float MAX_DENSITY = 21.4f;

        inline int MIN_ITERATIONS = 1;
        inline int MAX_ITERATIONS = 128;
    }
    class World
    {
        
    private:
        Collisions collision;
        Vector2 gravity = {0.0f, 9.8f};
        std::vector<Body*> body_list;
        std::vector<std::pair<int, int>> contact_pairs;

        Vector2 contact_list[2];
        Vector2 impulse_list[2];
        Vector2 ra_list[2];
        Vector2 rb_list[2];
        Vector2 friction_impulse_list[2];
        float j_list[2];
    public:
        World();
        int get_body_count();
        void add_body(Body* body);
        bool remove_body(Body* body);
        bool get_body(int index, Body* body);
        void update(float time, int total_iterations);
        void update_bodies(float time, int total_iterations);
        void resolve_collision_basic(CollisionData contact);
        void resolve_collision_with_rotation(CollisionData contact);
        void resolve_collision_with_rotation_and_friction(CollisionData contact);
    private:
        void broad_phase();
        void narrow_phase();
        void separate_bodies(Body* body_a, Body* body_b, Vector2 mtv);
    };

    
    namespace Instance
    {
        inline World WORLD = World();
    }
}
