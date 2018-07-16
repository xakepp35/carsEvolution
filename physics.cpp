#include "physics.h"

#include <cmath>

#include "../vectorizedMathUtils/xakepp35_sse_utils.h"
#include "../vectorizedMathUtils/mathfun.h"
#include "../vectorizedMathUtils/fasttrigo.h"


physics::physics(size_t nAgents, size_t nWalls, size_t nSensors):
	//_nAgents(nAgents),
	_nWalls(nWalls),
	_nSensors(nSensors),
	_wallS0X(nullptr),
	_wallS0Y(nullptr),
	_wallS0S1X(nullptr),
	_wallS0S1Y(nullptr),
	_sensorAngle(nullptr),
	_agentSensorProximity(nullptr)
{
	auto mmCount = mm_count(nAgents);
	for (size_t i = 0; i < ControlCOUNT; ++i)
		_controlData[i] = aalloc(mmCount);
	
	_agentAngle				= aalloc(mmCount);
	//_agentDirX				= aalloc<mmr>(mmCount);
	//_agentDirY				= aalloc<mmr>(mmCount);
	_agentPosX				= aalloc(mmCount);
	_agentPosY				= aalloc(mmCount);
	_agentPPosX				= aalloc(mmCount);
	_agentPPosY				= aalloc(mmCount);
	_agentPathAdvancement	= aalloc(mmCount);
	_agentCollision			= aalloc(mmCount);
	_agentTTL				= aalloc<__m128i>(mmCount);

	// init walls
	mmCount = mm_count(nWalls);
	_wallS0X = aalloc(mmCount);
	_wallS0Y = aalloc(mmCount);
	_wallS0S1X = aalloc(mmCount);
	_wallS0S1Y = aalloc(mmCount);

	// init sensors
	mmCount = mm_count(nSensors);
	_sensorAngle = aalloc(mmCount);

	mmCount = mm_count(nAgents*nSensors);
	_agentSensorProximity = aalloc(mmCount);

}


physics::~physics() {
	for( size_t i = 0; i < ControlCOUNT; ++i )
		_mm_free(_controlData[i]);
	_mm_free(_agentAngle);
	//_mm_free(_agentDirX);
	//_mm_free(_agentDirY);
	_mm_free(_agentPosX);
	_mm_free(_agentPosY);
	_mm_free(_agentPPosX);
	_mm_free(_agentPPosY);
	_mm_free(_agentPathAdvancement);
	_mm_free(_wallS0X);
	_mm_free(_wallS0Y);
	_mm_free(_wallS0S1X);
	_mm_free(_wallS0S1Y);
	_mm_free(_agentCollision);
	_mm_free(_agentTTL);
	_mm_free(_sensorAngle);
	_mm_free(_agentSensorProximity);
}


void physics::describe_situation(size_t numAgents) {
	for (size_t i = 0; i < numAgents / 4; ++i)
		substep_find_max_inverse_distance_to_wall(i);
}


void physics::update_situation(competition::score_chart& accumulatedScore, size_t numAgents) {
	// physics actuator: first loads physics constants for SSE
	mmr amountSteering = _mm_set_ps1(_simConfig.dT2 * _simConfig.steeringMagnitude);
	mmr amountAcceleration = _mm_set_ps1(_simConfig.dT2 * _simConfig.accelerationMagnitude);
	mmr agentRadiusSquared = _mm_set_ps1(sqrt(_simConfig.agentRadius));

	// do physics step stuff (SSE/AVX, heavyweight calculations)
	for (size_t i = 0; i < numAgents / 4; ++i) {
		substep_integrate_rotation(i, amountSteering);
		substep_integrate_movement(i, amountAcceleration);
		substep_estimate_circular_path_advancement(i);
		substep_detect_collisions(i, agentRadiusSquared);
	}

	// update scores (individually, bool, lightweight logic)
	for (size_t i = 0; i < numAgents; ++i) {
		accumulatedScore[i] += get_agent_score(i); // updates score as it moves along circular track path
		if (get_agent_collision_flags(i)) // whoops... BUMP!
			accumulatedScore[i] = -1; // kill the bill
	}
}


void physics::new_performer(size_t i, const competition::ranking_chart& rankingChart) {
	set_agent_angle(i, _simConfig.agentRespawnAngle);
	set_agent_position(i, _simConfig.agentRespawnPosX, _simConfig.agentRespawnPosY);
	set_agent_collision_flags(i, 0);
}


void physics::substep_find_max_inverse_distance_to_wall(size_t i) {
	auto agentSensorArrayOffset = i*_nSensors;
	for (size_t k = 0; k < _nSensors; k++) {
		_agentSensorProximity[agentSensorArrayOffset + k] = _mm_setzero_ps();
	}
	auto agentWallArrayOffset = i*_nWalls;
	for (size_t j = 0; j < _nWalls; ++j) {
		//get_agent_wall_distances();
		auto wallToAgentX = _mm_sub_ps(_agentPosX[i], _wallS0X[j]);
		auto wallToAgentY = _mm_sub_ps(_agentPosY[i], _wallS0Y[j]);

		//calculate_sensor_proximity();
		for (size_t k = 0; k < _nSensors; k++) { // really heavyweight powerlifter here: for(...) for(...) for(...) do_hardcore();
												 // get_sensor_direction();
			auto agentSensorAngle = _mm_add_ps(_agentAngle[i], _sensorAngle[k]);
			__m128 sensorDirX, sensorDirY;
			FT::sincos_ps(agentSensorAngle, &sensorDirX, &sensorDirY);
			// proximity=max(proximity, get_sensor_proximity());
			auto newSensorProximity = pu_ray_segment_distance_inverse(wallToAgentX, wallToAgentY, _wallS0S1X[j], _wallS0S1Y[j], sensorDirX, sensorDirY);
			_agentSensorProximity[agentSensorArrayOffset + k] = _mm_max_ps(_agentSensorProximity[agentSensorArrayOffset + k], newSensorProximity);
		}
	}
}


