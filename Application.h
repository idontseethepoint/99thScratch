#pragma once

#include "LJ_Parameters.h"

#include <QObject>

class Application : public QObject
{
	Q_OBJECT
public:
	static Application* App();
signals:
	void StartSimulation(LJ_Parameters);
	void StopSimulation();
private:
	Application();
	static Application* _app;
};