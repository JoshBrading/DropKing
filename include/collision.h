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

struct Collider
{
    bool is_static = false;
    Entity* parent;

    Shape type;
    union
    {
        Polygon* polygon;
        Circle* circle;
    };
    
    Collider(Polygon* p): type(POLYGON), polygon(p) {}
    Collider(Circle* c): type(CIRCLE), circle(c) {}
};

class Collision
{
public:
    static Collision& instance();
    static void update() { instance().i_update();}
    static void draw_debug() { instance().i_draw_debug();}
    static void add_collider(Collider* collider) { instance().i_add_collider(collider);}
private:
    Collision() = default;
    std::vector<Collider*> colliders;
    void i_update();
    void i_draw_debug() const;
    void i_add_collider( Collider* collider);
};



#endif // COLLISION_H
