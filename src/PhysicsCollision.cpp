#include "PhysicsCollision.h"
#include "PhysicsBody.h"
#include <raymath.h>

namespace Physics
{
    
    CollisionData::CollisionData(Body* body_a, Body* body_b, Vector2 normal, float depth, Vector2 contact1,
        Vector2 contact2, int contact_count)
    {
        this->body_a = body_a;
        this->body_b = body_b;
        this->normal = normal;
        this->depth = depth;
        this->contact1 = contact1;
        this->contact2 = contact2;
        this->contact_count = contact_count;
    }

    void Collisions::point_segment_distance(Vector2 p, Vector2 a, Vector2 b, float& distance_squared,
                                                     Vector2& contact_point)
    {
        Vector2 ab = Vector2Subtract(b, a);
        Vector2 ap = Vector2Subtract(p, a);

        float proj = Vector2DotProduct(ap, ab);
        float abLenSq = Vector2LengthSqr(ab);
        float d = proj / abLenSq;

        if(d <= 0.0f)
        {
            contact_point = a;
        }
        else if(d >= 1.0f)
        {
            contact_point = b;
        }
        else
        {
            contact_point = Vector2Add(a, Vector2Scale(ab, d));
        }

        distance_squared = Vector2DistanceSqr(p, contact_point);
    }

    void Collisions::find_contact_points(Body body_a, Body body_b, Vector2& contact1, Vector2& contact2, int& contact_count)
    {
        contact1 = {0, 0};
        contact2 = {0, 0};
        contact_count = 0;

        ShapeType shapeTypeA = body_a.shape_type;
        ShapeType shapeTypeB = body_b.shape_type;

        if (shapeTypeA == BOX)
        {
            if (shapeTypeB == BOX)
            {
                find_polygons_contact_points(body_a.get_transformed_vertices(), body_b.get_transformed_vertices(),
                    contact1, contact2, contact_count);
            }
            else if (shapeTypeB == CIRCLE)
            {
                find_circle_polygon_contact_point(body_b.get_position(), body_b.radius, body_a.get_position(), body_a.get_transformed_vertices(), contact1);
                contact_count = 1;
            }
        }
        else if (shapeTypeA == CIRCLE)
        {
            if (shapeTypeB == BOX)
            {
                find_circle_polygon_contact_point(body_a.get_position(), body_a.radius, body_b.get_position(), body_b.get_transformed_vertices(), contact1);
                contact_count = 1;
            }
            else if (shapeTypeB == CIRCLE)
            {
                find_circles_contact_point(body_a.get_position(), body_a.radius, body_b.get_position(), contact1);
                contact_count = 1;
            }
        }
    }

    bool Collisions::check_collision(Body body_a, Body body_b, Vector2& normal, float& depth)
    {
        normal = {0, 0};
        depth = 0.0f;

        ShapeType shapeTypeA = body_a.shape_type;
        ShapeType shapeTypeB = body_b.shape_type;

        if (shapeTypeA == BOX)
        {
            if (shapeTypeB == BOX)
            {
                return intersect_polygons(
                    body_a.get_position(), body_a.get_transformed_vertices(),
                    body_b.get_position(), body_b.get_transformed_vertices(),
                    normal, depth);
            }
            else if (shapeTypeB == CIRCLE)
            {
                bool result = intersect_circle_polygon(
                    body_b.get_position(), body_b.radius,
                    body_a.get_position(), body_a.get_transformed_vertices(),
                    normal, depth);

                normal = Vector2Negate(normal);
                return result;
            }
        }
        else if (shapeTypeA == CIRCLE)
        {
            if (shapeTypeB == BOX)
            {
                return intersect_circle_polygon(
                    body_a.get_position(), body_a.radius,
                    body_b.get_position(), body_b.get_transformed_vertices(),
                    normal, depth);
            }
            else if (shapeTypeB == CIRCLE)
            {
                return intersect_circles(
                    body_a.get_position(), body_a.radius,
                    body_b.get_position(), body_b.radius,
                    normal, depth);
            }
        }

        return false;
    }

