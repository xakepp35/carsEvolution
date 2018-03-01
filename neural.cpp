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

void neural::init_weights(size_t nAgents, size_t nNeurons, size_t inputWidth)
{
	auto mmCount = mm_count(nAgents*nNeurons*inputWidth);
	_agentNeuronWeights = aalloc(mmCount);
}

// here starts offset hell, in order to follow data layout and avoid shuffling at all costs
void neural::substep_calculate_decisions(size_t i, mmr* inputData, size_t inputWidth) {
	auto neuronOffset = i*_nNeurons;
	for (size_t j = neuronOffset+0; j < neuronOffset+_nNeurons; ++j)
		substep_neuron_compress(i, j, substep_neuron_calc_dot(i, j, inputData, inputWidth));
}

mmr neural::substep_neuron_calc_dot(size_t i, size_t j, const mmr* inputData, size_t inputWidth) const {
	auto agentNeuronDotProduct = _agentNeuronBiases[j];
	auto inputOffset = i*inputWidth;
	auto dotResult = _mm_set_ps1(0);
	for (size_t k = 0; k < inputWidth; ++k)
		agentNeuronDotProduct = _mm_add_ps(agentNeuronDotProduct, _mm_mul_ps(inputData[inputOffset + k], _agentNeuronWeights[inputOffset+k]));
}

void neural::substep_neuron_compress(size_t i, size_t j, mmr dotResult) {
	_agentNeuronActivations[j] = _mm_tanh_ps(dotResult);
}

const float & neural::get_neuron_output(size_t i, size_t neuronIndex) const {
	auto neuronOffset = i/4*_nNeurons;
	return reinterpret_cast<const float*>(&_agentNeuronActivations[neuronOffset + neuronIndex])[i%4];
	// TODO: insert return statement here
}
