#pragma once

#include "Mesh.h"
#include "PosNormal.h"

#include <array>
#include <vector>

class CuboidMesh : public Mesh
{
public:
	CuboidMesh(vec3D const& low, vec3D const& high);
	CuboidMesh();
	virtual const GLfloat* constPosNormalData() const override;
	virtual int floatCount() const override;
	virtual int vertCount() const override;
	virtual const unsigned int* constIndexData() const override;
	virtual int posNormalByteCount() const override;
	virtual int triCount() const override;
	virtual int indexCount() const override;
	virtual int indexByteCount() const override;
private:
	std::vector<PosNormal> _data;
	std::vector<std::array<unsigned int, 3>> _indices;
};