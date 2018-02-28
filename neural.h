#pragma once

#include "base.h"

class neural {
public:

	neural(size_t nAgents);
	~neural();

	void init_weights(size_t inputWidth);
	void step_forward(mmr* inputData, size_t inputWidth, size_t endAgent, size_t startAgent = 0);

	// shuffles for control logic:
	const float& get_neuron_output(size_t i, size_t neuronIndex) const;

	MMR* _agentNeuronBiases;

	MMR* _agentNeuronActivations;

	MMR* _agentNeuronWeights;

	size_t _nNeurons;

};
