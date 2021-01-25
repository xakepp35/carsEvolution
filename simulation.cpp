/*
	Natural Selection algorithm for Perceptrone weights

	Describes simulation business logic
	Acts like a glue within several computational engines

	Date : Feb 2018
	Author : xakepp35@gmail.com
	License : FreeBSD(ISC) for non - commercial(personal, educational) use.
*/
#include "simulation.h"

#include <numeric>	// std::iota
#include <algorithm> // std::sort


simulation::simulation(size_t nAgents, size_t nNeurons, size_t nWalls, size_t nSensors):
	competition(nAgents),
	_neuralEngine(nAgents, nNeurons, nSensors),
	_physicsEngine(nAgents, nWalls, nSensors),
	_tickSheduler()
{}


void simulation::translate_decision(i_problem& iProblem, const i_solver& iSolver) const {
	auto& currentPhysics = static_cast<physics&>(iProblem);
	auto& currentNeural = static_cast<const neural&>(iSolver);

	// transmit control decisions to physics engine (shuffling, slow)
	for (size_t i = 0; i < participant_count(); ++i)
		for (size_t j = 0; j < physics::ControlCOUNT; ++j)
			currentPhysics.set_agent_control(i, j, currentNeural.get_neuron_output(i, j));
}


void simulation::translate_situation(i_solver& iSolver, const i_problem& iProblem) const {
	auto& currentNeural = static_cast<neural&>(iSolver);
	auto& currentPhysics = static_cast<const physics&>(iProblem);

	// transmit situation sense to neural engine (shuffling, slow)
	for (size_t i = 0; i < participant_count(); ++i)
		for (size_t j = 0; j < currentNeural._inputWidth; ++j)
			currentNeural.set_agent_input(i, j, currentPhysics.get_agent_sensor_value(i, j));
}


void simulation::step() {
	// keyboard/mouse interaction: pause/control speed/...
	// process_user_input();
	
	// go go go
	competition_step(_neuralEngine, _physicsEngine);

	// sheduler: analyze performance, artificial delay to slow down simulation
	_tickSheduler.step();

	auto& dataDynamic(_frameDrawer._renderSync[drawer_gl1::render_sync::Writer]);
	dataDynamic.stream_data(_tickSheduler._nStep, participant_count(), _physicsEngine._agentPosX, _physicsEngine._agentPosY, _physicsEngine._agentAngle);
}

void simulation::physics_loop() {
	while (true)
		step();
}

#include "renderer.h"

template< typename T>
T sqr(T x) {
	return x*x;
}

void simulation::main() {

	_physicsEngine._simConfig.dT = 1.0f/1024; // fixed time delta for physics simulation
	_physicsEngine._simConfig.dT2 = sqr(_physicsEngine._simConfig.dT); // squared time delta;
	_physicsEngine._simConfig.agentRadius = 1.0f / 64;
	_physicsEngine._simConfig.steeringMagnitude = 64;
	_physicsEngine._simConfig.accelerationMagnitude = 64;

	_physicsEngine._spawnConfig.agentTTL = 0xffffff; // max time to live in ticks

	_physicsEngine._spawnConfig.agentRespawnAngle = 0.0f;
	_physicsEngine._spawnConfig.agentRespawnPosX = 0.0f;
	_physicsEngine._spawnConfig.agentRespawnPosY = 0.8f;

	_frameDrawer._dataStatic.carRadius = _physicsEngine._simConfig.agentRadius;

	//void engine::generate_rectangle_track() {
	_frameDrawer._dataStatic.register_wall(0.9f, 0.9f, 0.9f, -0.9f);
	_frameDrawer._dataStatic.register_wall(0.9f, -0.9f, -0.9f, -0.9f);
	_frameDrawer._dataStatic.register_wall(-0.9f, -0.9f, -0.9f, 0.9f);
	_frameDrawer._dataStatic.register_wall(-0.9f, 0.9f, 0.9f, 0.9f);
	_frameDrawer._dataStatic.register_wall(0.7f, 0.7f, 0.7f, -0.8f);
	_frameDrawer._dataStatic.register_wall(0.7f, -0.8f, -0.85f, -0.8f);
	_frameDrawer._dataStatic.register_wall(-0.85f, -0.8f, -0.85f, 0.7f);
	_frameDrawer._dataStatic.register_wall(-0.85f, 0.7f, 0.7f, 0.7f);

	for (size_t i = 0; i < _frameDrawer._dataStatic.vWalls.size(); ++i) {
		auto& curWall = _frameDrawer._dataStatic.vWalls[i];
		_physicsEngine._wallS0X[i / 4].m128_f32[i % 4] = curWall[0];
		_physicsEngine._wallS0Y[i / 4].m128_f32[i % 4] = curWall[1];
		_physicsEngine._wallS0S1X[i / 4].m128_f32[i % 4] = -(curWall[2]- curWall[0]);
		_physicsEngine._wallS0S1Y[i / 4].m128_f32[i % 4] = -(curWall[3]- curWall[1]);
	}
	//}

	renderer frameRenderer(800, 800);
	frameRenderer.set_vsync_interval(1);
	_physicsThread = std::thread(&simulation::physics_loop, this);
	frameRenderer.main_loop(_frameDrawer, _frameDrawer);
	_physicsThread.join(); // stuck for infinity

}