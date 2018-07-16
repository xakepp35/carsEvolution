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
	_frameSheduler(),
	_numThreads(0)
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
	_frameSheduler.step();
}
