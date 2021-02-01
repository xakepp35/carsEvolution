#pragma once

#include "renderer.h"
#include "buffer3.h"

#include "sheduler.h"
#include "physics.h"
#include "neural.h"

#include <vector>

class drawer_gl1:
	public renderer::i_drawer,
	public renderer::i_handler
{
public:

	// data that would not change in the logic thread
	class local_data
	{
	public:

		// wall is a segment, (x0,y0)-(x1,y1); data is stored contigously in that layout
		std::vector< physics::wall_segment > vWalls;
		float carRadius;

		// add wall segment for static rendering
		void register_wall(float x0, float y0, float x1, float y1);

	};

	// dynamic_data that would change in the logic thread and shared with renderer
	class shared_data
	{
	public:

		uint64_t nTick;
		size_t rcBest;

		size_t carCount;

		MMR* carPosX;
		MMR* carPosY;
		MMR* carAngle;

		shared_data();
		~shared_data();

		//void stream_data(const sheduler& tickSheduler, size_t agentCount, const physics& physicsEngine);
		shared_data& realloc_frame(size_t carCountUpdate);

	protected:

		
		void release();
		
	};

	drawer_gl1();

	// Renderer code
	virtual void draw_frame(renderer& r) override;

	// "Immediate mode" keyboard & window events handling
	virtual void handle_input(renderer& r) override;

	typedef buffer3< shared_data > render_sync;

	local_data _localData;
	render_sync _renderSync;

protected:
	
	bool enableRender;
	bool enableFOVrender;
	uint64_t lastRenderedTick;

};