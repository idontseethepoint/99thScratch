#pragma once

#include "SceneNode.h"

class SimulationSceneNode : public SceneNode
{
public:
	virtual void onFrame() override;
	SimulationSceneNode(std::shared_ptr<Simulation> sim,
		QString const& name, SceneNode::Ptr parent);
private:
	std::shared_ptr<Simulation> _sim;
};