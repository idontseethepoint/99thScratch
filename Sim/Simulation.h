#pragma once

#include "SceneNode.h"

#include <QObject>

class Simulation : public QObject
{
public:
	virtual void UpdateNode(SceneNode::Ptr node)=0;
};
