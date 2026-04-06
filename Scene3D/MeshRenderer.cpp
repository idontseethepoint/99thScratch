#include "MeshRenderer.h"
#include "ConeMesh.h"
#include "CuboidMesh.h"
#include "SceneWidget.h"
#include "SphereMesh.h"

#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGlExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

MeshRenderer::MeshRenderer(Mesh::Ptr mesh) : _VAO(new QOpenGLVertexArrayObject),
_VBO(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)), _EBO(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer)),
	_program(new QOpenGLShaderProgram), _mesh(mesh)
{
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/basic.vert");
	_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/basic.frag");
	_program->link();
	_program->bind();

	_projLoc = _program->uniformLocation("projection");
	_viewLoc = _program->uniformLocation("view");
	_modelLoc = _program->uniformLocation("model");

	_lightPosLoc = _program->uniformLocation("lightPos");
	_viewPosLoc = _program->uniformLocation("viewPos");
	_lightColorLoc = _program->uniformLocation("lightColor");
	_objectColorLoc = _program->uniformLocation("objectColor");


	_VAO->create();
	_VAO->bind();

	_VBO->create();
	_VBO->bind();
	_VBO->allocate(mesh->constPosNormalData(), mesh->floatCount() * sizeof(GLfloat));

	_EBO->create();
	_EBO->bind();
	_EBO->allocate(mesh->constIndexData(), mesh->indexByteCount());

	f->glEnableVertexAttribArray(0);
	f->glEnableVertexAttribArray(1);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), nullptr);
	f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));


	_program->release();
	_VAO->release();
	_VBO->release();
	_EBO->release();

}

MeshRenderer::Ptr MeshRenderer::Cuboid(vec3D const& low, vec3D const& high)
{
	auto cm = std::make_shared<CuboidMesh>(low, high);
	auto ret = std::make_shared<MeshRenderer>(cm);
	ret->SetColor({ 0, 255, 255 });
	return ret;
}

MeshRenderer::Ptr MeshRenderer::Sphere(vec3D const& center, float r)
{
	auto sm = std::make_shared<SphereMesh>(center, r);
	auto ret = std::make_shared<MeshRenderer>(sm);
	ret->SetColor({ 255, 255, 0 });
	return ret;
}

MeshRenderer::Ptr MeshRenderer::Cone(vec3D const& baseCenter, vec3D axis, float r)
{
	auto cm = std::make_shared<ConeMesh>(baseCenter, axis, r);
	auto ret = std::make_shared<MeshRenderer>(cm);
	ret->SetColor({ 0, 255, 0 });
	return ret;
}

MeshRenderer::~MeshRenderer()
{}

void MeshRenderer::Render(QMatrix4x4 const& trans)
{
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
	_program->bind();
	_VAO->bind();

	_program->setUniformValue(_projLoc, SceneWidget::GetProjMatrix());
	_program->setUniformValue(_viewLoc, SceneWidget::GetViewMatrix());
	_program->setUniformValue(_modelLoc, trans);

	_program->setUniformValue(_lightPosLoc, trans.inverted() * QVector3D(20, 20, 20));
	_program->setUniformValue(_viewPosLoc, SceneWidget::GetCamPos());
	_program->setUniformValue(_lightColorLoc, QColor(255, 255, 255));
	_program->setUniformValue(_objectColorLoc, _color);

	f->glDrawElements(GL_TRIANGLES, _mesh->indexCount(), GL_UNSIGNED_INT, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	_program->setUniformValue(_objectColorLoc, QColor(0, 0, 0));
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1, -1);
	f->glDrawElements(GL_TRIANGLES, _mesh->indexCount(), GL_UNSIGNED_INT, 0);
	glDisable(GL_POLYGON_OFFSET_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	_VAO->release();
	_program->release();
}

void MeshRenderer::SetColor(QColor const& color)
{
	_color = color;
}
