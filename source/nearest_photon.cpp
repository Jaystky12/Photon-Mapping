#include "../include/Photon_Map/photon_map.h"
#include "../include/Photon_Map/nearest_photon.h"

using namespace std;

nearest_photon :: nearest_photon(size_t size)
		: photons()
		, less()
	{
		photons.reserve(size + 1);
	}

nearest_photon :: ~nearest_photon()
	{
		photons.clear();
		photons.shrink_to_fit();
	}

void nearest_photon :: push(const photon_query_result& value)
	{
		photons.emplace_back(value);
		std::push_heap(photons.begin(), photons.end(), less);
	}

bool nearest_photon :: empty() const
	{
		return photons.empty();
	}

size_t nearest_photon :: size() const
	{
		return photons.size();
	}

const photon_query_result& nearest_photon :: top() const
	{
		return photons.front();
	}

void nearest_photon :: pop()
	{
		std::pop_heap(photons.begin(), photons.end(), less);
		photons.pop_back();
	}

const photon_query_result& nearest_photon::operator[](int index)
	{
		return photons[index];
	}