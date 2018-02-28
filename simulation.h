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

class simulation {
public:

	simulation(size_t nAgents);
	void step();

protected:


protected:

	size_t		_nAgents;
	neural		_neuralEngine;
	physics		_physicsEngine;
	sheduler	_frameSheduler;

	// number of worker cpu threads, 0 for autodetect, 1 for singlethreaded version, ...
	size_t		_numThreads; 

};
