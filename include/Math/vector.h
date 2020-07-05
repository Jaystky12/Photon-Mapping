#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "random.h"
using namespace std;
/*------------------------------------------------------------------
| Struct for standard Vector operations in 3D 
| (used for points, vectors, and colors)
------------------------------------------------------------------*/
struct Vector {
    union {
        struct {
            double x;
            double y;
            double z;
        };
        double a[3];  
    };         /* Position XYZ or color RGB */

    Vector(const Vector &b) : x(b.x), y(b.y), z(b.z) {}

    Vector(double x_ = 0, double y_ = 0, double z_ = 0) : x(x_), y(y_), z(z_) {}

    Vector operator+(const Vector &b) const {
        return Vector(x + b.x, y + b.y, z + b.z);
    }

    Vector operator-(const Vector &b) const {
        return Vector(x - b.x, y - b.y, z - b.z);
    }

    Vector operator/(double c) const {
        return Vector(x / c, y / c, z / c);
    }

    Vector operator*(double c) const {
        return Vector(x * c, y * c, z * c);
    }

    friend Vector operator*(double c, const Vector &b) {
        return b * c;
    }

    Vector multiply(const Vector &b) const {
        return Vector(x * b.x, y * b.y, z * b.z);
    }

    const Vector normalized() const {
        return Vector(x, y, z) / sqrt(x * x + y * y + z * z);
    }

    const double dot(const Vector &b) const {
        return x * b.x + y * b.y + z * b.z;
    }

    const Vector cross(const Vector &b) const {
        return Vector((y * b.z) - (z * b.y),
                      (z * b.x) - (x * b.z),
                      (x * b.y) - (y * b.x));
    }

    const double max() {
        return fmax(z, fmax(x, y));
    }

    const double length_squared() const
    {
        return x*x + y*y + z*z;
    }

    const double length() const
    {
        return sqrt(length_squared());
    }

    Vector &clamp() {
        x = x < 0 ? 0.0 : x > 1.0 ? 1.0 : x;
        y = y < 0 ? 0.0 : y > 1.0 ? 1.0 : y;
        z = z < 0 ? 0.0 : z > 1.0 ? 1.0 : z;
        return *this;
    }
};

typedef Vector Color;
/*------------------------------------------------------------------
| Structure for rays (e.g. viewing ray, path tracing)
------------------------------------------------------------------*/
struct Ray {
    Vector org, dir;    /* Origin and direction */
    Ray(const Vector org_, const Vector &dir_) : org(org_), dir(dir_) {}
};

#endif