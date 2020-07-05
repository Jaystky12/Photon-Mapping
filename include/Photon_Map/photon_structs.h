#ifndef PHOTONSTRUCTS_H
#define PHOTONSTRUCTS_H

#include "../Math/vector.h"


struct photon
{
	Vector position;
	Vector flux;
	Vector direction;
};

struct photon_query
{
	double     max_distance;
	size_t     count;
	Vector     position;
	Vector     normal;
};

struct photon_query_result
{
	const photon*   point;
	double          dist2;

	photon_query_result(const photon* p, double d2)
		: point(p)
		, dist2(d2)
	{ /* DO_NOTHING */
	}

	bool operator < (const photon_query_result& value) const
	{
		return dist2 < value.dist2;
	}
};
#endif