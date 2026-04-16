#pragma once

#include "GeometryRendererAbstract.h"
#include "Mesh.h"
#include "vec3D.h"

#include <QColor>

#include <memory>

class QOpenGLBuffer;
class QOpenGLShaderProgram;
class QOpenGLVertexArrayObject;

class MeshRenderer : public GeometryRendererAbstract
{
public:
	using Ptr = std::shared_ptr<MeshRenderer>;
	MeshRenderer(Mesh::Ptr mesh);
	static MeshRenderer::Ptr Arrow(vec3D const& start, vec3D const& end, float r);
	static MeshRenderer::Ptr Cuboid(vec3D const& low, vec3D const& high);
	static MeshRenderer::Ptr Sphere(vec3D const& center, float r);
	static MeshRenderer::Ptr Cylinder(vec3D const& base1Center, vec3D const& axis, float r);
	static MeshRenderer::Ptr Cone(vec3D const& baseCenter, vec3D axis, float r);
	~MeshRenderer();
	virtual void Render(QMatrix4x4 const& trans) override;
	virtual void SetColor(QColor const& color) override;
private:
	Mesh::Ptr _mesh;
	QColor _color;
	std::unique_ptr<QOpenGLVertexArrayObject> _VAO;
	std::unique_ptr<QOpenGLBuffer> _VBO;
	std::unique_ptr<QOpenGLBuffer> _EBO;
	std::unique_ptr<QOpenGLShaderProgram> _program;
	int _projLoc;
	int _viewLoc;
	int _modelLoc;
	int _lightPosLoc;
	int _viewPosLoc;
	int _lightColorLoc;
	int _objectColorLoc;
};