    bool Collisions::intersect_circle_polygon(Vector2 circle_center, float circle_radius, Vector2 polygon_center,
        std::vector<Vector2> vertices, Vector2& normal, float& depth)
    {
        normal = {0, 0};
        depth = INFINITY;

        Vector2 axis = {0, 0};
        float axisDepth = 0.0f;
        float minA, maxA, minB, maxB;

        for (int i = 0; i < vertices.size(); i++)
        {
            Vector2 va = vertices[i];
            Vector2 vb = vertices[(i + 1) % vertices.size()];

            Vector2 edge = Vector2Subtract(vb, va);
            axis = Vector2(-edge.y, edge.x);
            axis = Vector2Normalize(axis);

            project_vertices(vertices, axis, minA, maxA);
            project_circle(circle_center, circle_radius, axis, minB, maxB);

            if (minA >= maxB || minB >= maxA)
            {
                return false;
            }

            axisDepth = std::min(maxB - minA, maxA - minB);

            if (axisDepth < depth)
            {
                depth = axisDepth;
                normal = axis;
            }
        }

        int cpIndex = find_closest_point_on_polygon(circle_center, vertices);
        Vector2 cp = vertices[cpIndex];

        axis = Vector2Subtract(cp, circle_center);
        axis = Vector2Normalize(axis);

        project_vertices(vertices, axis, minA, maxA);
        project_circle(circle_center, circle_radius, axis, minB, maxB);

        if (minA >= maxB || minB >= maxA)
        {
            return false;
        }

        axisDepth = std::min(maxB - minA, maxA - minB);

        if (axisDepth < depth)
        {
            depth = axisDepth;
            normal = axis;
        }

        Vector2 direction = Vector2Subtract(polygon_center, circle_center);

        if (Vector2DotProduct(direction, normal) < 0.0f)
        {
            normal = Vector2Negate(normal);
        }

        return true;
    }

    bool Collisions::intersect_polygons(Vector2 center_a, std::vector<Vector2> vertices_a, Vector2 center_b,
        std::vector<Vector2> vertices_b, Vector2& normal, float& depth)
    {
         normal = {0, 0};
            depth = INFINITY;

            for (int i = 0; i < vertices_a.size(); i++)
            {
                Vector2 va = vertices_a[i];
                Vector2 vb = vertices_a[(i + 1) % vertices_a.size()];

                Vector2 edge = Vector2Subtract(vb, va);
                Vector2 axis = Vector2(-edge.y, edge.x);
                axis = Vector2Normalize(axis);

                float minA, maxA, minB, maxB;
                project_vertices(vertices_a, axis, minA, maxA);
                project_vertices(vertices_b, axis, minB, maxB);

                if (minA >= maxB || minB >= maxA)
                {
                    return false;
                }

                float axisDepth = std::min(maxB - minA, maxA - minB);

                if (axisDepth < depth)
                {
                    depth = axisDepth;
                    normal = axis;
                }
            }

            for (int i = 0; i < vertices_b.size(); i++)
            {
                Vector2 va = vertices_b[i];
                Vector2 vb = vertices_b[(i + 1) % vertices_b.size()];

                Vector2 edge = Vector2Subtract(vb, va);
                Vector2 axis = Vector2(-edge.y, edge.x);
                axis = Vector2Normalize(axis);
                float minA, maxA, minB, maxB;
                project_vertices(vertices_a, axis, minA, maxA);
                project_vertices(vertices_b, axis, minB, maxB);

                if (minA >= maxB || minB >= maxA)
                {
                    return false;
                }

                float axisDepth = std::min(maxB - minA, maxA - minB);

                if (axisDepth < depth)
                {
                    depth = axisDepth;
                    normal = axis;
                }
            }

            Vector2 direction =  Vector2Subtract(center_b, center_a);

            if (Vector2DotProduct(direction, normal) < 0.0f)
            {
                normal = Vector2Negate(normal);
            }

            return true;
    }

    bool Collisions::intersect_circles(Vector2 center_a, float radius_a, Vector2 center_b, float radius_b,
        Vector2& normal, float& depth)
    {
        normal = {0, 0};
        depth = 0.0f;

        float distance = Vector2Distance( center_a,  center_b);
        float radii = radius_a + radius_b;

        if(distance >= radii)
        {
            return false;
        }

        normal = Vector2Normalize( Vector2Subtract(center_b, center_a));
        depth = radii - distance;

        return true;
    }

