#ifndef MATHCALC_H
#define MATHCALC_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include "vector.h"
#include "random.h"

/* Create random sample direction l towards spherical direction vector	*/
/* cos_a_max controls the spread of the rays [1,-1]						*/
Vector uniform_spherical_sampling(const Vector &dir, Random *random);

Vector uniform_spherical_sampling2(const Vector &dir, const double &cos_a_max, Random *random);

#endif