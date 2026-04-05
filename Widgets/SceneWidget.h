#pragma once
#include "SceneNode.h"

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QVector3D>

#include <memory>

class Mesh;
class LJ_Simulation;

class QOpenGLShaderProgram;
class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

struct cudaGraphicsResource;

class SceneWidget : public QOpenGLWidget
{
	Q_OBJECT
public:
	SceneWidget(QWidget* parent);

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;
    inline static QMatrix4x4 const& GetViewMatrix() { return _view; }
    inline static QMatrix4x4 const& GetProjMatrix() { return _proj; }
    inline static QVector3D const& GetCamPos() { return _eye; }
    inline static QVector3D const& GetUp() { return _up; }

signals:
    void viewUpdate();
protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
private slots:
    void onTimeout();
private:
    int m_projMatrixLoc = 0;
    int m_camMatrixLoc = 0;
    int m_worldMatrixLoc = 0;
    int m_myMatrixLoc = 0;
    int m_lightPosLoc = 0;
    static QMatrix4x4 _view;
    static QMatrix4x4 _proj;
    QMatrix4x4 m_world;
    static QVector3D _eye;
    static QVector3D _up;
    QVector3D m_target = { 0, 0, 0 };
    float _t = 0;
    cudaGraphicsResource* _cuda_VBO = nullptr;
    QPoint _lastPos;
    float _distToTarget = 10.0f;
};