#ifndef NEARESTPHOTON_H
#define NEARESTPHOTON_H

#include "photon_map.h"
#include "../Math/vector.h"
#include "photon_structs.h"
#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>

class nearest_photon
{
public:
	nearest_photon(size_t size);

	~nearest_photon();

	void push(const photon_query_result& value);

	bool empty() const;

	size_t size() const;

	const photon_query_result& top() const;

	void pop();

	const photon_query_result& operator[](int index);

private:
	std::vector<photon_query_result>    photons;
	std::less<photon_query_result>      less;
};
#endif