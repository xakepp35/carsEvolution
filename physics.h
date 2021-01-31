#pragma once

#include <array>

#include "base.h"
#include "competition.h"

class physics: public competition::i_problem
{
public:

	// wall is a segment, (x0,y0)-(x1,y1); data is stored contigously in that layout
	typedef std::array< float, 4 > wall_segment;

	physics(size_t nAgents, size_t nWalls, size_t nSensors);
	~physics();

	void populate_walls(const std::vector< physics::wall_segment >& vWalls);

	// i_problem interface
	virtual void describe_situation(size_t numAgents) override;
	virtual void update_situation(competition::score_chart& accumulatedScore, size_t numAgents) override;
	virtual void new_performer(size_t misbehavingIndex, const competition::ranking_chart& rankingChart, bool bestExists) override;


	// proximity sensor
	void substep_find_max_inverse_distance_to_wall(size_t i);
	
	// motion integrator
	void substep_integrate_rotation(size_t i, mmr amountSteering);
	void substep_integrate_movement(size_t i, mmr amountAcceleration);
	void substep_estimate_circular_path_advancement(size_t i);
	
	// collision detector
	void substep_detect_collisions(size_t i, mmr agentRadiusSquared);
	void substep_decrease_ttl(size_t i);

	// shuffles for glue logic: setters
	void set_agent_control(size_t i, size_t controlIndex, float controlValue);
	void set_agent_angle(size_t i, float newAngle);
	void set_agent_position(size_t i, float newX, float newY);
	void set_agent_collision_flags(size_t i, uint32_t newCollisionFlags = 0);
	void set_agent_ttl(size_t i, uint32_t newTTL);
	void set_agent_score(size_t i, int64_t newScore);

	// shuffles for glue logic/renderer: getters
	float get_agent_angle(size_t i) const;
	float get_agent_position_x(size_t i) const;
	float get_agent_position_y(size_t i) const;
	bool get_agent_collision_flags(size_t i) const;	
	const float& get_agent_sensor_value(size_t i, size_t sensorIndex) const;
	int64_t get_agent_score(size_t i) const;

	// for automatic mapping
	enum control: size_t {
		ControlSteering = 0, // rudder value [-1.0..1.0]
		ControlAcceleration, // gas/break pedal value [-1.0..1.0]
		ControlCOUNT
	};

	struct sim_config {
		float dT; // fixed time delta for physics simulation
		float dT2; // squared time delta;
		float agentRadius; // = 1.0f / 64
		float steeringMagnitude; // = 64;
		float accelerationMagnitude; // = 64;
	};

	struct spawn_config {
		uint32_t agentTTL; // max time to live in ticks

		float agentRespawnAngle; //  0.0f
		float agentRespawnPosX; // 0.0f
		float agentRespawnPosY; // 0.8f
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

	sim_config		_simConfig;
	spawn_config	_spawnConfig;
};
