#include "VectorFieldRenderer.h"
#include "ArrowMesh.h"
#include "SceneWidget.h"

#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGlExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

VectorFieldRenderer::VectorFieldRenderer(VectorField::Ptr field) : _VAO(new QOpenGLVertexArrayObject),
_field(field), _VBO(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)), 
_EBO(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer)),
_instanceVBO(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer)),
_program(new QOpenGLShaderProgram), _color(255, 0, 0),
_arrow(new ArrowMesh(vec3D::null(), { 1.0f, 0.f, 0.f }, 0.025))
{
	_n_instance = field->NXi[0] * field->NXi[1] * field->NXi[2];
	auto f = QOpenGLContext::currentContext()->extraFunctions();

	_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vector_field.vert");
	_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/vector_field.frag");
	_program->link();
	_program->bind();

	_projLoc = _program->uniformLocation("projection");
	_viewLoc = _program->uniformLocation("view");
	_modelLoc = _program->uniformLocation("model");
	_dxiLoc = _program->uniformLocation("dxi");
	_nxiLoc = _program->uniformLocation("nxi");

	_lightPosLoc = _program->uniformLocation("lightPos");
	_viewPosLoc = _program->uniformLocation("viewPos");
	_lightColorLoc = _program->uniformLocation("lightColor");
	_objectColorLoc = _program->uniformLocation("objectColor");


	_VAO->create();
	_VAO->bind();

	_VBO->create();
	_VBO->bind();
	_VBO->allocate(_arrow->constPosNormalData(), _arrow->floatCount() * sizeof(GLfloat));

	_EBO->create();
	_EBO->bind();
	_EBO->allocate(_arrow->constIndexData(), _arrow->indexByteCount());

	f->glEnableVertexAttribArray(0);
	f->glEnableVertexAttribArray(1);
	f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), nullptr);
	f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));


	_instanceVBO->create();
	_instanceVBO->bind();
	_instanceVBO->allocate(field->Vectors.data(), field->Vectors.size() * sizeof(vec3D));

	f->glEnableVertexAttribArray(2);
	f->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(GLfloat), reinterpret_cast<void*>(0));
	f->glVertexAttribDivisor(2, 1);

	_program->release();
	_VAO->release();
	_VBO->release();
	_EBO->release();
}

VectorFieldRenderer::~VectorFieldRenderer() {}

void VectorFieldRenderer::Render(QMatrix4x4 const& trans)
{
	auto f = QOpenGLContext::currentContext()->extraFunctions();
	_program->bind();
	_VAO->bind();
	_instanceVBO->bind();
	_instanceVBO->write(0, _field->Vectors.data(),
		_field->Vectors.size() * sizeof(vec3D));

	_program->setUniformValue(_projLoc, SceneWidget::GetProjMatrix());
	_program->setUniformValue(_viewLoc, SceneWidget::GetViewMatrix());
	_program->setUniformValue(_modelLoc, trans);
	_program->setUniformValue(_dxiLoc, _field->DXi.Qvec());
	f->glUniform3i(_nxiLoc, _field->NXi[0], _field->NXi[1], _field->NXi[2]);

	_program->setUniformValue(_lightPosLoc, trans.inverted() * QVector3D(20, 20, 20));
	_program->setUniformValue(_viewPosLoc, SceneWidget::GetCamPos());
	_program->setUniformValue(_lightColorLoc, QColor(255, 255, 255));
	_program->setUniformValue(_objectColorLoc, _color);

	f->glDrawElementsInstanced(GL_TRIANGLES, _arrow->indexCount(), GL_UNSIGNED_INT, 0, _n_instance);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//_program->setUniformValue(_objectColorLoc, QColor(255, 255, 255));
	//glEnable(GL_POLYGON_OFFSET_LINE);
	//glPolygonOffset(-1, -1);
	//f->glDrawElements(GL_TRIANGLES, _mesh->indexCount(), GL_UNSIGNED_INT, 0);
	//glDisable(GL_POLYGON_OFFSET_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//_VAO->release();

	_program->release();
}

void VectorFieldRenderer::SetColor(QColor const& color)
{
	_color = color;
}