void physics::substep_integrate_rotation(size_t i, mmr amountSteering) {
	//void rotation_integration_step();
	auto agentPrevAngle = _agentAngle[i];
	auto agentNewAngle = _mm_add_ps(agentPrevAngle, _mm_mul_ps(_controlData[ControlSteering][i], amountSteering));
	//void normalize_angle(); todo - make it cheaper with 2 CMP instructions?
	agentNewAngle = pu_fmod_normalize_angle(agentNewAngle);
	//void update_rotation();	
	_agentAngle[i] = agentNewAngle;
}


void physics::substep_integrate_movement(size_t i, mmr amountAcceleration) {
	//void calculate_movement_heading_direction();
	__m128 agentDirX, agentDirY;
	FTA::sincos_ps(_agentAngle[i], &agentDirY, &agentDirX);
	//void calculate_movement_acceleration();
	auto agentAccelerationValue = _mm_mul_ps(_controlData[ControlAcceleration][i], amountAcceleration);
	auto accelerationX = _mm_mul_ps(agentDirX, agentAccelerationValue);
	auto accelerationY = _mm_mul_ps(agentDirY, agentAccelerationValue);
	//void verlet_integration_step(); // xNew = 2*x - xOld + a*dT^2
	// TODO: Add friction, Energy loss: 1.0 = no friction; 0.0 = infinite friction, no speed conservation between steps
	auto agentCurPosX = _agentPosX[i];
	auto agentCurPosY = _agentPosY[i];
	auto agentNewPosX = _mm_add_ps(_mm_sub_ps(_mm_mul_ps(_mm_set_ps1(2), agentCurPosX), _agentPPosX[i]), accelerationX);
	auto agentNewPosY = _mm_add_ps(_mm_sub_ps(_mm_mul_ps(_mm_set_ps1(2), agentCurPosY), _agentPPosY[i]), accelerationY);
	_agentPPosX[i] = _agentPosX[i];
	_agentPPosY[i] = _agentPosY[i];
	_agentPosX[i] = agentNewPosX;
	_agentPosY[i] = agentNewPosY;
}


void physics::substep_estimate_circular_path_advancement(size_t i) {
	_agentPathAdvancement[i] = _mm_add_ps(_agentPathAdvancement[i], FTA::atan2_ps(
		_mm_cross_ps(_agentPPosX[i], _agentPPosY[i], _agentPosX[i], _agentPosY[i]),
		_mm_dot_ps(_agentPPosX[i], _agentPPosY[i], _agentPosX[i], _agentPosY[i])
	));
}


void physics::substep_detect_collisions(size_t i, mmr agentRadiusSquared) {
	auto agentCollisionMask = _mm_setzero_ps();
	auto agentWallArrayOffset = i*_nWalls;
	for (size_t j = 0; j < _nWalls; ++j) {
		//void get_agent_wall_distances();
		auto wallToAgentX = _mm_sub_ps(_agentPosX[i], _wallS0X[j]);
		auto wallToAgentY = _mm_sub_ps(_agentPosY[i], _wallS0Y[j]);

		//void estimate_collisions(); 
		//to avoid expensive raycasting i assume agent is not moving fast
		//agentSpeed/dT < 2*agentRadius; otherwise tunelling occurs
		//also its not a circle, but a thin ellipse with zero width parallel the wall
		auto newCollisionMask = pu_circle_segment_collides(wallToAgentX, wallToAgentY, _wallS0S1X[j], _wallS0S1Y[j], agentRadiusSquared);
		agentCollisionMask = _mm_or_ps(agentCollisionMask, newCollisionMask);
	}
	_agentCollision[i] = _mm_or_ps(_agentCollision[i], agentCollisionMask);
}


void physics::substep_decrease_ttl(size_t i) {
	_agentTTL[i] = _mm_sub_epi32(_agentTTL[i], _mm_set1_epi32(1));
	auto agentDeathMask = _mm_cvtepi32_ps( _mm_cmpgt_epi32(_agentTTL[i], _mm_set1_epi32(0)) );
	_agentCollision[i] = _mm_or_ps(_agentCollision[i], agentDeathMask);
}


void physics::set_agent_angle(size_t i, float newAngle) {
	reinterpret_cast<float*>(_agentAngle)[i] = newAngle;
}


void physics::set_agent_position(size_t i, float newX, float newY) {
	reinterpret_cast<float*>(_agentPosX)[i] = newX;
	reinterpret_cast<float*>(_agentPosY)[i] = newY;
}


void physics::set_agent_collision_flags(size_t i, uint32_t newCollisionFlags) {
	reinterpret_cast<uint32_t*>(_agentCollision)[i] = newCollisionFlags;
}


void physics::set_agent_control(size_t i, size_t controlIndex, float controlValue) {
	reinterpret_cast<float*>(_controlData[controlIndex])[i] = controlValue;
}


int64_t physics::get_agent_score(size_t i) const {
	return static_cast< int64_t >(reinterpret_cast<const float *>(_agentPathAdvancement)[i] * 4096);
}


bool physics::get_agent_collision_flags(size_t i) const {
	return reinterpret_cast<const uint32_t*>(_agentCollision)[i] != 0;
}


const float & physics::get_agent_sensor_value(size_t i, size_t sensorIndex) const {
	return reinterpret_cast<const float*>(_agentSensorProximity + (i / 4)*_nSensors + sensorIndex)[i % 4];
}