    void Collisions::find_polygons_contact_points(std::vector<Vector2> vertices_a, std::vector<Vector2> vertices_b,
                                                  Vector2& contact1, Vector2& contact2, int& contact_count)
    {
        contact1 = {0, 0};
            contact2 = {0, 0};
            contact_count = 0;

            float minDistSq = INFINITY;

            for(int i = 0; i < vertices_a.size(); i++)
            {
                Vector2 p = vertices_a[i];

                for(int j = 0; j < vertices_b.size(); j++)
                {
                    Vector2 va = vertices_b[j];
                    Vector2 vb = vertices_b[(j + 1) % vertices_b.size()];

                    float distSq;
                    Vector2 cp;
                    point_segment_distance(p, va, vb, distSq, cp);

                    if(FloatEquals(distSq, minDistSq))
                    {
                        if (!Vector2Equals(cp, contact1))
                        {
                            contact2 = cp;
                            contact_count = 2;
                        }
                    }
                    else if(distSq < minDistSq)
                    {
                        minDistSq = distSq;
                        contact_count = 1;
                        contact1 = cp;
                    }
                }
            }

            for (int i = 0; i < vertices_b.size(); i++)
            {
                Vector2 p = vertices_b[i];

                for (int j = 0; j < vertices_a.size(); j++)
                {
                    Vector2 va = vertices_a[j];
                    Vector2 vb = vertices_a[(j + 1) % vertices_a.size()];
                    float distSq;
                    Vector2 cp;
                    point_segment_distance(p, va, vb, distSq, cp );

                    if (FloatEquals(distSq, minDistSq))
                    {
                        if (!Vector2Equals(cp, contact1))
                        {
                            contact2 = cp;
                            contact_count = 2;
                        }
                    }
                    else if (distSq < minDistSq)
                    {
                        minDistSq = distSq;
                        contact_count = 1;
                        contact1 = cp;
                    }
                }
            }
    }

    void Collisions::find_circle_polygon_contact_point(Vector2 circle_center, float circle_radius,
        Vector2 polygon_center, std::vector<Vector2> polygon_vertices, Vector2& contact_point)
    {
        contact_point = {0, 0};

        float minDistSq = INFINITY;

        for(int i = 0; i < polygon_vertices.size(); i++)
        {
            Vector2 va = polygon_vertices[i];
            Vector2 vb = polygon_vertices[(i + 1) % polygon_vertices.size()];
            float distSq;
            Vector2 contact;
            point_segment_distance(circle_center, va, vb, distSq, contact);

            if(distSq < minDistSq)
            {
                minDistSq = distSq;
                contact_point = contact;
            }
        }
    }

    void Collisions::find_circles_contact_point(Vector2 center_a, float radius_a, Vector2 center_b,
        Vector2& contact_point)
    {
        Vector2 ab =  Vector2Subtract(center_b, center_a);
        Vector2 dir = Vector2Normalize(ab);
        contact_point = Vector2Add(center_a, Vector2Scale(dir, radius_a));
    }

    int Collisions::find_closest_point_on_polygon(Vector2 circle_center, std::vector<Vector2> vertices)
    {
        int result = -1;
        float minDistance = INFINITY;

        for(int i = 0; i < vertices.size(); i++)
        {
            Vector2 v = vertices[i];
            float distance = Vector2Distance(v, circle_center);

            if(distance < minDistance)
            {
                minDistance = distance;
                result = i;
            }
        }

        return result;
    }

    void Collisions::project_circle(Vector2 center, float radius, Vector2 axis, float& min, float& max)
    {
        Vector2 direction = Vector2Normalize(axis);
        Vector2 directionAndRadius = Vector2Scale(direction, radius);

        Vector2 p1 = Vector2Add(center, directionAndRadius);
        Vector2 p2 = Vector2Subtract(center, directionAndRadius);

        min = Vector2DotProduct(p1, axis);
        max = Vector2DotProduct(p2, axis);

        if(min > max)
        {
            // swap the min and max values.
            float t = min;
            min = max;
            max = t;
        }
    }

    void Collisions::project_vertices(std::vector<Vector2> vertices, Vector2 axis, float& min, float& max)
    {
        min = INFINITY;
        max = -INFINITY;

        for(int i = 0; i < vertices.size(); i++)
        {
            Vector2 v = vertices[i];
            float proj = Vector2DotProduct(v, axis);

            if(proj < min) { min = proj; }
            if(proj > max) { max = proj; }
        }
    }
}
