#pragma once

#include "LJ_Simulation.h"

#include <QWidget>

class LJ_Simulation;
class QLabel;

#include <memory>

class SimulationDisplay : public QWidget
{
	Q_OBJECT
public:
	SimulationDisplay(std::shared_ptr<LJ_Simulation> sim, QWidget* parent);
private slots:
	void onStatsUpdate(LJ_Simulation::SimStats const& stats);
private:
	QLabel* _tLabel, *_keLabel, *_peLabel, *_EtotLabel;
};
