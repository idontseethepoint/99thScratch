#include "SphereMesh.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace
{
	constexpr int N_RING = 20;
	constexpr int N_PER_RING = 20;
	constexpr float D_PHI = M_PI / N_RING;
	constexpr float D_THETA = 2 * M_PI / N_PER_RING;

	PosNormal toCartesian(float theta, float phi, float r)
	{
		vec3D normal = { std::sin(phi) * std::cos(theta),
		std::sin(phi) * std::sin(theta), std::cos(phi) };
		return { r * normal, normal };
	}
}

SphereMesh::SphereMesh(vec3D const& center, float r)
{
	//Top cap
	_data.push_back({{0, 0, r}, {0, 0, 1}});
	auto phi = D_PHI;
	int r1B = 1;
	for (int i = 0; i < N_PER_RING; ++i)
	{
		_data.push_back(toCartesian(i * D_THETA, phi, r));
	}
	for (unsigned int i = 0; i < N_PER_RING; ++i)
	{
		_indices.push_back({ r1B + i, r1B + (i + 1) % N_PER_RING, 0 });
	}

	//Middle
	for (int iRing = 0; iRing < N_RING - 1; ++iRing)
	{
		auto phi1 = iRing * D_PHI;
		auto phi2 = (iRing + 1) * D_PHI;
		int r2B = _data.size();
		auto theta_off = (iRing % 2) ? 0 * D_THETA / 2 : 0;
		for (int j = 0; j < N_PER_RING; ++j)
		{
			_data.push_back(toCartesian(theta_off + j * D_THETA, phi2, r));
		}
		for (unsigned int j = 0; j < N_PER_RING; ++j)
		{
			auto jn = (j + 1) % N_PER_RING;
			auto jp = (j + N_PER_RING - 1) % N_PER_RING;
			_indices.push_back({ r1B + j, r2B + j, r1B + jn });
			_indices.push_back({ r2B + jp, r2B + j, r1B + j });
		}
		r1B = r2B;
	}

	//Bottom cap
	unsigned int bCapIdx = _data.size();
	_data.push_back({ {0, 0, -r}, {0,0,-1} });
	for (unsigned int i = 0; i < N_PER_RING; ++i)
	{
		_indices.push_back({ r1B + i, bCapIdx, r1B + (i + 1) % N_PER_RING });
	}

	//Set center
	for (auto& pn : _data)
	{
		pn.pos += center;
	}
}

const GLfloat* SphereMesh::constPosNormalData() const
{
	return reinterpret_cast<const float*>(_data.data());
}

int SphereMesh::floatCount() const
{
	return _data.size() * 6;
}

int SphereMesh::vertCount() const
{
	return _data.size();
}

const unsigned int* SphereMesh::constIndexData() const
{
	return reinterpret_cast<const unsigned int*>(_indices.data());
}

int SphereMesh::posNormalByteCount() const
{
	return _data.size() * 6 * sizeof(GLfloat);
}

int SphereMesh::triCount() const
{
	return _indices.size();
}

int SphereMesh::indexCount() const
{
	return _indices.size() * 3;
}

int SphereMesh::indexByteCount() const
{
	return _indices.size() * 3 * sizeof(unsigned int);
}
