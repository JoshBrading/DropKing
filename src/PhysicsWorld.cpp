#include "PhysicsWorld.h"

#include <algorithm>
#include <ctime>
#include <raymath.h>

#include "PhysicsBody.h"
#include "PhysicsCollision.h"

namespace Physics
{
    World::World() = default;

    int World::get_body_count()
    {
        return body_list.size();
    }

    void World::add_body(Body* body)
    {
        body_list.push_back(body);
    }

    bool World::remove_body(Body* body)
    {
        return true;//return std::erase_if(body_list, *body);
    }

    bool World::get_body(int index, Body* body)
    {
        if (index < 0 || index >= body_list.size())
            return false;
        body = body_list.at(index);
        return true;
    }

    void World::update(float time, int total_iterations)
    {
        total_iterations = std::ranges::clamp(total_iterations, WorldSettings::MIN_ITERATIONS, WorldSettings::MAX_ITERATIONS);

        for (int current_iteration = 0; current_iteration < total_iterations; current_iteration++)
        {
            contact_pairs.clear();
            update_bodies(time, total_iterations);
            broad_phase();
            narrow_phase();
        }
    }

    void World::update_bodies(float time, int total_iterations)
    {
        for (int i = 0; i < body_list.size(); i++)
        {
            body_list[i]->update(time, gravity, total_iterations);
        }
    }

    void World::resolve_collision_basic(CollisionData contact)
    {
    }

    void World::resolve_collision_with_rotation(CollisionData contact)
    {
    }

    void World::resolve_collision_with_rotation_and_friction(CollisionData contact)
    {
        Body* body_a = contact.body_a;
        Body* body_b = contact.body_b;
        Vector2 normal = contact.normal;
        Vector2 contact1 = contact.contact1;
        Vector2 contact2 = contact.contact2;
        int contactCount = contact.contact_count;

        float e = std::min(body_a->restitution, body_b->restitution);

        float sf = (body_a->static_friction + body_b->static_friction) * 0.5f;
        float df = (body_a->dynamic_friction + body_b->dynamic_friction) * 0.5f;

        contact_list[0] = contact1;
        contact_list[1] = contact2;

        for (int i = 0; i < contactCount; i++)
        {
            impulse_list[i] = {0, 0};
            ra_list[i] = {0, 0};
            rb_list[i] = {0, 0};
            friction_impulse_list[i] = {0, 0};
            j_list[i] = 0.0f;
        }

        for (int i = 0; i < contactCount; i++)
        {
            Vector2 ra = Vector2Subtract(contact_list[i], body_a->get_position());
            Vector2 rb = Vector2Subtract(contact_list[i], body_b->get_position());

            ra_list[i] = ra;
            rb_list[i] = rb;

            Vector2 ra_perp = Vector2(-ra.y, ra.x);
            Vector2 rb_perp = Vector2(-rb.y, rb.x);

            Vector2 angular_linear_velocityA = Vector2Scale(ra_perp, body_a->get_angular_velocity());
            Vector2 angular_linear_velocityB = Vector2Scale(rb_perp, body_b->get_angular_velocity());

            Vector2 relative_velocity = Vector2Subtract(
                Vector2Add(body_b->get_linear_velocity(), angular_linear_velocityB),
                Vector2Add(body_a->get_linear_velocity(), angular_linear_velocityA));

            float contact_velocity_mag = Vector2DotProduct(relative_velocity, normal);

            if (contact_velocity_mag > 0.0f)
            {
                continue;
            }

            float ra_perpDotN = Vector2DotProduct(ra_perp, normal);
            float rb_perpDotN = Vector2DotProduct(rb_perp, normal);

            float denom = body_a->inverse_mass + body_b->inverse_mass +
                (ra_perpDotN * ra_perpDotN) * body_a->inverse_inertia +
                (rb_perpDotN * rb_perpDotN) * body_b->inverse_inertia;

            float j = -(1.0f + e) * contact_velocity_mag;
            j /= denom;
            j /= (float)contactCount;

            j_list[i] = j;

            Vector2 impulse = Vector2Scale(normal, j);
            impulse_list[i] = impulse;
        }

        for (int i = 0; i < contactCount; i++)
        {
            Vector2 impulse = impulse_list[i];
            Vector2 ra = ra_list[i];
            Vector2 rb = rb_list[i];

            body_a->set_linear_velocity(Vector2Add(body_a->get_linear_velocity(),
                                                  Vector2Scale(impulse, -body_a->inverse_mass)));
            body_a->set_angular_velocity(
                body_a->get_angular_velocity() + ((ra.x * impulse.y) - (ra.y * impulse.x) * -body_a->inverse_inertia));

            body_b->set_linear_velocity(Vector2Add(body_b->get_linear_velocity(),
                                                  Vector2Scale(impulse, body_b->inverse_mass)));
            body_b->set_angular_velocity(
                body_b->get_angular_velocity() + ((rb.x * impulse.y) - (rb.y * impulse.x) * body_b->inverse_inertia));
        }

        for (int i = 0; i < contactCount; i++)
        {
            Vector2 ra = Vector2Subtract(contact_list[i], body_a->get_position());
            Vector2 rb = Vector2Subtract(contact_list[i], body_b->get_position());

            ra_list[i] = ra;
            rb_list[i] = rb;

            Vector2 ra_perp = Vector2(-ra.y, ra.x);
            Vector2 rb_perp = Vector2(-rb.y, rb.x);

            Vector2 angular_linear_velocityA = Vector2Scale(rb_perp, body_a->get_angular_velocity());
            Vector2 angular_linear_velocityB = Vector2Scale(rb_perp, body_b->get_angular_velocity());

            Vector2 relative_velocity = Vector2Subtract(
                Vector2Add(body_b->get_linear_velocity(), angular_linear_velocityB),
                Vector2Add(body_a->get_linear_velocity(), angular_linear_velocityA));


            Vector2 tangent = Vector2Subtract(relative_velocity,
                                              Vector2Scale(normal, Vector2DotProduct(relative_velocity, normal)));

            if (Vector2Equals(tangent, {0, 0}))
            {
                continue;
            }
            else
            {
                tangent = Vector2Normalize(tangent);
            }

            float ra_perpDotT = Vector2DotProduct(ra_perp, tangent);
            float rb_perpDotT = Vector2DotProduct(rb_perp, tangent);

            float denom = body_a->inverse_mass + body_b->inverse_mass +
                (ra_perpDotT * ra_perpDotT) * body_a->inverse_inertia +
                (rb_perpDotT * rb_perpDotT) * body_b->inverse_inertia;

            float jt = -Vector2DotProduct(relative_velocity, tangent);
            jt /= denom;
            jt /= (float)contactCount;

            Vector2 friction_impulse;
            float j = j_list[i];

            if (std::abs(jt) <= j * sf)
            {
                friction_impulse = Vector2Scale(tangent, jt);
            }
            else
            {
                friction_impulse = Vector2Scale(Vector2Scale(tangent, -j), df);
            }

            friction_impulse_list[i] = friction_impulse;
        }

        for (int i = 0; i < contactCount; i++)
        {
            Vector2 frictionImpulse = friction_impulse_list[i];
            Vector2 ra = ra_list[i];
            Vector2 rb = rb_list[i];

            body_a->set_linear_velocity(Vector2Add(body_a->get_linear_velocity(),
                                                  Vector2Scale(frictionImpulse, -body_a->inverse_mass)));
            body_a->set_angular_velocity(
                body_a->get_angular_velocity() + ((ra.x * frictionImpulse.y) - (ra.y * frictionImpulse.x) * -body_a->
                    inverse_inertia));
            body_b->set_linear_velocity(Vector2Add(body_b->get_linear_velocity(),
                                                  Vector2Scale(frictionImpulse, body_b->inverse_mass)));
            body_b->set_angular_velocity(
                body_b->get_angular_velocity() + ((rb.x * frictionImpulse.y) - (rb.y * frictionImpulse.x) * body_b->
                    inverse_inertia));
        }
    }


