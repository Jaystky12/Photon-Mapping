#include "../include/Scene/triangle.h"

using namespace std;

Triangle :: Triangle(const Vector p0_, const Vector &p1_, const Vector &p2_,
             const Color &emission_, const Color &color_, double ior_, Refl_t refl_ = DIFF) :
            Surface(emission_, color_, refl_ ,ior_), p0(p0_), p1(p1_), p2(p2_) {
        edge_a = p1 - p0;
		edge_b = p2 - p0;
        normal = edge_a.cross(edge_b);
        normal = normal.normalized();
        a_len = edge_a.length();
        b_len = edge_b.length();
        area = a_len * b_len / 2;
    }

Triangle :: Triangle(){}
 
double Triangle :: intersect(const Ray &ray) const 
    {
        
        Vector pvec = ray.dir.cross(edge_b);
        double det = edge_a.dot(pvec);

        if (fabs(det) < 1e-8) {
            return 0.0;
        }

        double invDet = 1 / det;

        Vector tvec = ray.org - p0;
        double u = tvec.dot(pvec) * invDet;

        if (u < 0 || u > 1) return 0.0;

        Vector qvec = tvec.cross(edge_a);
        double v = ray.dir.dot(qvec) * invDet;

        if (v < 0 || u + v > 1) return 0.0;

        double t = edge_b.dot(qvec) * invDet;

        return t;
    }

Vector Triangle :: get_normal(const Vector &hitpoint) const
    {
        return normal;
    }

double Triangle :: get_area() const 
    {
		return edge_a.length() * edge_b.length() / 2;
	}

Vector Triangle :: get_uniform_area_pos(Random* random) const
    {
		double eps1 = 1 - sqrt(random->rand_double());
		double eps2 = random->rand_double() * sqrt(eps1);

		return eps1 * p0 + eps2 * (p0 + edge_a) + (1 - eps1 - eps2) * (p0 + edge_b);
	}