#pragma once

#include "base.h"

class neural {
public:

	neural(size_t nAgents);
	~neural();

	void init_weights(size_t nAgents, size_t nNeurons, size_t inputWidth);

	void substep_calculate_decisions(size_t i, mmr* inputData, size_t inputWidth);
	mmr substep_neuron_calc_dot(size_t i, size_t j, const mmr* inputData, size_t inputWidth) const;
	void substep_neuron_compress(size_t i, size_t j, mmr dotResult);

	// shuffles for control logic:
	const float& get_neuron_output(size_t i, size_t neuronIndex) const;

	// per agent, per neuron
	MMR* _agentNeuronBiases;

	// per agent
	MMR* _agentNeuronActivations;

	// 3D array: per agent/ per neuron/ per input
	MMR* _agentNeuronWeights;

	size_t _nNeurons;

};
