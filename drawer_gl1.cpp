#include "drawer_gl1.h"

#include "../vectorizedMathUtils/fasttrigo.h"

#include <iostream>

drawer_gl1::drawer_gl1()
{}

void drawer_gl1::draw_frame(renderer& r) {
	// sync with render thread
	auto& dataDynamic(_renderSync[render_sync::Reader]);
	enableRender = true;
	if (enableRender) {

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT); // | GL_DEPTH_BUFFER_BIT);

		// render walls
		glLineWidth(1.0);
		glColor3f(1.0, 1.0, 0.0);
		glBegin(GL_LINES);
		for (auto&i : _dataStatic.vWalls) {
			glVertex2fv(&i[0]);
			glVertex2fv(&i[2]);
			//glVertex2f(i[0][0], i[0][1]);
			//glVertex2f(i[1][0], i[1][1]);
		}
		glEnd();

		// render cars
		auto carR = _mm_set_ps1(_dataStatic.carRadius); // sqrt(car._mPhysics.rSqr);
		auto mm2PIdiv3 = _mm_set_ps1(2.0943951023931954923084289221863f); // 2*pi / 3
		glBegin(GL_TRIANGLES);
		

		for (size_t i = 0; i < dataDynamic.carCount / 4; ++i) {
			//auto& car = *static_cast<simulation::car*>(i.get());

			MMR unitX, unitY;

			//auto pos0 = car._mPhysics.p + unit(car._mPhysics.ang) * carR;
			FTA::sincos_ps(dataDynamic.carAngle[i], &unitY, &unitX);
			auto pos0X = _mm_add_ps(dataDynamic.carPosX[i], _mm_mul_ps(unitX, carR));
			auto pos0Y = _mm_add_ps(dataDynamic.carPosY[i], _mm_mul_ps(unitY, carR));

			//auto pos2 = car._mPhysics.p + unit(car._mPhysics.ang - pi * 2 / 3) * carR;
			FTA::sincos_ps(_mm_sub_ps(dataDynamic.carAngle[i], mm2PIdiv3), &unitY, &unitX);
			auto pos2X = _mm_add_ps(dataDynamic.carPosX[i], _mm_mul_ps(unitX, carR));
			auto pos2Y = _mm_add_ps(dataDynamic.carPosY[i], _mm_mul_ps(unitY, carR));

			//auto pos1 = car._mPhysics.p + unit(car._mPhysics.ang + pi * 2 / 3) * carR;
			FTA::sincos_ps(_mm_add_ps(dataDynamic.carAngle[i], mm2PIdiv3), &unitY, &unitX);
			auto pos1X = _mm_add_ps(dataDynamic.carPosX[i], _mm_mul_ps(unitX, carR));
			auto pos1Y = _mm_add_ps(dataDynamic.carPosY[i], _mm_mul_ps(unitY, carR));

			// shuffle/interleave for graphics
			for (size_t j = 0; j < 4; ++j) {
				glColor3f(0.0, 1.0, 0.0); //if (!car._mPhysics._isActive) glColor3f(1.0, 0.0, 0.0);
				glVertex2f(pos0X.m128_f32[j], pos0Y.m128_f32[j]);
				glColor3f(0.0, 0.5, 1.0);
				glVertex2f(pos1X.m128_f32[j], pos1Y.m128_f32[j]);
				glVertex2f(pos2X.m128_f32[j], pos2Y.m128_f32[j]);
			}
		}
		glEnd();

		// render view fields
		/*if (enableFOVrender) {
			glBegin(GL_LINES);
			glColor3f(1.0, 1.0, 1.0);
			for (auto&i : e._vAgent) {
				auto& car = *static_cast<simulation::car*>(i.get());
				auto& mem = car._mNeural._vMemory[1 - car._mNeural._tapeIndex];
				auto pos0 = car._mPhysics.p;
				size_t sensorWidth = car._mNeural.input_size();
				float startAngle = car._mPhysics.ang + piDiv2 * (((scalar)1) - ((scalar)sensorWidth)) / sensorWidth;
				for (size_t j = 0; j < sensorWidth; j++) {
					auto dist = ((scalar*)mem.data())[car._mNeural._vcell.size() + j];
					if (dist > 0) {
						//auto t = sqrt(1 / dist - 1);
						auto t = 1 / dist - 1;
						auto currentAngle = startAngle + (pi * j) / sensorWidth;
						auto unitDir = unit(currentAngle);
						auto pos1 = car._mPhysics.p + unitDir * t;
						glVertex2f(pos0[0], pos0[1]);
						glVertex2f(pos1[0], pos1[1]);
					}
				}
			}
			glEnd();
		}*/
	}
}

void drawer_gl1::handle_input(renderer& r) {
	if (r.get_key(GLFW_KEY_R)) {
		enableRender = !enableRender;
		std::cout << "\t* enableRender = " << (int)(enableRender) << "\n";
	}
	/*
	if (r.get_key(GLFW_KEY_V)) {
		enableVSync = 1 - enableVSync;
		glfwSwapInterval(enableVSync);
		std::cout << "\t* vSync = " << enableVSync << "\n";
	}
	if (r.get_key(GLFW_KEY_F)) {
		enableFOVrender = !enableFOVrender;
		std::cout << "\t* fovRender = " << (int)(enableFOVrender) << "\n";
	}
	if (r.get_key(GLFW_KEY_KP_ADD)) {
		e._generationStepMax *= 2;
		std::cout << "\t* _generationSteps = " << e._generationStepMax << "\n";
	}
	if (r.get_key(GLFW_KEY_KP_SUBTRACT)) {
		e._generationStepMax /= 2;
		std::cout << "\t* _generationSteps = " << e._generationStepMax << "\n";
	}
	if (r.get_key(GLFW_KEY_KP_MULTIPLY)) {
		e._weightDelta *= 2;
		std::cout << "\t* _weightDelta = " << e._weightDelta << "\n";
	}
	if (r.get_key(GLFW_KEY_KP_DIVIDE)) {
		e._weightDelta /= 2;
		std::cout << "\t* _weightDelta = " << e._weightDelta << "\n";
	}
	*/
}


void drawer_gl1::data_static::register_wall(float x0, float y0, float x1, float y1) {
	vWalls.emplace_back(wall_segment({ x0, y0,x1,y1 }));
}


drawer_gl1::data_dynamic::data_dynamic():
	nTick(0),
	carCount(0),
	carPosX(nullptr),
	carPosY(nullptr),
	carAngle(nullptr)
{}


drawer_gl1::data_dynamic::~data_dynamic() {
	release(); 
}


void drawer_gl1::data_dynamic::stream_data(uint64_t newTick, size_t newCount, mmr* newPosX, mmr* newPosY, mmr* newAngle) {
	nTick = newTick;
	alloc(newCount);
	memcpy(carPosX, newPosX, newCount/4 * sizeof(mmr));
	memcpy(carPosY, newPosY, newCount / 4 * sizeof(mmr));
	memcpy(carAngle, newAngle, newCount / 4 * sizeof(mmr));
}


void drawer_gl1::data_dynamic::alloc(size_t newCount) {
	if (newCount != carCount) {
		release();
		carCount = newCount;
		auto mmCount = mm_count(newCount);
		carPosX = aalloc(mmCount);
		carPosY = aalloc(mmCount);
		carAngle = aalloc(mmCount);
	}
}


void drawer_gl1::data_dynamic::release() {
	_mm_free( carPosX);
	_mm_free(carPosY);
	_mm_free(carAngle);
}
