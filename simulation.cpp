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

simulation::simulation(size_t nAgents):
	_nAgents(nAgents),
	_neuralEngine(nAgents),
	_physicsEngine(nAgents),
	_frameSheduler(),
	_numThreads(0),
	_agentRatingRank(nAgents)
{
	iota(_agentRatingRank.begin(), _agentRatingRank.end(), 0);
}


void simulation::step() {
	// process_user_input();
	// does sorting every step, yes it could be bad for random, but rerunning sort on sorted array is fast

	// replaces every dead agent with new best one, but a bit modified
	substep_natural_selection(_nAgents);
	
	// runs neural network related stuff, to generate control decisions
	substep_neural_network(_nAgents);

	// physics actuator, must fire 
	// - after neural, to avoid wrong moves on agent respawn
	// - before natural_selection, as it will decide respawns
	substep_motion_integrator(_nAgents);

	// sheduler: analyze performance, artificial delay to slow down simulation
	_frameSheduler.step();
}


void simulation::substep_natural_selection(size_t endAgent, size_t startAgent) {
	std::sort(_agentRatingRank.begin(), _agentRatingRank.end(),
		[this](size_t i0, size_t i1) {
		return _physicsEngine.get_agent_score(i0) > _physicsEngine.get_agent_score(i1);
	}
	);
	auto topIndex = _agentRatingRank[0];
	for (size_t i = startAgent; i < endAgent; i++) {
		if (_physicsEngine.get_agent_collision_flags(i)) { // for respawned : mutate/crossover

		}
		else {
			;
		}
	}
}


void simulation::substep_neural_network(size_t endAgent, size_t startAgent) {
	for (size_t i = startAgent / 4; i < endAgent / 4; ++i) {
		// calculate input data for neural net
		_physicsEngine.substep_find_max_inverse_distance_to_wall(i);

		// fire it
		_neuralEngine.substep_calculate_decisions(i, _physicsEngine._agentSensorProximity, _physicsEngine._nSensors);

		// transmit control decisions to physics engine (shuffling)
		for (size_t j = 0; j < physics::ControlCOUNT; j++)
			_physicsEngine.set_agent_control(i, j, _neuralEngine.get_neuron_output(i, j));
	}
}


void simulation::substep_motion_integrator(size_t endAgent, size_t startAgent) {
	mmr amountSteering = _mm_set_ps1(dT2 * steeringMagnitude);
	mmr amountAcceleration = _mm_set_ps1(dT2 * accelerationMagnitude);
	mmr agentRadiusSquared = _mm_set_ps1(sqrt(agentRadius));
	for (size_t i = startAgent / 4; i < endAgent / 4; ++i) {
		_physicsEngine.substep_integrate_rotation(i, amountSteering);
		_physicsEngine.substep_integrate_movement(i, amountAcceleration);
		_physicsEngine.substep_estimate_circular_path_advancement(i);
		_physicsEngine.substep_detect_collisions(i, agentRadiusSquared);
	}
}