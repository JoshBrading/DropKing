#include "collision.h"

#include <complex>
#include <raymath.h>

#include "transform.h"

float TOLERANCE = 0.01f;
Vector2 GRAVITY = {0, 9.8};

Collision& Collision::instance()
{
    static auto instance = Collision();
    return instance;
}

Body::Body(Polygon* p)
{
    polygon = p;
    type = POLYGON;
    inertia = calculate_rotational_inertia();
}

Body::Body(Circle* c)
{
    circle = c;
    type = CIRCLE;
    inertia = calculate_rotational_inertia();
}

void Body::rotate(float degrees) const
{
    if( type != POLYGON )
        return;
    for( auto& point: polygon->points )
    {
        point = transform::rotate_point_about_target(polygon->origin, point, degrees);
    }
}

float Body::calculate_rotational_inertia()
{
    if( type == CIRCLE )
    {
        return (1.0f / 2.0f) * mass * powf(circle->radius, 2);
    }
    if( type == POLYGON )
    {
        return (1.0f / 12.0f) * mass * (powf(width , 2) + powf(height, 2));
    }
}

void Body::translate( Vector2 translation ) const
{
    if( type == CIRCLE )
    {
        circle->origin = Vector2Add(circle->origin, translation);
    }
    if( type == POLYGON )
    {
        polygon->origin = Vector2Add(polygon->origin, translation);
        for( auto& point: polygon->points )
        {
            point = Vector2Add(point, translation);
        }
    }
}

void Body::apply_force( Vector2 force )
{
    this->force = force;
}


void project_polygon_to_axis(const Vector2 axis, const Polygon* p, float &min, float &max)
{
    min = INFINITY;
    max = -INFINITY;

    for( const auto& point: p->points )
    {
        float projection = Vector2DotProduct(point, axis);

        if( projection < min )
            min = projection;
        if( projection > max )
            max = projection;
    }
}

void project_circle_to_axis(const Vector2 axis, const Circle* c, float &min, float &max)
{
    float projection = Vector2DotProduct(c->origin, axis);
    min = projection - c->radius;
    max = projection + c->radius;

    if( min > max )
        std::swap(min, max);
}

int closest_point_to_target(const std::vector<Vector2>& points, const Vector2 target )
{
    float min_distance = INFINITY;
    int closest_point = 0;

    for( int i = 0; i < points.size(); i++ )
    {
        float distance = Vector2Distance(points[i], target);
        if( distance < min_distance )
        {
            min_distance = distance;
            closest_point = i;
        }
    }

    return closest_point;
}

bool polygon_intersects_polygon(const Polygon* p1, const Polygon* p2, Vector2 &normal, float &depth)
{
    normal = {0, 0};
    depth = INFINITY;
    
    for( int i = 0; i < p1->points.size(); i++ )
    {
        const Vector2 vec_a = p1->points[i];
        const Vector2 vec_b = p1->points[(i + 1) % p1->points.size()];

        auto [x, y] = Vector2Subtract(vec_b, vec_a);
        const Vector2 axis = Vector2Normalize({-y, x});

        float p1_min, p1_max, p2_min, p2_max;
        project_polygon_to_axis(axis, p1, p1_min, p1_max);
        project_polygon_to_axis(axis, p2, p2_min, p2_max);

        if( p1_min >= p2_max - TOLERANCE || p2_min >= p1_max - TOLERANCE )
            return false;

        float tmp_depth = std::min(p2_max - p1_min, p1_max - p2_min);
        if( tmp_depth < depth )
        {
            depth = tmp_depth;
            normal = axis;
        }
    }
    
    for( int i = 0; i < p2->points.size(); i++ )
    {
        const Vector2 vec_a = p2->points[i];
        const Vector2 vec_b = p2->points[(i + 1) % p2->points.size()];

        auto [x, y] = Vector2Subtract(vec_b, vec_a);
        const Vector2 axis = Vector2Normalize({-y, x});

        float p1_min, p1_max, p2_min, p2_max;
        project_polygon_to_axis(axis, p1, p1_min, p1_max);
        project_polygon_to_axis(axis, p2, p2_min, p2_max);

        if( p1_min >= p2_max - TOLERANCE || p2_min >= p1_max - TOLERANCE )
            return false;

        float tmp_depth = std::min(p2_max - p1_min, p1_max - p2_min);
        if( tmp_depth < depth )
        {
            depth = tmp_depth;
            normal = axis;
        }
    }

    Vector2 direction = Vector2Subtract(p2->origin, p1->origin);

    if( Vector2DotProduct(direction, normal) > 0 )
        normal = Vector2Negate(normal);
    
    return true;
}

