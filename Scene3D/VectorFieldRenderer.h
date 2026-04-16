#pragma once

#include "GeometryRendererAbstract.h"
#include "VectorField.h"

#include <array>
#include <vector>

class ArrowMesh;
class QOpenGLBuffer;
class QOpenGLShaderProgram;
class QOpenGLVertexArrayObject;

class VectorFieldRenderer : public GeometryRendererAbstract
{
public:
	VectorFieldRenderer(VectorField::Ptr field);
	~VectorFieldRenderer();
	virtual void Render(QMatrix4x4 const& trans) override;
	virtual void SetColor(QColor const& color) override;
private:
	QColor _color;
	std::unique_ptr<ArrowMesh> _arrow;
	std::unique_ptr<QOpenGLVertexArrayObject> _VAO;
	std::unique_ptr<QOpenGLBuffer> _VBO;
	std::unique_ptr<QOpenGLBuffer> _instanceVBO;
	std::unique_ptr<QOpenGLBuffer> _EBO;
	std::unique_ptr<QOpenGLShaderProgram> _program;
	int _n_instance;
	int _projLoc;
	int _viewLoc;
	int _modelLoc;
	int _lightPosLoc;
	int _viewPosLoc;
	int _lightColorLoc;
	int _objectColorLoc;
	int _dxiLoc;
	int _nxiLoc;
	VectorField::Ptr _field;
};