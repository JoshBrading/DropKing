#include "gem.h"

#include "player.h"

namespace Game::Entities::Items
{
    Gem::Gem(Vector2 position) : Entity(position, 0)
    {
        gem_texture = LoadTexture("assets\\items\\gem.png");
        gem_object = Physics::create_static_circle(position, 16);
        Physics::ObjectDetails* details = new Physics::ObjectDetails();
        details->tag = Physics::ObjectDetails::GEM;
        details->data = this;
        cpShapeSetUserData(gem_object->shape, details);
    }

    Physics::Object* Gem::get_gem_object()
    {
        return gem_object;
    }

    void Gem::reset()
    {
        collected = false;
    }

    void Gem::update()
    {
        Entity::update();
        position.x += sin(GetTime()) * 5 * GetFrameTime();
        position.y += cos(GetTime()) * 10 * GetFrameTime();
        rotation += cos(GetTime())* 10 * GetFrameTime();
    }

    void Gem::draw()
    {
        Entity::draw();
        if( !collected )
            DrawTextureEx(gem_texture, {position.x - gem_texture.width / 2, position.y - gem_texture.height / 2}, rotation, 1, WHITE);
    }

    void remove_gem_from_space(cpSpace* space, void* key, void* gem)
    {
        auto gem_cast = static_cast<Gem*>(gem);
        cpSpaceRemoveShape(space, gem_cast->get_gem_object()->shape);
        cpSpaceRemoveBody(space, gem_cast->get_gem_object()->body);
    }

    void Gem::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
    {
        Entity::on_collision(arb, space, entity);
        if( entity )
        {
            if( entity->get_tag() == PLAYER )
            {
                Player* player = static_cast<Player*>(entity);
                player->set_score(player->get_score() * multiplier);
                collected = true;
                cpArbiterIgnore(arb);
                cpSpaceAddPostStepCallback(space, remove_gem_from_space, nullptr, this);
            }
        }
    }
}