bool polygon_intersects_circle(const Polygon* p, const Circle* c, Vector2& normal, float& depth)
{
    normal = {0, 0};
    depth = INFINITY;
    float p_min, p_max, c_min, c_max;
    
    for( int i = 0; i < p->points.size(); i++ )
    {
        const Vector2 vec_a = p->points[i];
        const Vector2 vec_b = p->points[(i + 1) % p->points.size()];

        auto [x, y] = Vector2Subtract(vec_b, vec_a);
        const Vector2 axis = Vector2Normalize({-y, x});

        project_polygon_to_axis(axis, p, p_min, p_max);
        project_circle_to_axis(axis, c, c_min, c_max);

        if( p_min >= c_max - TOLERANCE || c_min >= p_max - TOLERANCE )
            return false;

        float tmp_depth = std::min(c_max - p_min, p_max - c_min);
        if( tmp_depth < depth )
        {
            depth = tmp_depth;
            normal = axis;
        }
    }

    Vector2 closest_point = p->points[closest_point_to_target(p->points, c->origin)];
    Vector2 direction = Vector2Subtract(c->origin, closest_point);
    direction = Vector2Normalize(direction);

    project_polygon_to_axis(direction, p, p_min, p_max);
    project_circle_to_axis(direction, c, c_min, c_max);

    if( p_min >= c_max - TOLERANCE || c_min >= p_max - TOLERANCE )
        return false;
    
    float tmp_depth = std::min(c_max - p_min, p_max - c_min);
    if( tmp_depth < depth )
    {
        depth = tmp_depth;
        normal =direction;
    }

    direction = Vector2Subtract(p->origin, c->origin);

    if( Vector2DotProduct(direction, normal) < 0 )
        normal = Vector2Negate(normal);
    
    return true;
}

void point_segment_distance(Vector2 point, Vector2 a, Vector2 b, Vector2& closest_point, float& distance_squared) {
    Vector2 ab = Vector2Subtract(b, a);
    Vector2 ap = Vector2Subtract(point, a);

    float ab_length_squared = Vector2LengthSqr(ab);
    float distance = Vector2DotProduct(ap, ab) / ab_length_squared;

    if (distance <= 0) {
        closest_point = a;
    } else if (distance >= 1) {
        closest_point = b;
    } else {
        closest_point = Vector2Add(a, Vector2Scale(ab, distance));
    }

    Vector2 diff = Vector2Subtract(point, closest_point);
    distance_squared = Vector2LengthSqr(diff);
}


void get_contact_points(Polygon* a, Polygon* b, Vector2& contact_1, Vector2& contact_2, int& contact_count) {
    float min_distance_squared = INFINITY;

    // Check each a->points against edges of b
    for (int i = 0; i < a->points.size(); i++) {
        Vector2 pa = a->points[i];
        for (int j = 0; j < b->points.size(); j++) {
            float distance_squared;
            Vector2 contact;
            Vector2 pb = b->points[j];
            Vector2 pc = b->points[(j + 1) % b->points.size()];

            point_segment_distance(pa, pb, pc, contact, distance_squared);

            if (FloatEquals(distance_squared, min_distance_squared) && !Vector2Equals(contact, contact_1)) {
                contact_2 = contact;
                contact_count = 2;
            }

            if (distance_squared < min_distance_squared) {
                contact_1 = contact;
                min_distance_squared = distance_squared;
                contact_count = 1;
            }
        }
    }

    // Check each b->points against edges of a
    for (int i = 0; i < b->points.size(); i++) {
        Vector2 pa = b->points[i];
        for (int j = 0; j < a->points.size(); j++) {
            float distance_squared;
            Vector2 contact;
            Vector2 pb = a->points[j];
            Vector2 pc = a->points[(j + 1) % a->points.size()];

            point_segment_distance(pa, pb, pc, contact, distance_squared);

            if (FloatEquals(distance_squared, min_distance_squared) && !Vector2Equals(contact, contact_1)) {
                contact_2 = contact;
                contact_count = 2;
            }

            if (distance_squared < min_distance_squared) {
                contact_1 = contact;
                min_distance_squared = distance_squared;
                contact_count = 1;
            }
        }
    }

    if (contact_count == 2 && Vector2Equals(contact_1, contact_2)) {
        contact_count = 1;
    }
}


