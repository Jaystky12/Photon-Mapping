#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "../Photon_Map/photon_map.h"
#include "../Scene/scene.h"

class ray_tracer {
    public:

        Color BackgroundColor;

        double   gatherRadius;
        int      gatherCount;

        double   gatherRadius_caustics;
        int      gatherCount_caustics;

        ray_tracer(int gR, int gC, int gRC, int gCC, Color BC);

        Color trace_ray(scene* scene, const Ray &ray, int depth, Random* random, photon_map* photon_map, int E, bool caustics, bool direct_illumination, bool path_tracing);
};

#endif