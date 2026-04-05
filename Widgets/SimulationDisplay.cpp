#include "SimulationDisplay.h"

#include <QLabel>
#include <QLayout>

SimulationDisplay::SimulationDisplay(std::shared_ptr<LJ_Simulation> sim, QWidget* parent)
{
	auto layout = new QHBoxLayout(this);
	_tLabel = new QLabel("t", this);
	_keLabel = new QLabel("KE", this);
	_peLabel = new QLabel("PE", this);
	_EtotLabel = new QLabel("Etot", this);
	layout->addWidget(_tLabel);
	layout->addWidget(_keLabel);
	layout->addWidget(_peLabel);
	layout->addWidget(_EtotLabel);
	QObject::connect(sim.get(), &LJ_Simulation::SimStatsUpdate,
		this, &SimulationDisplay::onStatsUpdate);
}

void SimulationDisplay::onStatsUpdate(LJ_Simulation::SimStats const& stats)
{
	_tLabel->setText(QString("t: ") + QString::number(stats.t));
	_keLabel->setText(QString("KE: ") + QString::number(stats.KE));
	_peLabel->setText(QString("PE: ") + QString::number(stats.PE));
	_EtotLabel->setText(QString("Etot: ") + QString::number(stats.KE + stats.PE));
}