#pragma once

#include <qopengl.h>

#include <memory>

class Mesh
{
public:
	using Ptr = std::shared_ptr<Mesh>;
	virtual const GLfloat* constPosNormalData() const = 0;
	virtual int floatCount() const = 0;
	virtual int vertCount() const = 0;
	virtual const unsigned int* constIndexData() const = 0;
	virtual int posNormalByteCount() const = 0;
	virtual int triCount() const = 0;
	virtual int indexCount() const = 0;
	virtual int indexByteCount() const = 0;
};