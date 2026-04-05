#pragma once

#include "LJ_Simulation.h"

#include <QWidget>

class QDoubleSpinBox;
class QSpinBox;

class LJ_AtomsOptions : public QWidget
{
	Q_OBJECT
public:
	LJ_AtomsOptions(int typeId, QWidget* parent);
	LJ_Simulation::SpeciesParams GetAtomParams() const;
private:
	int _tId;
	QDoubleSpinBox* _epsSpin;
	QDoubleSpinBox* _sigSpin;
	QDoubleSpinBox* _massSpin;
	QSpinBox* _nSpin;
};
