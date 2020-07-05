#include "../include/Ray_Tracer/ray_tracer.h"
#include "../include/Photon_Map/nearest_photon.h"

using namespace std;

ray_tracer :: ray_tracer (int gR, int gC, int gRC, int gCC, Color BC)
    {
        BackgroundColor = BC;
        gatherRadius = gR;
        gatherCount = gC;
        gatherRadius_caustics = gRC;
        gatherCount_caustics = gCC;
    }

Color ray_tracer :: trace_ray (scene* scene, const Ray &ray, int depth, Random* random, photon_map* photon_map, int E, bool caustics, bool direct_illumination, bool path_tracing)
    {
        double t;
        int id = 0;

        if (!scene->intersect(ray, t, id))   /* No intersection with scene */
            return BackgroundColor;

        const Surface* obj = scene->surfaces[id];
        Vector hitpoint = ray.org + ray.dir * t;    /* intersection point */
        Vector normal = obj->get_normal(hitpoint);  /* Normal at intersection */

        Vector nl = normal;

        /* Obtain flipped normal, if object hit from inside */
        if (normal.dot(ray.dir) >= 0)
            nl = nl * -1.0;

        Color col = obj->color;

        /* maximum RGB reflectivity for Russian Roulette */
        double p = col.max();


        if (depth > 5 || !p)   /* After 5 bounces or if max reflectivity is zero */
        {
            if (random->rand_double() < p) /* Russian Roulette */
                col = col * (1 / p);        /* Scale estimator to remain unbiased */
            else
                return obj->emission * E;  /* No further bounces, only return potential emission */
        }

        switch (obj->refl)
        {
        case DIFF: // Diffuse
        {
            Vector e;

            if(direct_illumination){
                //cout << int(sizeof(scene->lights) /  sizeof(Sphere))<< endl;
                for (int i = 0; i < int(scene->lights.size()); i++) {
                    const Sphere* light = scene->lights[i];
                    
                    Vector ldir = light->position - hitpoint;
                        
                    if (scene->intersect(Ray((hitpoint+nl * 0.001), (ldir)), t, id) && id == i) {
                        Vector lemission = light->emission / (4 * M_PI * ldir.length_squared());
                        e = e + col.multiply((lemission) * max(0.0, nl.dot(ldir)));
                    }    
                }
                return obj->emission + e;
            }
            else if(path_tracing){
            
                /* Random reflection vector d */
                Vector d = uniform_spherical_sampling(nl, random);

                /* Explicit computation of direct lighting */
                
                for (int i = 0; i < int(scene->lights.size()); i++) {
                    const Sphere* light = scene->lights[i];
                   
                    /* Randomly sample spherical light source from surface intersection */
                    Vector ldir = light->position - hitpoint;

                    double cos_max = sqrt(1.0 - light->radius * light->radius /
                                                (hitpoint - light->position).dot(hitpoint - light->position));

                    Vector l = uniform_spherical_sampling2(ldir, cos_max, random);

                    /* Shoot shadow ray */
                    if (scene->intersect(Ray(hitpoint + nl* 0.001, l), t, id) && id == i) {
                        double o = 2 * M_PI * (1 - cos_max);

                        /* Add diffusely reflected light from light source - constant BRDF 1/PI */
                        e = e + col.multiply(light->emission * l.dot(nl) * o) * M_1_PI;
                    }
                }

                /* Return potential light emission, direct lighting, and indirect lighting (via
                recursive call for Monte-Carlo integration */
                return obj->emission * E + e + col.multiply(trace_ray(scene, Ray(hitpoint+nl* 0.001, d), depth + 1, random, photon_map, 0, caustics, direct_illumination, path_tracing));
            }
            else {
                //Calculate global illumination or caustics
                int radius = gatherRadius;
                int count = gatherCount;
                if(caustics){
                    radius = gatherRadius_caustics;
                    count = gatherCount_caustics;
                }

                photon_query query;
                query.position = hitpoint;
                query.normal = nl;
                query.count = count;
                query.max_distance = radius * radius;

                nearest_photon result(count);
            
                photon_map->search_for_photons(query, result);

                Vector accumulated_flux(0, 0, 0);
                double max_distance2 = -1;

                for (unsigned int i = 0; i < result.size(); ++i)
                {
                    max_distance2 = std::max(max_distance2, result[i].dist2);
                }

                if(max_distance2 > 0) {
                    { // Cone filter
                        /* 
                            * https://www.cs.princeton.edu/courses/archive/fall16/cos526/lectures/03-photonmapping.pdf
                            */
                        Vector accumulated_flux(0, 0, 0);
                        const double max_distance = sqrt(max_distance2);
                        const double k = 1.6;
                    
                        for (unsigned int i = 0; i < result.size(); ++i)
                        {
                            const double w = 1.0 - (sqrt(result[i].dist2) / (k * max_distance));
                            const Vector v = col.multiply(result[i].point->flux) / M_PI;
                            accumulated_flux = accumulated_flux + w * v;
                        }
                        accumulated_flux = accumulated_flux / (1.0 - 2.0 / (3.0 * k));


                        if (max_distance2 > 0)
                        {
                            return obj->emission + accumulated_flux /(M_PI * (max_distance2));
                        }
                    }
                }
            }
            break;
        } // case DIFF

        case SPEC: // Specular
        {
            Vector reflection = ray.dir - normal * 2 * normal.dot(ray.dir);
            Ray refl_ray = Ray(hitpoint + nl*0.001, reflection);
            return obj->emission + col.multiply(trace_ray(scene, refl_ray, depth + 1, random, photon_map, 1, caustics, direct_illumination,path_tracing));
        } // case SPE

        case GLOS: // Glossy
        {
            Vector reflection = ray.dir - normal * 2 * normal.dot(ray.dir);
            Vector dir = uniform_spherical_sampling2(reflection, 0.9, random);
            Ray refl_ray = Ray(hitpoint + nl*0.001, dir);
            return obj->emission + col.multiply(trace_ray(scene, refl_ray, depth + 1, random, photon_map, 1, caustics, direct_illumination,path_tracing));
        } // case GLOS

        case REFR: // Refractant
        {
            bool into = normal.dot(nl) > 0;       /* Bool for checking if ray from outside going in */
            Vector refl_ray_orig = into ? (hitpoint + nl * 0.001) : (hitpoint - nl * 0.001);
            Vector reflection = ray.dir - normal * 2 * normal.dot(ray.dir);
            Ray refl_ray(refl_ray_orig, reflection); /* Prefect reflection */

            double nc = 1;                        /* Index of refraction of air (approximately) */
            //double nt = 1.5;                      /* Index of refraction of glass (approximately) */
            double nt = obj->ior;                      /* Index of refraction of hit object*/
            double nnt;

            if (!into)      /* Set ratio depending on hit from inside or outside */
                nnt = nt / nc;
            else
                nnt = nc / nt;

            double ddn = ray.dir.dot(nl);
            double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);

            if (cos2t < 0)
                return obj->emission + col.multiply(trace_ray(scene, refl_ray, depth + 1, random, photon_map, 1, caustics, direct_illumination,path_tracing));

            /* Otherwise reflection and/or refraction occurs */
            Vector tdir;
            
            /* Determine transmitted ray direction for refraction */
            if (into)
                tdir = (ray.dir * nnt - normal * (ddn * nnt + sqrt(cos2t))).normalized();
            else
                tdir = (ray.dir * nnt + normal * (ddn * nnt + sqrt(cos2t))).normalized();

            /* Determine R0 for Schlick's approximation */
            double a = nt - nc;
            double b = nt + nc;
            double R0 = a * a / (b * b);

            /* Cosine of correct angle depending on outside/inside */
            double c;
            if (into)
                c = 1 + ddn;
            else
                c = 1 - tdir.dot(normal);

            /* Compute Schlicks approximation of Fresnel equation */
            double Re = R0 + (1 - R0) * c * c * c * c * c;   /* Reflectance */
            double Tr = 1 - Re;                     /* Transmittance */

            /* Probability for selecting reflectance or transmittance */
            double P = .25 + .5 * Re;
            double RP = Re / P;         /* Scaling factors for unbiased estimator */
            double TP = Tr / (1 - P);

            Ray transRay((hitpoint - nl * 0.001), tdir); 

            if (depth < 3)   /* Initially both reflection and trasmission */
                return obj->emission + col.multiply(trace_ray(scene, refl_ray, depth + 1, random, photon_map, 1, caustics, direct_illumination,path_tracing) * Re +
                    trace_ray(scene, transRay, depth + 1, random, photon_map,1, caustics, direct_illumination,path_tracing) * Tr);
            else             /* Russian Roulette */
                if (random->rand_double() < P)
                    return obj->emission + col.multiply(trace_ray(scene, refl_ray, depth + 1, random, photon_map, 1, caustics, direct_illumination,path_tracing) * RP);
                else
                    return obj->emission + col.multiply(trace_ray(scene, transRay, depth + 1, random, photon_map,1, caustics, direct_illumination,path_tracing) * TP);

        } // case REFR

        } // switch

        return Vector();
    }