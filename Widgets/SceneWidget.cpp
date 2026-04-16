#include "SceneWidget.h"
#include "CuboidMesh.h"
#include "LJ_Simulation.h"
#include "MeshRenderer.h"

#include "mykernels.cuh"

#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>

#include "cuda_gl_interop.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <iostream>

#include "VectorFieldRenderer.h"

QMatrix4x4 SceneWidget::_view;
QMatrix4x4 SceneWidget::_proj;
QVector3D SceneWidget::_eye = QVector3D(10.0, 0, 0);
QVector3D SceneWidget::_up = QVector3D(0, 1.0, 0);

SceneWidget::SceneWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    m_world.setToIdentity();
    m_world.translate(0, 0, -1);
    m_world.rotate(180, 1, 0, 0);

    auto timer = new QTimer(this);
    timer->setInterval(16);
    timer->setSingleShot(false);
    QObject::connect(timer, &QTimer::timeout, this,
        &SceneWidget::onTimeout);
    timer->start();
}

void SceneWidget::initializeGL()
{
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    qDebug() << "GL Vendor:" << reinterpret_cast<const char*>(f->glGetString(GL_VENDOR));
    qDebug() << "GL Renderer:" << reinterpret_cast<const char*>(f->glGetString(GL_RENDERER));
    unsigned int cudaDeviceCount = 0;
    int cudaDevices[1];
    cudaGLGetDevices(&cudaDeviceCount, cudaDevices, 1, cudaGLDeviceListAll);
    if (cudaDeviceCount > 0) {
        cudaSetDevice(cudaDevices[0]);
    }
    else {
        qDebug() << "No CUDA device found for GL interop!";
    }
    f->glEnable(GL_DEPTH_TEST);
    f->glEnable(GL_CULL_FACE);

    SceneNode::Root()->addAxes();
}

void SceneWidget::resizeGL(int w, int h)
{
    _proj.setToIdentity();
    _proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

namespace
{
    void print(std::string const& name, QMatrix4x4 const& m)
    {
        std::cout << name << ":\n";
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                std::cout << "\t" << m(i, j) << ", ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
}

void SceneWidget::paintGL()
{
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    f->glClearColor(0, 0, 0, 1);
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SceneNode::Root()->onFrame();
}

void SceneWidget::mousePressEvent(QMouseEvent* e)
{
    _lastPos = e->pos();
}

void SceneWidget::mouseMoveEvent(QMouseEvent* e)
{
    auto delta = e->pos() - _lastPos;

    auto ax = delta.x() * 0.001;
    auto ay = delta.y() * 0.001;
    auto v = _eye - m_target;
    auto t = QVector3D::crossProduct(v, _up);
    if (t.isNull())
        return;

    if (e->buttons() & Qt::RightButton)
    {
        t.normalize();
        v.normalize();
        auto d = (t * ax + _up * ay) * _distToTarget;
        _eye += d;
        m_target += d;
    }
    else
    {
        t *= v.length() / t.length();
        _eye = m_target + v * std::cos(ax) + t * std::sin(ax);

        v = _eye - m_target;
        auto uv = _up * _distToTarget;
        t = QVector3D::crossProduct(v, _up);
        _eye = m_target + v * std::cos(ay) + uv * std::sin(ay);
        v = _eye - m_target;
		_up = QVector3D::crossProduct(t, v);
		_up.normalize();
	}
	_lastPos = e->pos();
}

void SceneWidget::mouseReleaseEvent(QMouseEvent* e)
{
}

void SceneWidget::wheelEvent(QWheelEvent* e)
{
    auto v = _eye - m_target;
    if (v.isNull())
        return;
    v.normalize();
    auto dy = e->angleDelta().y();
    _distToTarget *= std::exp(-static_cast<float>(dy) / 1'000);
    v *= _distToTarget;
    _eye = m_target + v;
}

void SceneWidget::onTimeout()
{
    _t += 0.2 * 0.16;
    _view = QMatrix4x4();
    _view.lookAt(_eye, m_target, _up);
    emit viewUpdate();
    update();
}
