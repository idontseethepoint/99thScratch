#pragma once

#include "LJ_Simulation.h"

#include <QWidget>

#include <memory>
#include <vector>

class LJ_AtomsOptions;

class QDoubleSpinBox;
class QToolButton;
class QVBoxLayout;

class SimulationControls : public QWidget
{
	Q_OBJECT
public:
	SimulationControls(std::shared_ptr<LJ_Simulation> sim, 
		QWidget* parent);
	LJ_Simulation::Parameters GetParameters();
private slots:
	void onCountChange(int newCount);
	void onStartPausePress();
private:
	bool _playing = false;
	std::vector<LJ_AtomsOptions*> _atomControls;
	QDoubleSpinBox* _dtSpin;
	QDoubleSpinBox* _lxSpinner, * _lySpinner, * _lzSpinner;
	QDoubleSpinBox* _hxSpinner, * _hySpinner, * _hzSpinner;
	QToolButton* _startPauseButton;
	QVBoxLayout* _atomsLayout;
	std::shared_ptr<LJ_Simulation> _sim;
};
