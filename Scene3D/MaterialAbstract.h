#pragma once

#include "Mesh.h"

#include <memory>

class MaterialAbstract
{
public:
	virtual void Render(std::shared_ptr<Mesh> mesh)=0;
};
