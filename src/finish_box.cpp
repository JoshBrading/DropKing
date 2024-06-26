﻿#include "finish_box.h"

namespace Game::Entities
{
    FinishBox::FinishBox(Vector2 start, Vector2 end, GameWorld* game) : Entity(start, 0)
    {
        this->position = start;
        this-> position_end = end;
        this->checkerboard = LoadTexture("assets\\checkerboard.png");
        this->game = game;
        this->tag = FINISH_BOX;
        Physics::ObjectDetails* details = new Physics::ObjectDetails();
        details->tag = Physics::ObjectDetails::FINISH;
        details->data = this;
        Vector2 offset_center = {start.x + (end.x - start.x) / 2, start.y + (end.y - start.y) / 2};
        this->finish_box = Physics::create_static_square(offset_center, {end.x - start.x, end.y - start.y});

        cpShapeSetUserData(finish_box->shape, details);
    }

    Vector2 FinishBox::get_end()
    {
        return position_end;
    }

    Physics::Object* FinishBox::get_finish_box()
    {
        return finish_box;
    }

    void FinishBox::update()
    {
        Entity::update();
    }

    void FinishBox::draw()
    {
        Entity::draw();
        Rectangle rect = {position.x, position.y, position_end.x - position.x, position_end.y - position.y};
        DrawTexturePro(checkerboard, {0, 0, rect.width, rect.height}, rect, {0, 0}, 0, {255, 255, 255, 128});
    }

    void post_collision(cpSpace* space, void* key, Game::GameWorld* game)
    {
        game->open_level_complete_menu();
    }

    void FinishBox::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
    {
        Entity::on_collision(arb, space, entity);
        if( entity->get_tag() == PLAYER)
        {
            cpArbiterIgnore(arb);
            cpSpaceAddPostStepCallback( space, reinterpret_cast<cpPostStepFunc>(post_collision), nullptr, game);
        }
    }
}
