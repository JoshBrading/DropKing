#include "key.h"

#include "player.h"

namespace Game::Entities::Items
{
    Key::Key(Vector2 position) : Entity(position, 0)
    {
        key_texture = LoadTexture("assets\\items\\key.png");
        key_object = Physics::create_static_circle(position, 16);
        Physics::ObjectDetails* details = new Physics::ObjectDetails();
        details->tag = Physics::ObjectDetails::KEY;
        details->data = this;
        cpShapeSetUserData(key_object->shape, details);
    }

    Physics::Object* Key::get_key_object()
    {
        return key_object;
    }

    void Key::reset()
    {
        collected = false;
    }

    void Key::update()
    {
        Entity::update();
        position.x += sin(GetTime()) * 5 * GetFrameTime();
        position.y += cos(GetTime()) * 10 * GetFrameTime();
        rotation += sin(GetTime())* 10 * GetFrameTime();
    }

    void Key::draw()
    {
        Entity::draw();
        if (!collected)
            DrawTextureEx(key_texture,{ position.x - key_texture.width / 2, position.y - key_texture.height / 2}, rotation, 1, WHITE);
    }

    void remove_key_from_space(cpSpace* space, void* _, void* key)
    {
        auto key_cast = static_cast<Key*>(key);
        cpSpaceRemoveShape(space, key_cast->get_key_object()->shape);
        cpSpaceRemoveBody(space, key_cast->get_key_object()->body);
    }

    void Key::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
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
                cpSpaceAddPostStepCallback(space, remove_key_from_space, nullptr, this);
            }
        }
    }
}
