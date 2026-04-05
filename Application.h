#pragma once

#include "LJ_Simulation.h"

#include <QObject>

class Application : public QObject
{
	Q_OBJECT
public:
	static Application* App();
signals:
	void StartSimulation(LJ_Simulation::Parameters);
	void StopSimulation();
private:
	Application();
	static Application* _app;
};