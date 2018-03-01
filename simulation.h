/*
Natural Selection algorithm for Perceptrone weights

Simulation:
	Describes simulation business logic
	Acts like a glue within several computational engines

Date : Feb 2018
Author : xakepp35@gmail.com
License : FreeBSD(ISC) for non - commercial(personal, educational) use.
*/
#pragma once

//#include <memory>
//#include <vector>
#include "neural.h"
#include "physics.h"
#include "sheduler.h"

#include <vector>

class simulation {
public:

	simulation(size_t nAgents);
	void step();
	void substep_natural_selection(size_t endAgent, size_t startAgent = 0);
	void substep_neural_network(size_t endAgent, size_t startAgent = 0);
	void substep_motion_integrator(size_t endAgent, size_t startAgent = 0);
	

protected:

	std::vector< uint32_t >	_agentRatingRank; // indexes of agents, sorted by max scores

protected:

	size_t		_nAgents;
	neural		_neuralEngine;
	physics		_physicsEngine;
	sheduler	_frameSheduler;

	float dT; // fixed time delta for physics simulation
	float dT2; // squared time delta;
	float agentRadius; // = 1.0f / 64
	float steeringMagnitude; // = 64;
	float accelerationMagnitude; // = 64;

	// number of worker cpu threads, 0 for autodetect, 1 for singlethreaded version, ...
	size_t		_numThreads; 

};
