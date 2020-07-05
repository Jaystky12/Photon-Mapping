#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "surface.h"

class Triangle : public Surface {
public:
    Vector p0, p1, p2;
    Vector edge_a, edge_b;
    Vector normal;

    double a_len, b_len;    /* Edge lengths */
    double area;

    Triangle();

    Triangle(const Vector p0_, const Vector &p1_, const Vector &p2_,
             const Color &emission_, const Color &color_, double ior_, Refl_t refl_);

    /* Triangle-ray intersection */
    double intersect(const Ray &ray) const override ;

    Vector get_normal(const Vector &hitpoint) const override ;
    double get_area() const override ;
	/* Uniform random sampling of a area point of a triangle */
	Vector get_uniform_area_pos(Random* random) const override ;
};

#endif