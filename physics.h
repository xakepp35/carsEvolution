#pragma once

#include "base.h"

class physics
{
public:

	physics(size_t nAgents);
	~physics();

	void init_walls(size_t nWalls);
	void init_sensors(size_t nSensors);

	// proximity sensor
	void step_proximity_sensor(size_t endAgent, size_t startAgent = 0);
	void substep_find_max_inverse_distance_to_wall(size_t i);
	
	// motion integrator
	void step_motion_integrator(size_t endAgent, size_t startAgent = 0);
	void substep_integrate_rotation(size_t i, mmr amountSteering);
	void substep_integrate_movement(size_t i, mmr amountAcceleration);
	void substep_estimate_circular_path_advancement(size_t i);
	
	// collision detector
	void step_collision_detector(size_t endAgent, size_t startAgent = 0);
	void substep_detect_collisions(size_t i, mmr agentRadiusSquared);
	void substep_decrease_ttl(size_t i);

	// shuffles for control logic:
	void set_agent_control(size_t i, size_t controlIndex, float controlValue);
	const float& get_agent_score(size_t i) const;
	bool get_agent_collision_flags(size_t i) const;
	const float& get_agent_sensor_value(size_t i, size_t sensorIndex) const;

	// for automatic mapping
	enum control: size_t {
		ControlSteering = 0, // rudder value [-1.0..1.0]
		ControlAcceleration, // gas/break pedal value [-1.0..1.0]
		ControlCOUNT
	};

	MMR* _controlData[ControlCOUNT];

	// current turn angles
	MMR* _agentAngle;

	// heading direction unit vector
	//MMR* _agentDirX;
	//MMR* _agentDirY;

	// position
	MMR* _agentPosX;
	MMR* _agentPosY;

	// previous step position
	MMR* _agentPPosX;
	MMR* _agentPPosY;

	// path advancement per step, in radians
	MMR* _agentPathAdvancement; 

	// defines walls as a segments s0->s1, for each wall
	MMR* _wallS0X;
	MMR* _wallS0Y;

	// first to second point vector, for each wall
	MMR* _wallS0S1X;
	MMR* _wallS0S1Y;

	// collision masks, for each agent
	MMR* _agentCollision;

	// ticks to live, on zero will set collision flag
	__m128i* _agentTTL;

	// sensor angles, relative to agent origin; for each sensor
	MMR* _sensorAngle;

	// for each agent*each sensor: it is an input to neural network
	MMR* _agentSensorProximity;

	//size_t _nAgents; // number of agents
	size_t _nWalls; // number of walls (segments) on map
	size_t _nSensors; // number of sensors, each agent has

	float dT; // fixed time delta for physics simulation
	float dT2; // squared time delta;
	float agentRadius; // = 1.0f / 64
	float steeringMagnitude; // = 64;
	float accelerationMagnitude; // = 64;

};
