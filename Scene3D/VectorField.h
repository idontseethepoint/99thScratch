#pragma once

#include "vec3D.h"

#include <array>
#include <memory>
#include <vector>

struct VectorField
{
	using Ptr = std::shared_ptr<VectorField>;
	VectorField();
	VectorField(unsigned int nx, unsigned int ny, unsigned int nz, float dx, float dy, float dz);
	VectorField(unsigned int nxi, float dxi);
	std::array<unsigned int, 3> NXi;
	vec3D DXi;
	std::vector<vec3D> Vectors;
	vec3D& operator()(int i, int j, int k);
	vec3D const& operator()(int i, int j, int k) const;
};
