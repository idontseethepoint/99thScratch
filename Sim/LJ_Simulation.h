#pragma once

#include "AABB.h"
#include "SceneNode.h"
#include "vec3D.h"

#include <QObject>

#include <mutex>
#include <thread>
#include <vector>

class LJ_Simulation : public QObject
{
	Q_OBJECT
public:
	struct SpeciesParams
	{
		double Eps;
		double Sigma;
		double Mass;
		int N;
	};
	struct Parameters
	{
		double TimeStep;
		AABB Box;
		std::vector<SpeciesParams> Atoms;
	};
	struct SimStats
	{
		double KE;
		double PE;
		double t;
	};
	LJ_Simulation();
	void SetParams(Parameters const& params);
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
	void onStartSignal(Parameters const& params);
	void onPauseSignal();
private:
	void SimulationLoop();
	void ScaleVelocities(double vFactor);
	struct AtomState
	{
		vec3Dd Pos;
		vec3Dd Vel;
	};
	struct State
	{
		std::vector<AtomState> Atoms;
		std::vector<int> Types;
	};
	enum class RunState
	{
		NotStarted,
		Running,
		Paused
	};
	RunState _runState = RunState::NotStarted;
	State _cur;
	State _renderState;
	std::mutex _renderStateMutex;
	std::thread _simThread;
	int _nTotal;
	bool _needRenderUpate = false;
	Parameters _params;
	std::vector<std::vector<double>> _EpsMatrix;
	std::vector<std::vector<double>> _SigmaMatrix;
	vec3Dd force(vec3Dd const& p1, vec3Dd const& p2, int ty1, int ty2);
	double U(vec3Dd const& p1, vec3Dd const& p2, int ty1, int ty2);
	SimStats calculateStats(State const& s);
	SimStats _simStats;
	void reflect(AtomState& s);
	void step();
	double _t = 0;
	double _atomScale = 1.0;
	bool _needsHeat = false;
	bool _needsCool = false;
};