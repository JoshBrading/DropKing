#ifndef F_MORTAR_H
#define F_MORTAR_H

#include "entity.h"

class FMortar final : public Entity
{
public:
    explicit FMortar(const Vector3 position);

    virtual void update() override;
};

#endif // F_MORTAR_H