bool check_collision(Body* a, Body* b, Vector2& normal, float& depth)
{
    if( a->is_static && b->is_static )
        return false;
    bool collision = false;
    if( a->type == POLYGON ) // This is gross, clean up if possible
    {
        if( b->type == POLYGON )
            collision = polygon_intersects_polygon(a->polygon, b->polygon, normal, depth);
        if( b->type == CIRCLE )
            collision = polygon_intersects_circle(a->polygon, b->circle, normal, depth);
    }
    if( a->type == CIRCLE )
    {
        if( b->type == POLYGON )
            collision = polygon_intersects_circle(b->polygon, a->circle, normal, depth);
    }

    if( collision )
    {
        Vector2 contact_1 = {0, 0};
        Vector2 contact_2 = {0, 0};
        int count = 0;
        get_contact_points(a->polygon, b->polygon, contact_1, contact_2, count);
        const CollisionData data = {a, b, normal, depth, contact_1, contact_2, count};
        Collision::add_collision(data);
    }
}

void resolve_polygon_polygon(Polygon* a, Polygon* b, bool a_is_static, bool b_is_static, Vector2 normal, float depth)
{
    if( a_is_static && b_is_static )
        return;
    
    if( a_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        b->origin = Vector2Add(b->origin, displacement);
        for( auto& point: b->points )
        {
            point = Vector2Add(point, displacement);
        }
    }

    if( b_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        a->origin = Vector2Add(a->origin, displacement);
        for( auto& point: a->points )
        {
            point = Vector2Add(point, displacement);
        }
    }

    if( !a_is_static && !b_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth / 2);
        a->origin = Vector2Add(a->origin, displacement);
        for( auto& point: a->points )
        {
            point = Vector2Add(point, displacement);
        }

        b->origin = Vector2Subtract(b->origin, displacement);
        for( auto& point: b->points )
        {
            point = Vector2Subtract(point, displacement);
        }
    }

}

void resolve_polygon_circle(Polygon* a, Circle* b, bool a_is_static, bool b_is_static, Vector2 normal, float depth)
{
    if( a_is_static && b_is_static )
        return;
    
    if( a_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        b->origin = Vector2Add(b->origin, displacement);
    }

    if( b_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        a->origin = Vector2Add(a->origin, displacement);
        for( auto& point: a->points )
        {
            point = Vector2Add(point, displacement);
        }
    }

    if( !a_is_static && !b_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth / 2);
        a->origin = Vector2Add(a->origin, displacement);
        for( auto& point: a->points )
        {
            point = Vector2Add(point, displacement);
        }

        b->origin = Vector2Subtract(b->origin, displacement);
    }
}

void resolve_circle_polygon(Circle* circle, Polygon* polygon, bool circle_is_static, bool polygon_is_static, Vector2 normal, float depth)
{
    resolve_polygon_circle(polygon, circle, polygon_is_static, circle_is_static, normal, depth);

}

