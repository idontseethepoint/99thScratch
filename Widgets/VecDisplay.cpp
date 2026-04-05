#include "VecDisplay.h"

#include <QLabel>
#include <QLayout>

VecDisplay::VecDisplay(QString const& name, QVector3D const& v,
	QWidget* parent)
{
	auto layout = new QHBoxLayout(this);
	setLayout(layout);
	_nLabel = new QLabel(name + ":", this);
	layout->addWidget(_nLabel);
	_vLabel = new QLabel("", this);
	layout->addWidget(_vLabel);
	updateVector(v);
}

void VecDisplay::updateVector(QVector3D const& v)
{
	_vLabel->setText(QString("(") +
		QString::number(v.x()) + ", " + 
		QString::number(v.y()) + ", " + 
		QString::number(v.z()) + ")"
	);
}

