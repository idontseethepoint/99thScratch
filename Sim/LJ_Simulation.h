#pragma once

#include "AABB.h"
#include "AtomState.h"
#include "LJ_Parameters.h"
#include "SceneNode.h"
#include "vec3D.h"

#include <QObject>

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class LJ_CUDA_Evolver;

class LJ_Simulation : public QObject
{
	Q_OBJECT
public:
	struct SimStats
	{
		double KE;
		double PE;
		double t;
	};
	LJ_Simulation();
	void SetParams(LJ_Parameters const& params);
	void UpdateNode(SceneNode::Ptr node);
	inline SimStats GetSimStats() const { return _simStats; }
signals:
	void SimStatsUpdate(SimStats const& newStats);
	void SimulationReady();
public slots:
	void onAtomScaleChange(double scale);
	void onHeatPress();
	void onCoolPress();
private slots:
	void onStartSignal(LJ_Parameters const& params);
	void onPauseSignal();
private:
	void SetParamMatrices();
	void SimulationLoop();
	void ScaleVelocities(double vFactor);
	void Reset();
	struct State
	{
		std::vector<AtomState> Atoms;
		std::vector<int> Types;
	};
	enum class RunState
	{
		NotStarted,
		Running,
		Paused,
		Stopped
	};
	vec3Dd force(vec3Dd const& p1, vec3Dd const& p2, int ty1, int ty2);
	double U(vec3Dd const& p1, vec3Dd const& p2, int ty1, int ty2);
	SimStats calculateStats(State const& s);
	void reflect(AtomState& s);
	void step();

	RunState _runState = RunState::NotStarted;
	State _cur;
	State _renderState;
	std::mutex _renderStateMutex;
	std::mutex _runMutex;
	std::thread _simThread;
	int _nTotal;
	bool _needRenderUpate = false;
	bool _nodeNeedsReset = false;
	LJ_Parameters _params;
	std::vector<std::vector<double>> _EpsMatrix;
	std::vector<std::vector<double>> _SigmaMatrix;
	SimStats _simStats;
	double _t = 0;
	double _atomScale = 1.0;
	bool _needsHeat = false;
	bool _needsCool = false;
	std::vector<vec3Dd> _Fi;
	std::unique_ptr<LJ_CUDA_Evolver> _cudaEv;
};