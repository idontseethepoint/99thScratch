#include "LJ_Simulation.h"
#include "Application.h"
#include "MainWindow.h"
#include "MeshRenderer.h"
#include "EM.cuh"

#include <cmath>
#include <iostream>

LJ_Simulation::LJ_Simulation() : _params({
	0.001, 1.0,
	{{0, 0, 0}, {10, 10, 10}},
	{
		{100, 1, 1, 10},
		{1, 1, 1, 50},
		{1, 0.5, 1, 150},
	},
	false
	}), _cudaEv(nullptr)
{
	QObject::connect(Application::App(), &Application::StartSimulation,
		this, &LJ_Simulation::onStartSignal);
	QObject::connect(Application::App(), &Application::StopSimulation,
		this, &LJ_Simulation::onPauseSignal);
}

void LJ_Simulation::SetParams(LJ_Parameters const& params)
{
	_params = params;
	_cur = State();
	SetParamMatrices();

	_nTotal = _cur.Types.size();
	if (_nTotal == 0)
		return;
	auto const& box = _params.Box;
	auto d = box.High - box.Low;
	auto Vol = box.Volume();
	if (Vol == 0)
		return;
	auto crtVol = std::pow(Vol, 1.0 / 3);
	std::array<int, 3> n;
	std::array<double, 3> dx;
	int f = std::ceil(std::pow(_nTotal, 1.0 / 3));
	for (int iDim = 0; iDim < 3; ++iDim)
	{
		n[iDim] = std::floor(1 + f * d[iDim] / crtVol);
		dx[iDim] = d[iDim] / n[iDim];
	}
	for (int ix = 0; ix < n[0]; ++ix)
	{
		float x = box.Low[0] + dx[0] * ix;
		for (int iy = 0; iy < n[1]; ++iy)
		{
			float y = box.Low[1] + dx[1] * iy;
			for (int iz = 0; iz < n[2]; ++iz)
			{
				float z = box.Low[2] + dx[2] * iz;
				_cur.Atoms.push_back({ { x, y, z }, { 0, 0, 0 } });
				if (_cur.Atoms.size() >= _nTotal)
					goto doneAtoms;
			}
		}
	}
	doneAtoms:

	_renderState = _cur;

	if (params.UseCUDA)
	{
		_cudaEv.reset(new LJ_CUDA_Evolver(_params, _cur.Types, _cur.Atoms));
	}
	else
	{
		_cudaEv.reset(nullptr);
	}
}

void LJ_Simulation::UpdateNode(SceneNode::Ptr node)
{
	State toShow;
	{
		std::unique_lock<std::mutex> renderLock;
		toShow = _renderState;
	}
	_simStats = calculateStats(toShow);
	emit SimStatsUpdate(_simStats);
	static const std::vector<QColor> TypeColors{
		{0, 0, 255},
		{255, 0, 0},
		{0, 255, 0},
		{255, 255, 0},
		{255, 0, 255},
		{0, 255, 255},
		{255, 255, 255},
		{155, 155, 155},
	};
	if (!node)
		return;
	if (_nodeNeedsReset)
	{
		node->Clear();
		_nodeNeedsReset = false;
	}
	if (node->nChild() < 2 && toShow.Atoms.size() > 0)
	{
		for (int iAt = 0; iAt < _nTotal; ++iAt)
		{
			auto ty = toShow.Types[iAt];
			auto atNode = node->addNode(QString("Atom ") + QString::number(iAt));
			auto rer = MeshRenderer::Sphere({ 0,0,0 }, _SigmaMatrix[ty][ty]);
			rer->SetColor(TypeColors[ty % TypeColors.size()]);
			atNode->setRenderer(rer);
			QMatrix4x4 t;
			t.translate(toShow.Atoms[iAt].Pos.Qvec());
			t.scale(_atomScale);
			atNode->setTransform(t);
		}
		auto boxNode = node->addNode("Box");
		auto const& box = _params.Box;
		auto Lvec = box.High - box.Low;
		static constexpr double D_OFF = 0.05;
		vec3Dd off_base = { D_OFF, D_OFF, D_OFF };
		for (auto iLongDim = 0; iLongDim < 3; ++iLongDim)
		{
			int jDim = (iLongDim + 1) % 3;
			int kDim = (iLongDim + 2) % 3;
			for (int iSide1 = 0; iSide1 <= 1; ++iSide1)
			{
				for (int iSide2 = 0; iSide2 <= 1; ++iSide2)
				{
					auto bLow = box.Low - off_base;
					auto bHigh = box.Low + off_base;
					bHigh[iLongDim] = box.High[iLongDim] + D_OFF;
					bHigh[jDim] += iSide1 * Lvec[jDim];
					bHigh[kDim] += iSide2 * Lvec[kDim];
					bLow[jDim]  += iSide1 * Lvec[jDim];
					bLow[kDim]  += iSide2 * Lvec[kDim];
					boxNode->addBoxNode(bLow, bHigh, "BoxSide");
				}
			}
		}
	}
	else //if (node->nChild() == _nTotal)
	{
		for (int iAt = 0; iAt < toShow.Atoms.size(); ++iAt)
		{
			QMatrix4x4 t;
			t.translate(toShow.Atoms[iAt].Pos.Qvec());
			t.scale(_atomScale);
			auto c = node->childAt(iAt);
			if (!c)
			{
				std::cout << "FAILED TO FIND NODE!\n";
			}
			else
			{
				c->setTransform(t);
			}
		}
	}
	_needRenderUpate = true;
}