void resolve(CollisionData collision)
{
    Body* a = collision.a;
    Body* b = collision.b;
    Vector2 normal = collision.normal;
    float depth = collision.depth;
    
    if( a->is_static && b->is_static )
        return;
    
    if( a->type == POLYGON )
    {
        if( b->type == POLYGON )
            resolve_polygon_polygon(a->polygon, b->polygon, a->is_static, b->is_static, normal, depth);
        if( b->type == CIRCLE )
            resolve_polygon_circle(a->polygon, b->circle, a->is_static, b->is_static, normal, depth);
    }
    if( a->type == CIRCLE )
    {
        if( b->type == POLYGON )
            resolve_circle_polygon(a->circle, b->polygon, a->is_static, b->is_static, normal, depth);
    }
    
    float e = std::min(a->restitution, b->restitution);
    float relative_velocity = Vector2DotProduct(Vector2Subtract(b->velocity, a->velocity), normal);
    float mass_sum = (a->is_static ? 0.0f : 1.0f / a->mass) + (b->is_static ? 0.0f : 1.0f / b->mass);

    if (mass_sum == 0.0f) return;
    
    float j = -(1 + e) * relative_velocity / mass_sum;
    if( !a->is_static )
        a->velocity = Vector2Subtract(a->velocity, Vector2Scale(normal, j / a->mass));
    if( !b->is_static )
        b->velocity = Vector2Add(b->velocity, Vector2Scale(normal, j / b->mass));
}

void resolve_with_rotation(CollisionData contact)
{
    Body* bodyA = contact.a;
    Body* bodyB = contact.b;
    Vector2 normal = contact.normal;
    normal = Vector2Normalize(normal);

    if( bodyA->is_static && bodyB->is_static )
        return;
    
    if( bodyA->type == POLYGON )
    {
        if( bodyB->type == POLYGON )
            resolve_polygon_polygon(bodyA->polygon, bodyB->polygon, bodyA->is_static, bodyB->is_static, normal, contact.depth);
        if( bodyB->type == CIRCLE )
            resolve_polygon_circle(bodyA->polygon, bodyB->circle, bodyA->is_static, bodyB->is_static, normal, contact.depth);
    }
    if( bodyA->type == CIRCLE )
    {
        if( bodyB->type == POLYGON )
            resolve_circle_polygon(bodyA->circle, bodyB->polygon, bodyA->is_static, bodyB->is_static, normal, contact.depth);
    }

    
    Vector2 contact1 = contact.contact_1;
    Vector2 contact2 = contact.contact_2;
    int contactCount = contact.contact_count;

    get_contact_points(bodyA->polygon, bodyB->polygon, contact1, contact2, contactCount);

    float e = std::min(bodyA->restitution, bodyB->restitution);
    Vector2 contactList[2];
    Vector2 impulseList[2];
    Vector2 raList[2];
    Vector2 rbList[2];
    contactList[0] = contact1;
    contactList[1] = contact2;


    float bodyAInvMass = bodyA->mass > 0.0f ? 1.0f / bodyA->mass : 0.0f;
    float bodyBInvMass = bodyB->mass > 0.0f ? 1.0f / bodyB->mass : 0.0f;
    float bodyAInvInertia = bodyA->inertia > 0.0f ? 1.0f / bodyA->inertia : 0.0f;
    float bodyBInvInertia = bodyB->inertia > 0.0f ? 1.0f / bodyB->inertia : 0.0f;

    if( bodyA->is_static ) bodyAInvMass = 0.0f;
    if( bodyB->is_static ) bodyBInvMass = 0.0f;
    if( bodyA->rotation_static ) bodyAInvInertia = 0.0f;
    if( bodyB->rotation_static ) bodyBInvInertia = 0.0f;
    
    for(int i = 0; i < contactCount; i++)
    {
        impulseList[i] = {0, 0};
        raList[i] = {0, 0};
        rbList[i] = {0, 0};
    }

    for(int i = 0; i < contactCount; i++)
    {
        Vector2 ra = Vector2Subtract(contactList[i], bodyA->polygon->origin);
        Vector2 rb = Vector2Subtract(contactList[i], bodyB->polygon->origin);

        raList[i] = ra;
        rbList[i] = rb;

        Vector2 raPerp = Vector2(-ra.y, ra.x);
        Vector2 rbPerp = Vector2(-rb.y, rb.x);

        Vector2 angularLinearVelocityA = Vector2Scale(raPerp, bodyA->angular_velocity);
        Vector2 angularLinearVelocityB = Vector2Scale(rbPerp, bodyB->angular_velocity);

        Vector2 relativeVelocity = Vector2Subtract(Vector2Add(bodyB->velocity, angularLinearVelocityB), Vector2Add(bodyA->velocity, angularLinearVelocityA));

        float contactVelocityMag = Vector2DotProduct(relativeVelocity, normal);

        if (contactVelocityMag < 0.0f)
        {
            continue;
        }

        float raPerpDotN = Vector2DotProduct(raPerp, normal);
        float rbPerpDotN = Vector2DotProduct(rbPerp, normal);

        float denom = bodyAInvMass + bodyBInvMass + 
            (raPerpDotN * raPerpDotN) * bodyAInvInertia + 
            (rbPerpDotN * rbPerpDotN) * bodyBInvInertia;

        float j = -(1 + e) * contactVelocityMag;
        j /= denom;
        j /= (float)contactCount;

        Vector2 impulse = Vector2Scale(normal, j);
        impulseList[i] = impulse;
    }

    for(int i = 0; i < contactCount; i++)
    {
        Vector2 impulse = impulseList[i];
        Vector2 ra = raList[i];
        Vector2 rb = rbList[i];

        bodyA->velocity = Vector2Add(bodyA->velocity, Vector2Scale(impulse, -bodyAInvMass));
        bodyA->angular_velocity += -(ra.x * impulse.y - ra.y * impulse.x) * bodyAInvInertia;
        
        bodyB->velocity = Vector2Subtract(bodyB->velocity, Vector2Scale(impulse, -bodyBInvMass));
        bodyB->angular_velocity -= -(rb.x * impulse.y - rb.y * impulse.x) * bodyBInvInertia;
    }
}

