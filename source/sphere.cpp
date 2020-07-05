#include "../include/Scene/sphere.h"

using namespace std;

Sphere :: Sphere(double radius_, Vector position_, Vector emission_,
           Vector color_, Refl_t refl_, double ior_) : Surface(emission_, color_, refl_, ior_),
           radius(radius_), position(position_) {}

double Sphere :: intersect(const Ray &ray) const 
    {
        double t0, t1, t;
        Vector L = ray.org - position;
        double a = ray.dir.dot(ray.dir);
        double b = 2 * ray.dir.dot(L);
        double c = L.dot(L) - (radius * radius);

        if (!solve_quadratic(a, b, c, t0, t1)) return 0.0; 
        if (t0 > t1) std::swap(t0, t1); 
        
        if (t0 < 0) { 
            t0 = t1; // if t0 is negative, let's use t1 instead 
            if (t0 < 0) return 0.0; // both t0 and t1 are negative 
        } 
 
        t = t0; 
 
        return t;        /* No intersection in ray direction */
    }
Vector Sphere :: get_normal(const Vector &hitpoint) const 
    {
        return (hitpoint - position).normalized();
    }

double Sphere :: get_area() const
    {
		return 4 * M_PI * radius * radius;
	}

Vector Sphere :: get_uniform_area_pos(Random* random) const 
    {
		/* Distribute rays evenly accross the whole area of the light source [cos_a_max = -1.0] */

        const auto r1 = 2.0 * M_PI * random->rand_double();
        const auto r2 = 1.0 - 2.0 * random->rand_double();
        	
        return position + radius * Vector(sqrt(1.0 - r2 * r2) * cos(r1), sqrt(1.0 - r2 * r2) * sin(r1), r2);
	}

bool Sphere :: solve_quadratic(const double &a, const double &b, const double &c, double &x0, double &x1) const {

    double discr = b * b - 4 * a * c; 
    if (discr < 0) return false; 
    else if (discr == 0) { 
        x0 = x1 = - 0.5 * b / a; 
    } 
    else { 
        double q = (b > 0) ? 
            -0.5 * (b + sqrt(discr)) : 
            -0.5 * (b - sqrt(discr)); 
        x0 = q / a; 
        x1 = c / q; 
    } 

    return true;        
} 
