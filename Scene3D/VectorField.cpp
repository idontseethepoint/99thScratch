#include "VectorField.h"

#include <random>


std::mt19937 rng(std::random_device{}());


VectorField::VectorField() : NXi{ 0 }, DXi(vec3D::null())
{
	std::uniform_real_distribution<float> dist(-1, 1);
	NXi = { 20, 20, 20 };
	DXi = { 10.0f, 10.0f, 10.0f };
	vec3D v(1, 0, 0);
	for (int i = 0; i < NXi[0]; ++i)
	{
		for (int j = 0; j < NXi[1]; ++j)
		{
			for (int k = 0; k < NXi[2]; ++k)
			{
				Vectors.push_back(v);
				v += 0.25f * vec3D(dist(rng), dist(rng), dist(rng));
				v.normalize();
			}
		}
	}
}

VectorField::VectorField(unsigned int nx, unsigned int ny, unsigned int nz, float dx, float dy, float dz) :
	NXi{ nx, ny, nz }, DXi(dx, dy, dz), Vectors(nx * ny * nz, vec3D::null())
{}

VectorField::VectorField(unsigned int nxi, float dxi) : VectorField(nxi, nxi, nxi, dxi, dxi, dxi)
{}

vec3D& VectorField::operator()(int i, int j, int k)
{
	return Vectors[i + NXi[0] * j + NXi[0] * NXi[1] * k];
}

vec3D const& VectorField::operator()(int i, int j, int k) const
{
	return Vectors[i + NXi[0] * j + NXi[0] * NXi[1] * k];
}
