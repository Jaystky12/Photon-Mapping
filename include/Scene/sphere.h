#ifndef SPHERE_H
#define SPHERE_H


#include "surface.h"
#include "../Math/random.h"
class Sphere : public Surface {

public:
    double radius;
    Vector position;
    
    Sphere(double radius_, Vector position_, Vector emission_,
           Vector color_, Refl_t refl_, double ior_);


    double intersect(const Ray &ray) const override;

    Vector get_normal(const Vector &hitpoint) const override;

    double get_area() const override;
	
	Vector get_uniform_area_pos(Random* random) const override;

    bool solve_quadratic(const double &a, const double &b, const double &c, double &x0, double &x1) const;
};

#endif