    void World::broad_phase()
    {
        for (int i = 0; i < body_list.size(); i++)
        {
            Body* body_a = body_list.at(i);

            for (int j = i + 1; j < body_list.size(); j++)
            {
                Body* body_b = body_list.at(j);

                if (body_a->is_static && body_b->is_static)
                    continue;

                contact_pairs.push_back({i, j});
            }
        }
    }

    void World::narrow_phase()
    {
        for (int i = 0; i < contact_pairs.size(); i++)
        {
            std::pair<int, int> pair = contact_pairs.at(i);
            Body* body_a = body_list.at(pair.first);
            Body* body_b = body_list.at(pair.second);
            Vector2 normal;
            float depth;

            if (collision.check_collision(*body_a, *body_b, normal, depth))
            {
                separate_bodies(body_a, body_b, Vector2Scale(normal, depth));
                Vector2 contact1;
                Vector2 contact2;
                int contact_count;
                collision.find_contact_points(*body_a, *body_b, contact1, contact2, contact_count);
                auto contact = CollisionData(body_a, body_b, normal, depth, contact1, contact2, contact_count);
                resolve_collision_with_rotation_and_friction(contact);
            }
        }
    }

    void World::separate_bodies(Body* body_a, Body* body_b, Vector2 mtv)
    {
        if (body_a->is_static)
            body_b->translate(mtv);
        else if (body_b->is_static)
            body_a->translate(Vector2Negate(mtv));
        else
        {
            body_a->translate(Vector2Negate(Vector2Scale(mtv, 0.5f)));
            body_b->translate(Vector2Scale(mtv, 0.5f));
        }
    }
}
