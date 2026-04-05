#pragma once

#include "vec3D.h"

//Axis-Aligned Bounding Box
struct AABB 
{
	vec3Dd Low;
	vec3Dd High;
	double Volume() const;
};