void resolve_with_rotation_and_friction(CollisionData contact)
{
    Body* bodyA = contact.a;
    Body* bodyB = contact.b;
    Vector2 normal = contact.normal;
    normal = Vector2Normalize(normal);

    if( bodyA->is_static && bodyB->is_static )
        return;
    
    if( bodyA->type == POLYGON )
    {
        if( bodyB->type == POLYGON )
            resolve_polygon_polygon(bodyA->polygon, bodyB->polygon, bodyA->is_static, bodyB->is_static, normal, contact.depth);
        if( bodyB->type == CIRCLE )
            resolve_polygon_circle(bodyA->polygon, bodyB->circle, bodyA->is_static, bodyB->is_static, normal, contact.depth);
    }
    if( bodyA->type == CIRCLE )
    {
        if( bodyB->type == POLYGON )
            resolve_circle_polygon(bodyA->circle, bodyB->polygon, bodyA->is_static, bodyB->is_static, normal, contact.depth);
    }

    
    Vector2 contact1 = contact.contact_1;
    Vector2 contact2 = contact.contact_2;
    int contactCount = contact.contact_count;

    get_contact_points(bodyA->polygon, bodyB->polygon, contact1, contact2, contactCount);

    float e = std::min(bodyA->restitution, bodyB->restitution);

    float sf = (bodyA->friction + bodyB->friction) / 2;
    float fr = (bodyA->friction_ramp + bodyB->friction_ramp) / 2;
    
    Vector2 contactList[2];
    Vector2 impulseList[2];
    Vector2 friction_impulse_list[2];
    float jlist[2];
    Vector2 raList[2];
    Vector2 rbList[2];
    contactList[0] = contact1;
    contactList[1] = contact2;


    float bodyAInvMass = bodyA->mass > 0.0f ? 1.0f / bodyA->mass : 0.0f;
    float bodyBInvMass = bodyB->mass > 0.0f ? 1.0f / bodyB->mass : 0.0f;
    float bodyAInvInertia = bodyA->inertia > 0.0f ? 1.0f / bodyA->inertia : 0.0f;
    float bodyBInvInertia = bodyB->inertia > 0.0f ? 1.0f / bodyB->inertia : 0.0f;

    if( bodyA->is_static ) bodyAInvMass = 0.0f;
    if( bodyB->is_static ) bodyBInvMass = 0.0f;
    if( bodyA->rotation_static ) bodyAInvInertia = 0.0f;
    if( bodyB->rotation_static ) bodyBInvInertia = 0.0f;
    
    for(int i = 0; i < contactCount; i++)
    {
        impulseList[i] = {0, 0};
        raList[i] = {0, 0};
        rbList[i] = {0, 0};
    }

    // Angular
    for(int i = 0; i < contactCount; i++)
    {
        Vector2 ra = Vector2Subtract(contactList[i], bodyA->polygon->origin);
        Vector2 rb = Vector2Subtract(contactList[i], bodyB->polygon->origin);

        raList[i] = ra;
        rbList[i] = rb;

        Vector2 raPerp = Vector2(-ra.y, ra.x);
        Vector2 rbPerp = Vector2(-rb.y, rb.x);

        Vector2 angularLinearVelocityA = Vector2Scale(raPerp, bodyA->angular_velocity);
        Vector2 angularLinearVelocityB = Vector2Scale(rbPerp, bodyB->angular_velocity);

        Vector2 relativeVelocity = Vector2Subtract(Vector2Add(bodyB->velocity, angularLinearVelocityB), Vector2Add(bodyA->velocity, angularLinearVelocityA));

        float contactVelocityMag = Vector2DotProduct(relativeVelocity, normal);

        if (contactVelocityMag < 0.0f)
        {
            continue;
        }

        float raPerpDotN = Vector2DotProduct(raPerp, normal);
        float rbPerpDotN = Vector2DotProduct(rbPerp, normal);

        float denom = bodyAInvMass + bodyBInvMass + 
            (raPerpDotN * raPerpDotN) * bodyAInvInertia + 
            (rbPerpDotN * rbPerpDotN) * bodyBInvInertia;

        float j = -(1 + e) * contactVelocityMag;
        j /= denom;
        j /= (float)contactCount;

        Vector2 impulse = Vector2Scale(normal, j);
        impulseList[i] = impulse;
        jlist[i] = j;
    }

    for(int i = 0; i < contactCount; i++)
    {
        Vector2 impulse = impulseList[i];
        Vector2 ra = raList[i];
        Vector2 rb = rbList[i];

        bodyA->velocity = Vector2Add(bodyA->velocity, Vector2Scale(impulse, -bodyAInvMass));
        bodyA->angular_velocity += -(ra.x * impulse.y - ra.y * impulse.x) * bodyAInvInertia;
        
        bodyB->velocity = Vector2Subtract(bodyB->velocity, Vector2Scale(impulse, -bodyBInvMass));
        bodyB->angular_velocity -= -(rb.x * impulse.y - rb.y * impulse.x) * bodyBInvInertia;
    }

    // Friction
    for(int i = 0; i < contactCount; i++)
    {
        Vector2 ra = Vector2Subtract(contactList[i], bodyA->polygon->origin);
        Vector2 rb = Vector2Subtract(contactList[i], bodyB->polygon->origin);

        raList[i] = ra;
        rbList[i] = rb;

        Vector2 raPerp = Vector2(-ra.y, ra.x);
        Vector2 rbPerp = Vector2(-rb.y, rb.x);

        Vector2 angularLinearVelocityA = Vector2Scale(raPerp, bodyA->angular_velocity);
        Vector2 angularLinearVelocityB = Vector2Scale(rbPerp, bodyB->angular_velocity);

        Vector2 relativeVelocity = Vector2Subtract(Vector2Add(bodyB->velocity, angularLinearVelocityB), Vector2Add(bodyA->velocity, angularLinearVelocityA));

        Vector2 tangent = Vector2Subtract(relativeVelocity, Vector2Scale(normal, Vector2DotProduct(relativeVelocity, normal)));

        if( Vector2Equals(tangent, Vector2{0, 0}) )
            continue;
        
        tangent = Vector2Normalize(tangent);
        
        float raPerpDotT = Vector2DotProduct(raPerp, tangent);
        float rbPerpDotT = Vector2DotProduct(rbPerp, tangent);

        float denom = bodyAInvMass + bodyBInvMass + 
            (raPerpDotT * raPerpDotT) * bodyAInvInertia + 
            (rbPerpDotT * rbPerpDotT) * bodyBInvInertia;

        float jt = Vector2DotProduct(relativeVelocity, tangent);
        jt /= denom;
        jt /= (float)contactCount;
        Vector2 friction_impulse = {0, 0};
        if( std::abs(jt) <= jlist[i] * sf)
        {
            friction_impulse = Vector2Scale(tangent, jt);
        }
        else
        {
            friction_impulse = Vector2Scale(tangent, -jlist[i] * fr);
        }
        
        friction_impulse_list[i] = friction_impulse;
    }

    for(int i = 0; i < contactCount; i++)
    {
        Vector2 friction_impulse = friction_impulse_list[i];
        Vector2 ra = raList[i];
        Vector2 rb = rbList[i];

        bodyA->velocity = Vector2Subtract(bodyA->velocity, Vector2Scale(friction_impulse, -bodyAInvMass));
        bodyA->angular_velocity += (ra.x * friction_impulse.y - ra.y * friction_impulse.x) * bodyAInvInertia;
        
        bodyB->velocity = Vector2Add(bodyB->velocity, Vector2Scale(friction_impulse, -bodyBInvMass));
        bodyB->angular_velocity -= (rb.x * friction_impulse.y - rb.y * friction_impulse.x) * bodyBInvInertia;
    }
}

