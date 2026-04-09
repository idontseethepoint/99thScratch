#pragma once

#include "vec3D.h"

//Axis-Aligned Bounding Box
struct AABB 
{
	vec3Dd Low;
	vec3Dd High;
	double Volume() const;
	inline bool operator==(AABB const& other) const { return Low == other.Low && High == other.High; }
	inline bool operator!=(AABB const& other) const { return !(*this == other); }
};
