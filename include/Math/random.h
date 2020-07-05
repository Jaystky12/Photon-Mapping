#ifndef RANDOM_H
#define RANDOM_H
#include "stdint.h"
/* Implementation details for fast pseudo random generator used in project:
 * https://codingforspeed.com/using-faster-psudo-random-generator-xorshift/	
 */
class Random
{
public:
	Random(uint32_t seed)
	{
		a = 123456789;
		b = 362436069;
		c = 521288629;
		d = (seed <= 0) ? 88675123 : seed;
	}

	inline uint32_t rand_int()
	{
		uint32_t t = a ^ (a << 11);
		a = b;
		b = c;
		c = d;
		d = (d ^ (d >> 19)) ^ (t ^ (t >> 8));
		return d;
	}

	inline double rand_double()
	{
		return static_cast<double>(rand_int()) / UINT32_MAX;
	}

private:
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;
};
#endif /* !GRANDPARENT_H */