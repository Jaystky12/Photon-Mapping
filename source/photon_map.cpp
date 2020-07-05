#include "../include/Photon_Map/photon_map.h"
#include "../include/Photon_Map/nearest_photon.h"

using namespace std;

photon_map :: photon_map()
		: root(nullptr)
	{ /* DO_NOTHING */
	}

photon_map :: ~photon_map()
	{
		clear_photon_map();
	}

size_t photon_map :: size() const
	{
		return points.size();
	}

void photon_map :: search_for_photons(photon_query& query, nearest_photon& result)
	{
		recursive_search(query, root, result);
	}

void photon_map :: store(const photon& value)
	{
		points.emplace_back(value);
	}

void photon_map ::  build_photon_map()
	{
		points.shrink_to_fit();

        root = recursive_build(points.data(), int(points.size()), 0);
		
    }
void photon_map :: clear_photon_map()
	{
		recursive_clear(root);
	}

void photon_map ::recursive_search(photon_query& query, node* node, nearest_photon& result)
	{
		if (node == nullptr)
		{
			return;
		}

		const int  axis = node->axis;
		double     d = query.position.a[axis] - node->point->position.a[axis];
		const auto dir = node->point->position - query.position;
		const auto dist2 = dot(dir, dir);
		const auto dt = dot(query.normal, dir / sqrt(dist2));

		if (dist2 < query.max_distance && fabs(dt) <= query.max_distance * 0.1)
		{
			result.push(photon_query_result(node->point, dist2));
			if (result.size() > query.count)
			{
				result.pop();
				query.max_distance = result.top().dist2;
			}
		}

		if (d > 0.0)
		{
			recursive_search(query, node->right, result);
			if (d * d < query.max_distance)
			{
				recursive_search(query, node->left, result);
			}
		}
		else
		{
			recursive_search(query, node->left, result);
			if (d * d < query.max_distance)
			{
				recursive_search(query, node->right, result);
			}
		}
	}

photon_map::node* photon_map :: recursive_build(photon* photons, int count, int depth)
	{
		if (count <= 0)
		{
			return nullptr;
		}

		const int axis = depth % 3;
		const int middle = (count - 1) / 2;

		std::nth_element(photons, photons + middle, photons + count, less_comp(axis));

		node* node = new(std::nothrow) photon_map::node();

		node->axis = axis;

		node->point = &photons[middle];

		node->left = recursive_build(photons, middle, depth + 1);

		node->right = recursive_build(photons + middle + 1, count - middle - 1, depth + 1);

        return node;
	}

void photon_map :: recursive_clear(node* node)
	{
		if (node == nullptr)
		{
			return;
		}

		recursive_clear(node->left);
		recursive_clear(node->right);
		delete node;
	}

void photon_map :: generate_map (scene* scene, photon_map* photon_m, int photons, Random* random, bool caustics)
    {
        for (int i = 0; i < photons; ++i)
        {
            const Sphere* light = scene->lights[0];

            /* Get unifomly random point on surface */
            Vector pos = light->get_uniform_area_pos(random);
            
            /* Emit ray randomly from starting point in a half sphere [cos_a_max = 0.0] */
            Vector dir = uniform_spherical_sampling2(light->get_normal(pos), 0.0, random);

            /* Calulate flux emitted by light source */
            Vector flux = light->emission * light->get_area() * M_PI/ photons;

            /* Trace photon in the scene to store it in the photon map */
            Ray emitRay = Ray(pos, dir);
            
            trace_photons(scene, emitRay, flux, random, photon_m, caustics);
        }
    }
 
void photon_map :: trace_photons (scene* scene, const Ray& emit_ray, const Vector& emit_flux, Random* random, photon_map* photon_map, bool caustics_bool)
    {
        Ray ray = Ray(emit_ray.org, emit_ray.dir);
        Vector flux = emit_flux;
        
        int max_bounces = 5;
        bool caustics = caustics_bool;
        while (max_bounces)
        {
            max_bounces--;
            double t;
            int id = 0;

            /* Break if value of flux reaches 0, low flux photons not needed in the photon map*/
            if (flux.x <= 0 && flux.y <= 0 && flux.z <= 0)
                break;

            if (!scene->intersect(ray, t, id))
                break;

            
            const Surface* obj = scene->surfaces[id];
            Vector hitpoint = ray.org + ray.dir * t;    /* intersection point */
            Vector normal = obj->get_normal(hitpoint);  /* Normal at intersection */

            Vector nl = normal;

            /* Obtain flipped normal, if object hit from inside */
            if (normal.dot(ray.dir) >= 0)
                nl = nl * -1.0;

            Color col = obj->color;

            // if(max_bounces > 998 && !(obj->refl == REFR)) return;

            switch (obj->refl)
            {
            case DIFF: // Diffuse
            {
                photon photon;
                photon.position = hitpoint;
                photon.direction = ray.dir;
                photon.flux = flux;

                if(!caustics) return;
                
                photon_map->store(photon);

                /* Average RGB reflectivity for Russian Roulette */
                double p = (obj->color.x + obj->color.y + obj->color.z) / 3.0;

                if (random->rand_double() > p)	/* Russian Roulette */
                {
                    Vector dir = uniform_spherical_sampling(nl, random);
                    ray = Ray(hitpoint + nl * 0.001, dir);
                    flux = flux.multiply(col);
                }
                else
                {
                    return;		/* No further bounces*/
                }
                break;
            } // case DIFF

            case SPEC: // Specular
            {
                Vector reflection = ray.dir - normal * 2 * normal.dot(ray.dir);
                ray = Ray(hitpoint + nl * 0.001, reflection);
                flux = col.multiply(flux);
                caustics = true;
                break;
            } // case SPE

            case GLOS: // Glossy
            {
                Vector reflection = ray.dir - normal * 2 * normal.dot(ray.dir);
                Vector dir = uniform_spherical_sampling2(reflection, 0.9, random);
                ray = Ray(hitpoint + nl * 0.001, dir);
                flux = col.multiply(flux);
                break;
            } // case GLOS

            case REFR: // Refractant
            {
                caustics = true;
                Vector reflection = ray.dir - normal * 2 * normal.dot(ray.dir);
                Ray reflRay(hitpoint + nl * 0.001, reflection);  /* Prefect reflection */

                bool into = normal.dot(nl) > 0;       /* Bool for checking if ray from outside going in */
                double nc = 1;                        /* Index of refraction of air (approximately) */
                double nt = obj->ior;                      /* Index of refraction of the hit object*/
                double nnt;

                if (into)      /* Set ratio depending on hit from inside or outside */
                    nnt = nc / nt;
                else
                    nnt = nt / nc;

                double ddn = ray.dir.dot(nl);
                double cos2t = 1 - nnt * nnt * (1 - ddn * ddn);

                if (cos2t < 0) {
                    ray = reflRay;
                    flux = col.multiply(flux);
                    break;
                }

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
                /* Probability for selecting reflectance or transmittance */
                double P = .25 + .5 * Re;

                if (random->rand_double() < P)
                    ray = reflRay;
                else
                    ray = Ray(hitpoint- nl * 0.001, tdir);

                flux = col.multiply(flux);
                break;

            } // case REFR
            } // switch

        }
    }

inline double dot(const Vector& lhs, const Vector& rhs) {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}