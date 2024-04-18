#ifndef F_MORTAR_H
#define F_MORTAR_H

#include "entity.h"

class f_mortar final : public entity
{
public:
    explicit f_mortar(const Vector3 position);

    virtual void update() override;
};

#endif // F_MORTAR_H