void LJ_Simulation::onAtomScaleChange(double scale)
{
	_atomScale = scale;
}

void LJ_Simulation::onStartSignal(LJ_Parameters const& params)
{
	switch (_runState)
	{
	case RunState::NotStarted:
		SetParams(params);
		emit SimulationReady();
		_runState = RunState::Running;
		_simThread = std::thread(&LJ_Simulation::SimulationLoop, this);
		break;
	case RunState::Paused:
		_params.TimeStep = params.TimeStep;
		_params.OffDiagFactor = params.OffDiagFactor;
		if (_params.Box != params.Box || 
			_params.Atoms.size() != params.Atoms.size() ||
			_params.UseCUDA != params.UseCUDA)
		{
			_nodeNeedsReset = true;
			SetParams(params);
		}
		else
		{
			for (int iAt = 0; iAt < params.Atoms.size(); ++iAt)
			{
				if (_params.Atoms[iAt].N != params.Atoms[iAt].N ||
					_params.Atoms[iAt].Sigma != params.Atoms[iAt].Sigma)
				{
					_nodeNeedsReset = true;
					SetParams(params);
					break;
				}
				else
				{
					_params.Atoms[iAt].Eps = params.Atoms[iAt].Eps;
					_params.Atoms[iAt].Mass = params.Atoms[iAt].Mass;
				}
			}
		}
		if (_cudaEv)
		{
			_cudaEv->setBox(_params);
			_cudaEv->setParamTable(_params);
		}
		SetParamMatrices();
		_runState = RunState::Running;
		break;
	case RunState::Running:
		break;

	}
}

void LJ_Simulation::onHeatPress()
{
	if (_cudaEv)
	{
		_cudaEv->heatCool(1.25);
	}
	else
	{
		_needsHeat = true;
	}
}

void LJ_Simulation::onCoolPress()
{
	if (_cudaEv)
	{
		_cudaEv->heatCool(0.8);
	}
	else
	{
		_needsCool = true;
	}
}

void LJ_Simulation::onPauseSignal()
{
	if (_runState == RunState::Running)
		_runState = RunState::Paused;
}

void LJ_Simulation::SetParamMatrices()
{
	auto nType = _params.Atoms.size();
	_EpsMatrix = std::vector<std::vector<double>>(nType,
		std::vector<double>(nType, 0));
	_SigmaMatrix = std::vector<std::vector<double>>(nType,
		std::vector<double>(nType, 0));
	for (int iAt = 0; iAt < nType; ++iAt)
	{
		_EpsMatrix[iAt][iAt] = _params.Atoms[iAt].Eps;
		_SigmaMatrix[iAt][iAt] = _params.Atoms[iAt].Sigma;
		_cur.Types.insert(_cur.Types.end(), _params.Atoms[iAt].N, iAt);
		for (int jAt = iAt + 1; jAt < nType; ++jAt)
		{
			_EpsMatrix[iAt][jAt] = _params.OffDiagFactor * std::sqrt(
				_params.Atoms[iAt].Eps * _params.Atoms[jAt].Eps);
			_EpsMatrix[jAt][iAt] = _EpsMatrix[iAt][jAt];

			_SigmaMatrix[iAt][jAt] = 0.5 * (
				_params.Atoms[iAt].Sigma + _params.Atoms[jAt].Sigma);
			_SigmaMatrix[jAt][iAt] = _SigmaMatrix[iAt][jAt];
		}
	}
}

void LJ_Simulation::SimulationLoop()
{
	_t = 0;
	std::unique_lock<std::mutex> lock(_runMutex);
	while (1)
	{
		if (_runState == RunState::Stopped)
			return;
		while (_runState == RunState::Paused)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		if (_needsHeat)
		{
			_needsHeat = false;
			ScaleVelocities(1.25);
		}
		if (_needsCool)
		{
			_needsCool = false;
			ScaleVelocities(0.8);
		}
		step();
		_t += _params.TimeStep;
	}
}

