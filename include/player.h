#pragma once

#include <vector>

#include "entity.h"
#include "physics.h"


namespace Game::Entities
{
    class Player : public Entity
    {
        int hearts = 3;
        float score = 0;
        bool level_complete = false;
        bool is_jumping = false;
        Vector2 ground_normal = {0, 0};
        cpBody* ground = nullptr;
        float max_jump_height = 1000;
        float jump_scale = 0.0f;
        float max_jump_scale = 2.0f;
        float jump_scale_increase = 2.0f;
        Physics::Object* player_object;
        Vector2 spawn_point;
        Texture2D player_background;
        Texture2D player_face;
        std::vector<Texture2D> player_faces;
        std::vector<Texture2D> player_backgrounds;
        Vector2 texture_offset = {0, 0};

        void take_snapshot() const;
        void calculate_score();
    public:
        bool is_grounded = false;
        explicit Player(Vector2 position);
        void reset_player();
        void set_spawn_point(Vector2 spawn_point);
        void set_ground_normal(Vector2 normal);
        int get_hearts() const;
        float get_score() const;
        void set_score(float new_score);
        Physics::Object* get_player_object() const;
        void update() override;
        void draw() override;
        void on_collision(cpArbiter* arb, cpSpace* space, Entity* entity) override;
    };
}
