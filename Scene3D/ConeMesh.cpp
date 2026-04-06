#include "ConeMesh.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace
{
	constexpr unsigned int N_RING = 20;
	constexpr float D_THETA = 2 * M_PI / N_RING;
}


ConeMesh::ConeMesh(vec3D const& baseCenter, vec3D const& axis, float r)
{
	auto d0 = axis[0] < axis[1] ? vec3D(1, 0, 0) : vec3D(00, 1, 0);
	auto d1 = axis ^ d0;
	auto M = d1 * d1;
	if (M == 0)
	{
		d1 = { 1, 0, 0 };
		M = 1;
	}
	else
	{
		M = std::sqrt(M);
	}
	d1 *= (r / M);
	auto d2 = axis ^ d1;
	M = d2 * d2;
	if (M == 0)
	{
		d2 = { 0, 1, 0 };
		M = 1.0;
	}
	else
	{
		M = std::sqrt(M);
	}
	d2 *= (r / M);
	auto n = -1.0f * axis;
	for (int i = 0; i < N_RING; ++i)
	{
		auto theta = i * D_THETA;
		_data.push_back(
			{ baseCenter + d1 * std::cos(theta) + d2 * std::sin(theta), n});
	}
	auto tip = baseCenter + axis;

	for (int i = 0; i < N_RING; ++i)
	{
		auto theta = i * D_THETA;
		_data.push_back(
			{ _data[i].pos, ((tip - _data[i].pos) ^ (_data[i].pos - baseCenter)).normalized()});
	}
	unsigned int baseCenterIdx = _data.size();
	_data.push_back({ baseCenter, n });

	for (unsigned int i = 0; i < N_RING; ++i)
	{
		_indices.push_back({ i, baseCenterIdx, (i + 1) % N_RING });
	}
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		unsigned int tipIdx = _data.size();
		auto i1 = N_RING + i;
		auto i2 = N_RING + (1 + i) % N_RING;
		_data.push_back({ tip, 0.5f * (_data[i1].normal + _data[i2].normal)});
		_indices.push_back({ tipIdx, i1, i2});
	}
}

const GLfloat* ConeMesh::constPosNormalData() const
{
	return reinterpret_cast<const float*>(_data.data());
}

int ConeMesh::floatCount() const
{
	return _data.size() * 6;
}

int ConeMesh::vertCount() const
{
	return _data.size();
}

const unsigned int* ConeMesh::constIndexData() const
{
	return reinterpret_cast<const unsigned int*>(_indices.data());
}

int ConeMesh::posNormalByteCount() const
{
	return _data.size() * 6 * sizeof(GLfloat);
}

int ConeMesh::triCount() const
{
	return _indices.size();
}

int ConeMesh::indexCount() const
{
	return _indices.size() * 3;
}

int ConeMesh::indexByteCount() const
{
	return _indices.size() * 3 * sizeof(unsigned int);
}
