#ifndef COLLISION_H
#define COLLISION_H
#include <vector>
#include <raylib.h>

#include "entity.h"


enum Shape
{
    POLYGON,
    CIRCLE
};

struct Polygon
{
    Vector2 origin;
    std::vector<Vector2> points;
};

struct Circle
{
    Vector2 origin;
    float radius;
};

struct Body
{
    bool is_static = false;
    bool rotation_static = false;
    Entity* parent;
    float width = 20;
    float height = 20;
    float rotation = 0;
    float mass = 20.0f;
    float inertia = 0;
    float restitution = 0.5f;
    float friction = 0.5f;
    float friction_ramp = 0.75f;
    Vector2 velocity = {0, 0};
    float angular_velocity = 0;
    Vector2 force = {0, 0};
    
    Shape type;
    union
    {
        Polygon* polygon;
        Circle* circle;
    };
    
    Body(Polygon* p);
    Body(Circle* c);

    void rotate(float degrees) const;
    float calculate_rotational_inertia();
    void translate(Vector2 translation) const;
    void apply_force(Vector2 force);
};

struct CollisionData
{
    Body* a;
    Body* b;
    Vector2 normal;
    float depth;
    Vector2 contact_1;
    Vector2 contact_2;
    int contact_count;

    CollisionData(Body* a, Body* b, Vector2 normal, float depth, Vector2 contact_1, Vector2 contact_2, int contact_count):
        a(a), b(b), normal(normal), depth(depth), contact_1(contact_1), contact_2(contact_2), contact_count(contact_count) {}
};

class Collision
{
public:
    static Collision& instance();
    static void update( int substeps ) { instance().i_update( substeps );}
    static void draw_debug() { instance().i_draw_debug();}
    static void add_collider(Body* collider) { instance().i_add_collider(collider);}
    static void add_collision(const CollisionData& collision) { instance().collisions.push_back(collision);}
private:
    Collision() = default;
    std::vector<Body*> colliders;
    std::vector<CollisionData> collisions;
    void i_update( int substeps );
    void i_draw_debug() const;
    void i_add_collider( Body* collider);
};



#endif // COLLISION_H
