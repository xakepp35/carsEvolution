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
#include "../vectorizedMathUtils/xoroshiro.h"

#include <vector>


class simulation:
	public competition
{
public:

	simulation(size_t nAgents, size_t nNeurons, size_t nWalls, size_t nSensors);

	// competition interface:
	virtual void translate_decision(i_problem& iProblem, const i_solver& iSolver) const override;
	virtual void translate_situation(i_solver& iSolver, const i_problem& iProblem) const override;

	void step();

protected:

	neural		_neuralEngine;
	physics		_physicsEngine;
	sheduler	_frameSheduler;

};
