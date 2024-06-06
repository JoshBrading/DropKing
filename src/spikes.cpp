#include "spikes.h"

namespace Game::Entities::Obstacles
{
    Spikes::Spikes(Vector2 start, Vector2 end) : Entity(start, 0)
    {
        this->position = start;
        this->position_end = end;
        this->tag = SPIKE_PIT;
        this->spikes_texture = LoadTexture("assets\\spikes.png");
        Vector2 offset_center = {start.x + (end.x - start.x) / 2, start.y + (end.y - start.y) / 2};
        this->spikes = Physics::create_static_square(offset_center, {end.x - start.x, end.y - start.y - 32});
        Physics::ObjectDetails* details = new Physics::ObjectDetails();
        details->tag = Physics::ObjectDetails::SPIKES;
        details->data = this;
        cpShapeSetUserData(spikes->shape, details);

    }

    Vector2 Spikes::get_end()
    {
        return position_end;
    }

    Physics::Object* Spikes::get_spikes()
    {
        return spikes;
    }

    void Spikes::update()
    {
        Entity::update();
    }

    void Spikes::draw()
    {
        
        Entity::draw();
        Rectangle rect = {position.x, position.y, position_end.x - position.x, position_end.y - position.y};
        DrawTexturePro(spikes_texture, {0, 0, rect.width, rect.height}, rect, {0, 0}, 0, {255, 255, 255, 128});
   
    }

    void Spikes::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
    {
        Entity::on_collision(arb, space, entity);
    }
}