void Collision::i_add_collider(Body* collider)
{
    colliders.push_back(collider);
}

void Collision::i_update(int substeps)
{
    
    float step_time = GetFrameTime() / substeps;

    for(int step = 0; step < substeps; step++)
    {
        for(int i = 0; i < colliders.size(); i++)
        {
            
            Body* a = colliders[i];
            a->rotate(a->angular_velocity * step_time);
            if(colliders[i]->is_static)
                continue;

            Vector2 acceleration = Vector2Scale(a->force, (1.0f / a->mass));
            a->velocity = Vector2Add(a->velocity, Vector2Scale(acceleration, step_time));
            a->velocity = Vector2Add(a->velocity, Vector2Scale(GRAVITY, step_time));
            a->force = {0, 0};
            a->translate(Vector2Scale(a->velocity, step_time));
            if( a->polygon->origin.x < 0 || a->polygon->origin.x > GetScreenWidth() || a->polygon->origin.y < 0 || a->polygon->origin.y > GetScreenHeight() )
            {
                const bool t = true;
            }

            // Damping
           // a->velocity = Vector2Scale(a->velocity, 0.999f);
           // a->angular_velocity *= 0.999f;

            for(int j = 0; j < colliders.size(); j++)
            {
                if(i == j)
                    continue;

                Vector2 normal;
                float depth;
                check_collision(colliders[i], colliders[j], normal, depth);
            }
        }
    }
    for(const auto& collision: collisions)
    {
        resolve_with_rotation_and_friction(collision);
    }
    collisions.clear();
}


void Collision::i_draw_debug() const
{
    for( const auto& collider: colliders )
    {
        if( collider->type == POLYGON )
        {
            for( int i = 0; i < collider->polygon->points.size(); i++ )
            {
                if( !collider->is_static )
                {
                    DrawCircle(collider->polygon->origin.x, collider->polygon->origin.y, 2, WHITE);
                    DrawLineEx(collider->polygon->points[i], collider->polygon->points[(i + 1) % collider->polygon->points.size()], 2, RED);
                }
                else
                {
                    DrawCircle(collider->polygon->origin.x, collider->polygon->origin.y, 2, WHITE);
                    DrawLineEx(collider->polygon->points[i], collider->polygon->points[(i + 1) % collider->polygon->points.size()], 2, SKYBLUE);
                }
            }
        }
        if( collider->type == CIRCLE )
        {
            DrawCircle(collider->circle->origin.x, collider->circle->origin.y, collider->circle->radius, RED);
            DrawLine( collider->circle->origin.x, collider->circle->origin.y, collider->circle->origin.x + collider->circle->radius, collider->circle->origin.y, RED);
        }
    }
}
