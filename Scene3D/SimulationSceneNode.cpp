#include "SimulationSceneNode.h"
#include "Simulation.h"

void SimulationSceneNode::onFrame()
{
	_sim->UpdateNode(shared_from_this());
	for (auto c : _children)
	{
		c->onFrame();
	}
}

SimulationSceneNode::SimulationSceneNode(std::shared_ptr<Simulation> sim, 
	QString const& name, SceneNode::Ptr parent) : SceneNode(name, parent),
	_sim(sim)
{}
