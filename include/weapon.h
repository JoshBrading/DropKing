#ifndef WEAPON_H
#define WEAPON_H
#include "entity.h"
#include "Projectile.h"

class Weapon
{
public:

    void shoot();
    void reload();

    Entity* get_parent();
    
    int max_ammo = 10;
    int magazine_count = 4;
    float reload_time = 2.0f;
    float damage = 20.0f;
    bool explosive = false;
    
private:
    Projectile projectile;
    Entity* parent = nullptr;
    
};

#endif //WEAPON_H