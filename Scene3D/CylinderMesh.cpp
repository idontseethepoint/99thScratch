#include "CylinderMesh.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace
{
	constexpr unsigned int N_RING = 20;
	constexpr float D_THETA = 2 * M_PI / N_RING;
}

CylinderMesh::CylinderMesh(vec3D const& base1Center, vec3D const& axis, float r)
{
	auto minusAxis = -axis;
	_data.push_back({ base1Center, minusAxis });
	auto n0 = axis.GetANormal();
	auto n1 = axis ^ n0;
	n1.normalize();
	auto d0 = r * n0;
	auto d1 = r * n1;


	unsigned int bottomStart = _data.size();
	//Bottom vertices with down-facing normals
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		auto theta = D_THETA * i;
		_data.push_back({ {base1Center + d0 * std::cos(theta) + d1 * std::sin(theta)},
			minusAxis });
	}

	//Bottom triangles
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		_indices.push_back({ bottomStart + i, 0, bottomStart + (i + 1) % N_RING });
	}

	bottomStart = _data.size();
	//Bottom vertices with side-facing normals
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		auto theta = D_THETA * i;
		_data.push_back({ {base1Center + d0 * std::cos(theta) + d1 * std::sin(theta)},
			n0 * std::cos(theta) + n1 * std::sin(theta) });
	}

	auto topCenter = base1Center + axis;
	unsigned int top_start = _data.size();
	//Top vertices with side-facing normals
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		auto theta = D_THETA * i;
		_data.push_back({ {topCenter + d0 * std::cos(theta) + d1 * std::sin(theta)},
			n0* std::cos(theta) + n1 * std::sin(theta) });
	}

	//Middle Triangles
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		unsigned int cb = bottomStart + i, nb = bottomStart + (1 + i) % N_RING;
		unsigned int ct = top_start + i, nt = top_start + (1 + i) % N_RING;
		_indices.push_back({ cb, nb, ct });
		_indices.push_back({ ct, nb, nt });
	}

	top_start = _data.size();
	//Top vertices with up-facing normals
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		auto theta = D_THETA * i;
		_data.push_back({ {topCenter + d0 * std::cos(theta) + d1 * std::sin(theta)},
			axis });
	}

	unsigned int topCenterIdx = _data.size();
	_data.push_back({ topCenter, axis });

	//Top Triangles
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		_indices.push_back({ topCenterIdx, top_start + i, top_start + (i + 1) % N_RING });
	}
}

const GLfloat* CylinderMesh::constPosNormalData() const
{
	return reinterpret_cast<const float*>(_data.data());
}

int CylinderMesh::floatCount() const
{
	return _data.size() * 6;
}

int CylinderMesh::vertCount() const
{
	return _data.size();
}

const unsigned int* CylinderMesh::constIndexData() const
{
	return reinterpret_cast<const unsigned int*>(_indices.data());
}

int CylinderMesh::posNormalByteCount() const
{
	return _data.size() * 6 * sizeof(GLfloat);
}

int CylinderMesh::triCount() const
{
	return _indices.size();
}

int CylinderMesh::indexCount() const
{
	return _indices.size() * 3;
}

int CylinderMesh::indexByteCount() const
{
	return _indices.size() * 3 * sizeof(unsigned int);
}
