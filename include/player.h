#pragma once
#include <raylib.h>
#include "collision.h"


class Player
{
public:
    Player(const Vector2 position, const int width, const int height);
    Texture2D sprite;
    Vector2 position;
    int width;
    int height;

    Polygon* polygon;
    
    void update();

    void draw();
    
};
