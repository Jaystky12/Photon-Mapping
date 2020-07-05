#ifndef PHOTONMAP_H
#define PHOTONMAP_H

#include "../Math/vector.h"
#include "nearest_photon.h"
#include "photon_structs.h"
#include "../Scene/scene.h"
#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>

inline double dot(const Vector& lhs, const Vector& rhs);

class photon_map
{
public:
	photon_map();

	~photon_map();

	size_t size() const;

	void search_for_photons(photon_query& query, nearest_photon& result);

	void store(const photon& value);

	void build_photon_map();

	void clear_photon_map();

    void generate_map(scene* scene, photon_map* photon_m, int photons, Random* random, bool caustics) ;

    void trace_photons(scene* scene, const Ray& emit_ray, const Vector& emit_flux, Random* random, photon_map* photon_map, bool caustics_bool);

private:
	struct node
	{
		photon* point;
		node*   left;
		node*   right;
		int     axis;
	};

	struct less_comp
	{
		less_comp(int axis)
			: axis_(axis)
		{ /* DO_NOTHING */
		}

		bool operator() (const photon& lhs, const photon& rhs)
		{
			return lhs.position.a[axis_] < rhs.position.a[axis_];
		}

		int axis_;
	};

	node*                   root;
	std::vector<photon>     points;

	void recursive_search(photon_query& query, node* node, nearest_photon& result);

	node* recursive_build(photon* photons, int count, int depth);

    void recursive_clear(node* node);
};
#endif