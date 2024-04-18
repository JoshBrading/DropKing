#ifndef ENTITY_H
#define ENTITY_H
#include <raylib.h>

class entity
{
public:
    entity(const Vector3 position, const char* model_path, const char* name);
    entity(entity&&) = delete; // Move constructor
    entity(const entity&) = delete; // Copy constructor
    entity& operator=(entity&&) = delete; // Move assignment operator
    entity& operator=(const entity&) = delete; // Copy assignment operator
    virtual ~entity(); // Destructor, needed for virtual function overrides?
    
    virtual void update();
    virtual void update_fixed();
    void draw() const;

private:

    int id_;
    char tag_;
    const char* name_;
    
    bool visibility_;
    bool collision_;

    Model model_;

    Vector3 position_;
    Vector3 rotation_;
    Vector3 scale_;
};

#endif //ENTITY_H
