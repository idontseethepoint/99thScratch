#pragma once

#include "Mesh.h"
#include "PosNormal.h"
#include "vec3D.h"

class ConeMesh : public Mesh
{
public:
	ConeMesh(vec3D const& baseCenter, vec3D const& axis, float r);
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