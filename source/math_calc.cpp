#include "../include/Math/math_calc.h"

using namespace std;

Vector uniform_spherical_sampling(const Vector &dir, Random *random) {

	Vector sw = dir;
    Vector su;

	sw = dir;
	if (fabs(sw.x) > 0.1)
		su = Vector(0.0, 1.0, 0.0);
	else
		su = Vector(1.0, 0.0, 1.0);

	su = (su.cross(sw)).normalized();
	Vector sv = sw.cross(su);

	double r1 = random->rand_double();
	double r2 = random->rand_double();
	double phi = 2.0 * M_PI * r1;

	Vector l = su * cos(phi) * sqrt(r2) +
		sv * sin(phi) *  sqrt(r2) +
		sw *  sqrt(1.0 - r2);

	l = l.normalized();
    
	return l;
}

Vector uniform_spherical_sampling2(const Vector &dir, const double &spread_max, Random *random) {

    /* Set up local orthogonal coordinate system su,sv,sw towards direction vector */
    Vector sw = dir;
    Vector su;

    if (fabs(sw.x) > 0.1)
        su = Vector(0.0, 1.0, 0.0);
    else
        su = Vector(1.0, 0.0, 0.0);

    su = (su.cross(sw)).normalized();
    Vector sv = sw.cross(su);

    /* Create random sample direction l towards spherical direction vector */
    /* spread_max is the spread of the rays */

    double r1 = random->rand_double();
	double r2 = random->rand_double();

    double spread = 1.0 - r1 + r1 * spread_max;
    double sin_a = sqrt(1.0 - spread * spread);
    double phi = 2.0 * M_PI * r2;

    Vector l = su * cos(phi) * sin_a +
               sv * sin(phi) * sin_a +
               sw * spread;

    l = l.normalized();

    return l;
}