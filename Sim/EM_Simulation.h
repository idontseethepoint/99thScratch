#pragma once

#include "Simulation.h"
#include "VectorField.h"

#include <mutex>
#include <thread>

class EM_Simulation : public Simulation
{
	Q_OBJECT
public:
	EM_Simulation();
	virtual void UpdateNode(SceneNode::Ptr node);
private slots:
	void onStartSignal();
	void onPauseSignal();
private:
	void SimulationLoop();
	void step();
	VectorField _EField;
	VectorField _BField;
	VectorField::Ptr _Erender;
	VectorField::Ptr _Brender;
	bool _nodeInitialized = false;
	std::mutex _renderMutex;
	std::thread _simThread;
};
