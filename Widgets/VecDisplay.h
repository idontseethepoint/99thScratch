#pragma once

#include <QWidget>

#include <QVector3D>

class QLabel;

class VecDisplay : public QWidget
{
	Q_OBJECT
public:
	VecDisplay(QString const& name, QVector3D const& v, QWidget* parent);
public slots:
	void updateVector(QVector3D const& v);
private:
	QLabel* _nLabel;
	QLabel* _vLabel;
};