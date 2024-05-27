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
    Entity* parent;
    float rotation = 0;
    float mass = 1.0f;
    float restitution = 1.0f;
    Vector2 velocity = {0, 0};
    Vector2 force = {0, 0};
    
    Shape type;
    union
    {
        Polygon* polygon;
        Circle* circle;
    };
    
    Body(Polygon* p): type(POLYGON), polygon(p) {}
    Body(Circle* c): type(CIRCLE), circle(c) {}

    void rotate(float degrees) const;
    void translate(Vector2 translation) const;
    void apply_force(Vector2 force);
};

class Collision
{
public:
    static Collision& instance();
    static void update() { instance().i_update();}
    static void draw_debug() { instance().i_draw_debug();}
    static void add_collider(Body* collider) { instance().i_add_collider(collider);}
private:
    Collision() = default;
    std::vector<Body*> colliders;
    void i_update();
    void i_draw_debug() const;
    void i_add_collider( Body* collider);
};



#endif // COLLISION_H
