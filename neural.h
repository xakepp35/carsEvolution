#pragma once

#include "base.h"
#include "competition.h"

#include "../vectorizedMathUtils/xoroshiro.h"


class neural: public competition::i_solver {
public:

	neural(size_t nAgents, size_t nNeurons, size_t inputWidth);
	~neural();

	// i_solver interface
	virtual void predict_decision(competition::score_chart& accumulatedCost, size_t numAgents) override;
	virtual void new_predictor(size_t misbehavingIndex, const competition::ranking_chart& rankingChart, bool bestExists) override;

	mmr substep_neuron_calc_dot(size_t i, size_t j) const;
	void substep_neuron_compress(size_t i, size_t j, mmr dotResult);
	
	// shuffles for glue logic (setters):
	void set_agent_input(size_t i, size_t inputIndex, float inputValue);
	void set_agent_neuron_weight(size_t i, size_t neuronIndex, size_t inputIndex, float weightValue);
	float get_agent_neuron_weight(size_t i, size_t neuronIndex, size_t inputIndex) const;

	// shuffles for glue logic (getters):
	const float& get_neuron_output(size_t i, size_t neuronIndex) const;

	// store for input data
	MMR* _agentInputData;

	// per agent, per neuron
	//MMR* _agentNeuronBiases;

	// per agent
	MMR* _agentNeuronActivations;

	// 3D array: per agent/ per neuron/ per input
	MMR* _agentNeuronWeights;

	xoroshiro	_weightGen;

	size_t _nNeurons;
	size_t _inputWidth;

};
