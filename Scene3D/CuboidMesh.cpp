#include "CuboidMesh.h"

CuboidMesh::CuboidMesh(vec3D const& low, vec3D const& high)
{
	vec3D v = high - low;
	for (int iFaceDim = 0; iFaceDim < 3; iFaceDim++)
	{
		for (int iFaceSide = 0; iFaceSide <= 1; ++iFaceSide)
		{
			int jDim = (iFaceDim + 1 + iFaceSide) % 3;
			int kDim = (iFaceDim + 2 - iFaceSide ) % 3;

			auto normal = vec3D::null();
			normal[iFaceDim] = 2.0f * iFaceSide - 1.0;
			
			vec3D c0 = low;
			c0[iFaceDim] += iFaceSide * v[iFaceDim];

			vec3D c1 = c0, c2 = c0;
			c1[jDim] += v[jDim];
			c2[kDim] += v[kDim];
			auto c3 = c1;
			c3[kDim] = c2[kDim];

			unsigned int idx0 = _data.size();
			_data.push_back({ c0, normal });
			_data.push_back({ c1, normal });
			_data.push_back({ c2, normal });
			_data.push_back({ c3, normal });

			//_data.push_back({ c0, normal });
			//_data.push_back({ c2, normal });
			//_data.push_back({ c1, normal });

			//_data.push_back({ c3, normal });
			//_data.push_back({ c1, normal });
			//_data.push_back({ c2, normal });

			_indices.push_back({ idx0, idx0 + 2, idx0 + 1 });
			_indices.push_back({ idx0 + 3, idx0 + 1, idx0 + 2 });
		}
	}
}

CuboidMesh::CuboidMesh() : CuboidMesh(vec3D::null(), vec3D(1, 1, 1))
{}

const GLfloat* CuboidMesh::constPosNormalData() const
{
	return reinterpret_cast<const float*>(_data.data());
}

int CuboidMesh::floatCount() const
{
	return 144; // 6 floats per vertex * 4 verts per face * 6 faces
}

int CuboidMesh::vertCount() const
{
	return 24; //4 verts per face * 6 faces
}

const unsigned int* CuboidMesh::constIndexData() const
{
	return reinterpret_cast<const unsigned int*>(_indices.data());
}

int CuboidMesh::posNormalByteCount() const
{
	return 144 * sizeof(float);
}
int CuboidMesh::triCount() const
{
	return 12;
}

int CuboidMesh::indexCount() const
{
	return 36;
}

int CuboidMesh::indexByteCount() const
{
	return 36 * sizeof(unsigned int); //3 indices per tri, 2 tris per side, 6 sides
}