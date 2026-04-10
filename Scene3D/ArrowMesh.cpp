#include "ArrowMesh.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace
{
	constexpr unsigned int N_RING = 20;
	constexpr float D_THETA = 2 * M_PI / N_RING;
	constexpr float TAIL_FRAC = 0.9;
	constexpr float HEAD_FRAC = 1.0f - TAIL_FRAC;
}

ArrowMesh::ArrowMesh(vec3D const& start, vec3D const& end, float r)
{
	auto axis = TAIL_FRAC * (end - start);
	auto minusAxis = -axis;
	_data.push_back({ start, minusAxis });
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
		_data.push_back({ {start + d0 * std::cos(theta) + d1 * std::sin(theta)},
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
		_data.push_back({ {start + d0 * std::cos(theta) + d1 * std::sin(theta)},
			n0 * std::cos(theta) + n1 * std::sin(theta) });
	}

	auto topCenter = start + axis;
	unsigned int top_start = _data.size();
	//Top vertices with side-facing normals
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		auto theta = D_THETA * i;
		_data.push_back({ {topCenter + d0 * std::cos(theta) + d1 * std::sin(theta)},
			n0 * std::cos(theta) + n1 * std::sin(theta) });
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
	//Top vertices with down-facing normals
	for (unsigned int i = 0; i < N_RING; ++i)
	{
		auto theta = D_THETA * i;
		_data.push_back({ {topCenter + d0 * std::cos(theta) + d1 * std::sin(theta)},
			minusAxis });
	}

	{
		r = 3 * r;
		auto baseCenter = start + axis;
		axis = HEAD_FRAC * (end - start);
		unsigned int coneStartIdx = _data.size();

		auto d1 = r * n0;
		auto d2 = r * n1;

		auto n = -1.0f * axis;
		for (int i = 0; i < N_RING; ++i)
		{
			auto theta = i * D_THETA;
			_data.push_back(
				{ baseCenter + d1 * std::cos(theta) + d2 * std::sin(theta), n });
		}
		auto tip = baseCenter + axis;

		for (int i = 0; i < N_RING; ++i)
		{
			auto theta = i * D_THETA;
			_data.push_back(
				{ _data[i + coneStartIdx].pos, ((tip - _data[i + coneStartIdx].pos) ^ (_data[i + coneStartIdx].pos - baseCenter)).normalized() });
		}
		unsigned int baseCenterIdx = _data.size();
		_data.push_back({ baseCenter, n });

		for (unsigned int i = 0; i < N_RING; ++i)
		{
			auto cyl0 = top_start + i, cyl1 = top_start + (i + 1) % N_RING;
			auto cone0 = coneStartIdx + i, cone1 = coneStartIdx + (i + 1) % N_RING;
			_indices.push_back({ cone0, cyl0, cyl1 });
			_indices.push_back({ cone1, cone0, cyl1});
			//_indices.push_back(
			//	{ i + coneStartIdx, baseCenterIdx, coneStartIdx + (i + 1) % N_RING });
		}
		for (unsigned int i = 0; i < N_RING; ++i)
		{
			unsigned int tipIdx = _data.size();
			auto i1 = coneStartIdx + N_RING + i;
			auto i2 = coneStartIdx + N_RING + (1 + i) % N_RING;
			_data.push_back({ tip, 0.5f * (_data[i1].normal + _data[i2].normal) });
			_indices.push_back({ tipIdx, i1, i2 });
		}
	}
}

const GLfloat* ArrowMesh::constPosNormalData() const
{
	return reinterpret_cast<const float*>(_data.data());
}

int ArrowMesh::floatCount() const
{
	return _data.size() * 6;
}

int ArrowMesh::vertCount() const
{
	return _data.size();
}

const unsigned int* ArrowMesh::constIndexData() const
{
	return reinterpret_cast<const unsigned int*>(_indices.data());
}

int ArrowMesh::posNormalByteCount() const
{
	return _data.size() * 6 * sizeof(GLfloat);
}

int ArrowMesh::triCount() const
{
	return _indices.size();
}

int ArrowMesh::indexCount() const
{
	return _indices.size() * 3;
}

int ArrowMesh::indexByteCount() const
{
	return _indices.size() * 3 * sizeof(unsigned int);
}
