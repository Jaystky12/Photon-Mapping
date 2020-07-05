#ifndef SURFACE_H
#define SURFACE_H

#include "../Math/math_calc.h"
#include "../Math/vector.h"
/*------------------------------------------------------------------
| Scene objects are spheres; material either perfectly diffuse, 
| specular (mirror reflection) or transparent (refraction/reflection)
| (DIFFuse, SPECular, REFRactive)
------------------------------------------------------------------*/

enum Refl_t {
    DIFF, SPEC, REFR, GLOS
};

class Surface {
public:
    Color emission;

    Color color;
    Refl_t refl;
    double ior; // index of refraction
    Surface(){}
    Surface(Vector emission_, Vector color_, Refl_t refl_, double ior_) :
    emission(emission_), color(color_), refl(refl_), ior(ior_) {}

    virtual double intersect(const Ray &ray) const = 0;
    virtual Vector get_normal(const Vector &hitpoint) const = 0;
    virtual double get_area() const = 0;
	virtual Vector get_uniform_area_pos(Random* random) const = 0;

};

#endif