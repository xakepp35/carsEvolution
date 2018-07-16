#include "neural.h"

#include "../vectorizedMathUtils/fasttrigo.h"
#include "../vectorizedMathUtils/xakepp35_sse_utils.h"


neural::neural(size_t nAgents, size_t nNeurons, size_t inputWidth):
	_nNeurons( nNeurons ),
	_inputWidth(inputWidth),
	_weightGen()
{
	auto mmCount = mm_count(nAgents*inputWidth);
	_agentInputData = aalloc(mmCount);

	mmCount = mm_count(nAgents*nNeurons);
	//_agentNeuronBiases = aalloc(mmCount);
	_agentNeuronActivations = aalloc(mmCount);
	

	mmCount = mm_count(nAgents*nNeurons*inputWidth);
	_agentNeuronWeights = aalloc(mmCount);
}


neural::~neural()
{
	_mm_free(_agentNeuronWeights);
	_mm_free(_agentNeuronActivations);
	//_mm_free(_agentNeuronBiases);
	_mm_free(_agentInputData);
}


void neural::predict_decision(competition::score_chart& accumulatedCost, size_t numAgents) {
	// runs neural network related stuff, to generate control decisions
	for (size_t i = 0; i < numAgents / 4; ++i) {
		auto neuronOffset = i*_nNeurons;
		for (size_t j = neuronOffset + 0; j < neuronOffset + _nNeurons; ++j) {
			auto dotValue = substep_neuron_calc_dot(i, j);
			substep_neuron_compress(i, j, dotValue);
		}
	}
}


void neural::new_predictor(size_t i, const competition::ranking_chart& rankingChart) {
	for (size_t j = 0; j < _nNeurons; ++j) {
		for (size_t k = 0; k < _inputWidth; ++k) {
			float newValue[4];
			_mm_store_ps(newValue, _weightGen.normal());
			set_agent_neuron_weight(i, j, k, newValue[0]);
		}
	}
	
}



mmr neural::substep_neuron_calc_dot(size_t i, size_t j) const {
	auto agentNeuronDotProduct = _mm_setzero_ps(); //auto agentNeuronDotProduct = _agentNeuronBiases[j];
	auto inputOffset = i*_inputWidth;
	auto weightOffset = (i*_inputWidth + j)*_nNeurons;
	auto dotResult = _mm_set_ps1(0);
	for (size_t k = 0; k < _inputWidth; ++k)
		agentNeuronDotProduct = _mm_add_ps(agentNeuronDotProduct, _mm_mul_ps(_agentInputData[inputOffset + k], _agentNeuronWeights[weightOffset +k]));
	return agentNeuronDotProduct;
}


void neural::substep_neuron_compress(size_t i, size_t j, mmr dotResult) {
	_agentNeuronActivations[j] = _mm_tanh_ps(dotResult);
}


void neural::set_agent_input(size_t i, size_t inputIndex, float inputValue) {
	reinterpret_cast<float*>(&_agentInputData[ i/4 ])[ inputIndex ] = inputValue;
}

void neural::set_agent_neuron_weight(size_t i, size_t neuronIndex, size_t inputIndex, float weightValue) {
	reinterpret_cast<float*>(&_agentNeuronWeights[i / 4])[neuronIndex * _inputWidth + inputIndex] = weightValue;
}

const float & neural::get_neuron_output(size_t i, size_t neuronIndex) const {
	auto neuronOffset = i/4*_nNeurons;
	return reinterpret_cast<const float*>(&_agentNeuronActivations[neuronOffset + neuronIndex])[i%4];
}
