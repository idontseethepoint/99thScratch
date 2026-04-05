#include "AABB.h"

double AABB::Volume() const
{
	auto d = High - Low;
	return d[0] * d[1] * d[2];
}
