/*
	Natural Selection algorithm for Perceptrone weights

	Describes simulation business logic
	Acts like a glue within several computational engines

	Date : Feb 2018
	Author : xakepp35@gmail.com
	License : FreeBSD(ISC) for non - commercial(personal, educational) use.
*/
#include "simulation.h"

simulation::simulation(size_t nAgents):
	_nAgents(nAgents),
	_neuralEngine(nAgents),
	_physicsEngine(nAgents),
	_frameSheduler(),
	_numThreads(0)
{}

//void user_input::step() {
	// if one day i would like to interact with this world, i will do it here :)
//}


//void logic::step() {
	//update_scores();
	//calculate_top_scores_chart();
//}



void simulation::step() {
	// process_user_input();

	//maintain_top_scores();
	size_t topIndex = 0;
	float topScore = 0;
	for (size_t i = 0; i < _nAgents; i++) {
		auto curScore = _physicsEngine.get_agent_score(i);
		if (curScore > topScore) {
			topScore = curScore;
			topIndex = i;
		}
	}

	for (size_t i = 0; i < _nAgents; i++) {
		//handle_natural_selection();
		if (_physicsEngine.get_agent_collision_flags(i)) { // for respawned : mutate/crossover
			
		}
		else {
			;
		}
	}
	// crafts input for neural network, so goes before its pass
	_physicsEngine.step_proximity_sensor(_nAgents);

	// calculate_control_decisions();
	_neuralEngine.step_forward(_physicsEngine._agentSensorProximity, _physicsEngine._nSensors, _nAgents);

	// as number of neurons could be greater than controls to agent physical body
	// void map_neural_outputs(); 
	for (size_t i = 0; i < _nAgents; i++) {
		for( size_t j = 0; j < physics::ControlCOUNT; j++)
			_physicsEngine.set_agent_control(i, j, _neuralEngine.get_neuron_output(i, j));

	// physics actuator, must fire only after neural, to avoid wrong moves on agent respawn
	_physicsEngine.step_motion_integrator(_nAgents);

	// must be before control logic, as it will decide respawns
	_physicsEngine.step_collision_detector(_nAgents);
	
	// sheduler: analyze performance, artificial delay to slow down simulation
	_frameSheduler.step();
}