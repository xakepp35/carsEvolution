#pragma once

#include "renderer.h"
#include "buffer3.h"

#include <vector>

class drawer_gl1:
	public renderer::i_drawer,
	public renderer::i_handler
{
public:

	// data that would not change in the logic thread
	class data_static
	{
	public:

		typedef std::array< float, 4 > wall_segment;
		// wall is a segment, (x0,y0)-(x1,y1); data is stored contigously in that layout
		std::vector< wall_segment > vWalls;
		float carRadius;

		// add wall segment for static rendering
		void register_wall(float x0, float y0, float x1, float y1);

	};

	// dynamic_data
	class data_dynamic
	{
	public:

		uint64_t nTick;

		size_t carCount;

		MMR* carPosX;
		MMR* carPosY;
		MMR* carAngle;

		data_dynamic();
		~data_dynamic();

		void stream_data(uint64_t newTick, size_t newCount, mmr* newPosX, mmr* newPosY, mmr* newAngle);

	protected:

		void alloc(size_t newCount);
		void release();
		
	};

	drawer_gl1();

	// Renderer code
	virtual void draw_frame(renderer& r) override;

	// "Immediate mode" keyboard & window events handling
	virtual void handle_input(renderer& r) override;

	typedef buffer3< data_dynamic > render_sync;

	data_static _dataStatic;
	render_sync _renderSync;

protected:
	
	bool enableRender;
	bool enableFOVrender;

};