void LJ_Simulation::ScaleVelocities(double vFactor)
{
	for (auto& at : _cur.Atoms)
	{
		at.Vel *= vFactor;
	}
}

void LJ_Simulation::Reset()
{
	_runState = RunState::Stopped;
	_nodeNeedsReset = true;
	{
		std::unique_lock<std::mutex> renderLock(_renderStateMutex);
		_renderState = State();
	}
	{
		std::unique_lock<std::mutex> runLock(_runMutex);
		_nTotal = 0;
		_t = 0;
		_cur = State();
	}
}

vec3Dd LJ_Simulation::force(vec3Dd const& p1, vec3Dd const& p2, int ty1, int ty2)
{
	auto v = p1 - p2;
	auto r2Inv = 1.0 / (v * v);
	auto eps = _EpsMatrix[ty1][ty2];
	auto sigma = _SigmaMatrix[ty1][ty2];
	auto s2inv = (sigma * sigma) * r2Inv;
	auto f6 = s2inv * s2inv * s2inv;
	auto f12 = f6 * f6;

	return 24 * eps * (2 * f12 - f6) * r2Inv * v;
}

double LJ_Simulation::U(vec3Dd const& p1, vec3Dd const& p2, int ty1, int ty2)
{
	auto v = p1 - p2;
	auto r2Inv = 1.0 / (v * v);
	auto eps = _EpsMatrix[ty1][ty2];
	auto sigma = _SigmaMatrix[ty1][ty2];
	auto s2inv = (sigma * sigma) * r2Inv;
	auto f6 = s2inv * s2inv * s2inv;
	auto f12 = f6 * f6;

	return 4 * eps * (f12 - f6);
}

LJ_Simulation::SimStats LJ_Simulation::calculateStats(State const& s)
{
	SimStats ret{ 0, 0 };
	for (int iAt = 0; iAt < s.Atoms.size(); ++iAt)
	{
		ret.KE += s.Atoms[iAt].Vel * s.Atoms[iAt].Vel;
		for (int jAt = iAt + 1; jAt < s.Atoms.size(); ++jAt)
		{
			ret.PE += U(s.Atoms[iAt].Pos, s.Atoms[jAt].Pos,
				s.Types[iAt], s.Types[jAt]);
		}
	}
	ret.KE *= 0.5;
	ret.t = _t;
	return ret;
}

void LJ_Simulation::reflect(AtomState& s)
{
	for (int iDim = 0; iDim < 3; ++iDim)
	{
		if (s.Pos[iDim] < _params.Box.Low[iDim])
		{
			s.Pos[iDim] = _params.Box.Low[iDim];
			s.Vel[iDim] *= -1;
		}
		else if (s.Pos[iDim] > _params.Box.High[iDim])
		{
			s.Pos[iDim] = _params.Box.High[iDim];
			s.Vel[iDim] *= -1;
		}
	}
}

void LJ_Simulation::step()
{
	if (_cudaEv)
	{
		_cudaEv->evolve(_params.TimeStep);
		_cur.Atoms = _cudaEv->getAtomStates();
		//for (int iAt = 0; iAt < _nTotal; ++iAt)
		//{
		//	reflect(_cur.Atoms[iAt]);
		//}
	}
	else
	{
		std::vector<AtomState> nextAtoms = _cur.Atoms;
		_Fi = std::vector<vec3Dd>(_nTotal, { 0,0,0 });
		for (int iAt = 0; iAt < _nTotal; ++iAt)
		{
			for (int jAt = iAt + 1; jAt < _nTotal; ++jAt)
			{
				auto fij = force(_cur.Atoms[iAt].Pos, _cur.Atoms[jAt].Pos,
					_cur.Types[iAt], _cur.Types[jAt]);
				_Fi[iAt] += fij;
				_Fi[jAt] -= fij;
			}
		}
		for (int iAt = 0; iAt < _nTotal; ++iAt)
		{
			nextAtoms[iAt].Vel += _Fi[iAt] * _params.TimeStep;
			nextAtoms[iAt].Pos += nextAtoms[iAt].Vel * _params.TimeStep;
			reflect(nextAtoms[iAt]);
		}
		_cur.Atoms = nextAtoms;
	}
	if (_needRenderUpate)
	{
		std::unique_lock<std::mutex> renderLock(_renderStateMutex);
		_renderState = _cur;
	}
}
