#pragma once

#include "LJ_Simulation.h"

#include <QWidget>

#include <chrono>
#include <memory>

class LJ_Simulation;
class QLabel;

class SimulationDisplay : public QWidget
{
	Q_OBJECT
public:
	SimulationDisplay(std::shared_ptr<LJ_Simulation> sim, QWidget* parent);
private slots:
	void onStatsUpdate(LJ_Simulation::SimStats const& stats);
private:
	QLabel* _tLabel, *_keLabel, *_peLabel, *_EtotLabel, *_rLabel;
	double _prevSimT = 0.0;
	int _cnt = 1;
	std::chrono::system_clock::time_point _prevUpdateWall;
};
