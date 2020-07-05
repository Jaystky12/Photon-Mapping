#ifndef SCENE_H
#define SCENE_H

#include "sphere.h"
#include "triangle.h"
/******************************************************************
* Hard-coded scene definition:
*
* Spheres:
* - radius, center 
* - emitted light (light sources), surface reflectivity (~color), 
*   ?material
*
* Triangles:
* - vector to corner(origin), edge a, edge b
* - emitted light energy (light sources), surface reflectivity (~color),
*   ?material
*******************************************************************/

class scene
{
public:
    vector<Surface *> surfaces;
    
    vector<Sphere *> lights;

    Vector emission = Vector();

    scene();
    
    scene(const char* filename, Vector translation, double scale);
    
    ~scene();

    /******************************************************************
	* Check for closest intersection of a ray with the scene;
	* returns true if intersection is found, as well as ray parameter
	* of intersection and id of intersected object
	*******************************************************************/
	bool intersect(const Ray &ray, double &t, int &id);

    void add_OBJ_file(const char* filename, Vector translation, double scale);

    void add_sphere(Sphere* sphere);
    
    void add_triangle(Triangle* triangle);
};


#endif