#include "EM_Simulation.h"
#include "SceneNode.h"
#include "VectorFieldRenderer.h"

#include <random>

namespace
{
	constexpr unsigned int NX = 20;
	constexpr float DX = 0.5;
	constexpr float dt = 0.01;
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
}

EM_Simulation::EM_Simulation() : _EField(NX, DX), _BField(NX, DX)
{
	for (int i = 0; i < NX; ++i)
	{
		float x = i * DX;
		for (int j = 0; j < NX; ++j)
		{
			float y = j * DX;
			for (int k = 0; k < NX; ++k)
			{
				float z = k * DX;
				vec3D dx{ x - 4.5f, y - 4.5f, z - 4.5f };
				auto r2Inv = 1.0 / (dx * dx);
				_EField(i, j, k) = dx * r2Inv;
			}
		}
	}
	//for (int i = 3; i < NX-3; ++i)
	//{
	//	_EField(i, 10, 10)[2] = 1;
	//	_BField(i, 10, 10)[1] = -1;
	//	_EField(i, 11, 10)[2] = 1;
	//	_BField(i, 11, 10)[1] = -1;
	//	_EField(i, 10, 11)[2] = 1;
	//	_BField(i, 10, 11)[1] = -1;
	//	_EField(i, 11, 11)[2] = 1;
	//	_BField(i, 11, 11)[1] = -1;
	//}
	//for (int i = 0; i < NX; ++i)
	//{
	//	for (int j = 1; j < NX - 1; ++j)
	//	{
	//		for (int k = 1; k < NX - 1; ++k)
	//		{
	//			_EField(i, j, k) = { dist(rng), dist(rng),dist(rng) };
	//			_BField(i, j, k) = { dist(rng), dist(rng),dist(rng) };
	//		}
	//	}
	//}
}

void EM_Simulation::UpdateNode(SceneNode::Ptr node)
{
	if (!_nodeInitialized)
	{
		auto ENode = node->addNode("E");
		auto BNode = node->addNode("B");
		_Erender = std::make_shared<VectorField>(NX, DX);
		_Brender = std::make_shared<VectorField>(NX, DX);
		(*_Erender) = _EField;
		(*_Brender) = _BField;
		auto Err = std::make_shared<VectorFieldRenderer>(_Erender);
		auto Brr = std::make_shared<VectorFieldRenderer>(_Brender);
		Brr->SetColor({ 0, 0, 255 });
		ENode->setRenderer(Err);
		BNode->setRenderer(Brr);
		_nodeInitialized = true;
	}
	else
	{
		step();
		(*_Erender) = _EField;
		(*_Brender) = _BField;
	}
}

void EM_Simulation::onStartSignal()
{
}

void EM_Simulation::onPauseSignal()
{
}

void EM_Simulation::SimulationLoop()
{
}

namespace
{
	vec3D Curl(VectorField const& F, int i, int j, int k)
	{
		return vec3D(
			(F(i, j + 1, k)[2] - F(i, j - 1, k)[2]) - (F(i, j, k + 1)[1] - F(i, j, k - 1)[1]),
			(F(i, j, k + 1)[0] - F(i, j ,k - 1)[0]) - (F(i + 1, j, k)[2] - F(i - 1, j, k)[2]),
			(F(i + 1, j, k)[1] - F(i - 1, j, k)[1]) - (F(i, j + 1, k)[0] - F(i, j - 1, k)[0])
		);
	}
}

void EM_Simulation::step()
{
	//static int cnt = 0;
	//if (cnt++ % 10)
	//	return;
	for (int i = 1; i < NX - 1; ++i)
	{

		for (int j = 1; j < NX - 1; ++j)
		{
			for (int k = 1; k < NX - 1; ++k)
			{
				_EField(i, j, k) += dt * Curl(_BField, i, j, k);
			}
		}
	}

	for (int i = 1; i < NX - 1; ++i)
	{

		for (int j = 1; j < NX - 1; ++j)
		{
			for (int k = 1; k < NX - 1; ++k)
			{
				_BField(i, j, k) -= dt * Curl(_EField, i, j, k);
			}
		}
	}
}
