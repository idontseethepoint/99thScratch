#pragma once

#include "AABB.h"

#include <vector>

struct LJ_SpeciesParams
{
	double Eps;
	double Sigma;
	double Mass;
	int N;
};

struct LJ_Parameters
{
	double TimeStep;
	double OffDiagFactor;
	AABB Box;
	std::vector<LJ_SpeciesParams> Atoms;
	bool UseCUDA;
};