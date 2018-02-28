#include "neural.h"

#include "../vectorizedMathUtils/fasttrigo.h"
#include "../vectorizedMathUtils/xakepp35_sse_utils.h"


neural::neural(size_t nAgents)
{
	auto mmCount = mm_count(nAgents);
	_agentNeuronBiases = aalloc(mmCount);
	_agentNeuronActivations = aalloc(mmCount);
}

neural::~neural()
{
	_mm_free(_agentNeuronBiases);
	_mm_free(_agentNeuronActivations);
	_mm_free(_agentNeuronWeights);
}

void neural::init_weights(size_t inputWidth)
{
	auto mmCount = mm_count(inputWidth);
	_agentNeuronWeights = ?;
}

// here starts offset hell, in order to follow data layout and avoid shuffling at all costs
void neural::step_forward(mmr* inputData, size_t inputWidth, size_t endAgent, size_t startAgent) {
	for (size_t i = startAgent/4; i < endAgent/4; ++i) {
		auto neuronOffset = i*_nNeurons;
		for (size_t j = 0; j < _nNeurons; ++j) {
			auto agentNeuronDotProduct = _agentNeuronBiases[neuronOffset + j];
			auto inputOffset = i*inputWidth;
			auto dotResult = _mm_set_ps1(0);
			for (size_t k = 0; k < inputWidth; ++k)
				agentNeuronDotProduct = _mm_add_ps(agentNeuronDotProduct, _mm_mul_ps(inputData[inputOffset + k], _agentNeuronWeights[?]));
			//update_activations()
			_agentNeuronActivations[neuronOffset + j] = _mm_tanh_ps(agentNeuronDotProduct);
		}
	}
}

const float & neural::get_neuron_output(size_t i, size_t neuronIndex) const {
	auto neuronOffset = i/4*_nNeurons;
	return reinterpret_cast<const float*>(&_agentNeuronActivations[neuronOffset + neuronIndex])[i%4];
	// TODO: insert return statement here
}
