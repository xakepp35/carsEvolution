#include "neural.h"

#include "../vectorizedMathUtils/fasttrigo.h"
#include "../vectorizedMathUtils/xakepp35_sse_utils.h"

/*
	assumptions:

	* values located seqentially in the input field are correlated with high probability
		so for a group of inputs for single neuron we could store only one offset, achieving 4x memory economy

	* perceptrone neuron with 16 inputs can be constructed with means of 4 neurons with 4 inputs,
		so its suffient to have fixed inputs count to build system of any complexity, achieving fixed memory and calc time footprint

	* number of synapses to neuron count ratio varies a little and mostly dependans on existance of complex visual cortex with high spatial locality props.
		for olfactory-based insects its somewhere between 30x and 40x, so 4 inputs per neuron seems to be good choise for simple tasks
		for mammals its around 1000x, so 16 inputs per neuron would give better results to select spatially close locations and converge to single value in less than 3 levels
		64 inputs is way too high value, which gives 262144 inputs in successful hierarchial advancement, is not observed anywhere and seems to be useless (unless we have 3D-volumetric spatially close data)

*/


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
		for (size_t j =  + 0; j < _nNeurons; ++j) {
			auto dotValue = substep_neuron_calc_dot(i, neuronOffset+j);
			substep_neuron_compress(i, neuronOffset + j, dotValue);
		}
	}

	for (size_t i = 0; i < numAgents; ++i)
		accumulatedCost[i] += _nNeurons;
}

// TODO: sse distributions, weight lerping, so on
void neural::new_predictor(size_t i, const competition::ranking_chart& rankingChart, bool bestExists) {
	// std::uniform_real_distribution<scalar> uDweights(wMin, wMax);
	if (bestExists) { // there exists some best agent. make a copy of him and do crossover mutation with random changes
		for (size_t j = 0; j < _nNeurons; ++j) {
			for (size_t k = 0; k < _inputWidth; ++k) {
				//	reinterpret_cast<scalar*>(outResult._vWeight.data())[i] = lerp(reinterpret_cast<const scalar*>(&_vWeight[0])[i], reinterpret_cast<const scalar*>(&targetcell._vWeight[0])[i], targetAlpha);
			}
		}
	}
	else {	// we just started or cost efficient agent is still not found: better fill predictor's brain with fully random stuff
		for (size_t j = 0; j < _nNeurons; ++j) { // fully random assigment would generate correct decision faster than small random adjustment at the very beginning
			for (size_t k = 0; k < _inputWidth; ++k) {
				float newValue[4];
				_mm_store_ps(newValue, _weightGen.normal());
				set_agent_neuron_weight(i, j, k, newValue[0]);
			}
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
