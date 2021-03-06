#include "drawer_gl1.h"

#include "../vectorizedMathUtils/fasttrigo.h"

#include <iostream>
#include <sstream>
#include <string>

std::string i2s(const uint64_t i) {
	std::ostringstream s;
	s << i;
	return s.str();
}

drawer_gl1::drawer_gl1():
	enableRender(true),
	lastRenderedTick(0)
{}

void drawer_gl1::draw_frame(renderer& r) {
	// sync with render thread
	auto& sharedData(_renderSync.swap_buffers(render_sync::Reader));
	if (enableRender && (lastRenderedTick < sharedData.nTick)) {
		
		lastRenderedTick = sharedData.nTick;
		std::ostringstream newTitle;
		newTitle << "t=" << lastRenderedTick << "; best=" << sharedData.rcBest;
		r.set_title(newTitle.str().c_str());

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT); // | GL_DEPTH_BUFFER_BIT);

		// render walls
		glLineWidth(1.0);
		
		glBegin(GL_LINES);
		glColor3f(1.0, 0.5f, 0.0);
		glVertex2f(0.0f, 1.0f);
		
		glVertex2f(
			sharedData.carPosX[sharedData.rcBest / 4].m128_f32[sharedData.rcBest % 4],
			sharedData.carPosY[sharedData.rcBest / 4].m128_f32[sharedData.rcBest % 4]
		);
		glColor3f(1.0, 1.0, 0.0);
		for (auto&i : _localData.vWalls) {
			glVertex2fv(&i[0]);
			glVertex2fv(&i[2]);
			//glVertex2f(i[0][0], i[0][1]);
			//glVertex2f(i[1][0], i[1][1]);
		}
		glEnd();

		// render cars
		auto carR = _mm_set_ps1(_localData.carRadius); // sqrt(car._mPhysics.rSqr);
		auto mm2PIdiv3 = _mm_set_ps1(2.0943951023931954923084289221863f); // 2*pi / 3
		glBegin(GL_TRIANGLES);
		

		for (size_t i = 0; i < sharedData.carCount / 4; ++i) {
			//auto& car = *static_cast<simulation::car*>(i.get());

			MMR unitX, unitY;

			//auto pos0 = car._mPhysics.p + unit(car._mPhysics.ang) * carR;
			FTA::sincos_ps(sharedData.carAngle[i], &unitY, &unitX);
			auto pos0X = _mm_add_ps(sharedData.carPosX[i], _mm_mul_ps(unitX, carR));
			auto pos0Y = _mm_add_ps(sharedData.carPosY[i], _mm_mul_ps(unitY, carR));

			//auto pos2 = car._mPhysics.p + unit(car._mPhysics.ang - pi * 2 / 3) * carR;
			FTA::sincos_ps(_mm_sub_ps(sharedData.carAngle[i], mm2PIdiv3), &unitY, &unitX);
			auto pos2X = _mm_add_ps(sharedData.carPosX[i], _mm_mul_ps(unitX, carR));
			auto pos2Y = _mm_add_ps(sharedData.carPosY[i], _mm_mul_ps(unitY, carR));

			//auto pos1 = car._mPhysics.p + unit(car._mPhysics.ang + pi * 2 / 3) * carR;
			FTA::sincos_ps(_mm_add_ps(sharedData.carAngle[i], mm2PIdiv3), &unitY, &unitX);
			auto pos1X = _mm_add_ps(sharedData.carPosX[i], _mm_mul_ps(unitX, carR));
			auto pos1Y = _mm_add_ps(sharedData.carPosY[i], _mm_mul_ps(unitY, carR));

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


void drawer_gl1::local_data::register_wall(float x0, float y0, float x1, float y1) {
	vWalls.emplace_back(physics::wall_segment({ x0, y0, x1, y1 }));
}


drawer_gl1::shared_data::shared_data():
	nTick(0),
	carCount(0),
	carPosX(nullptr),
	carPosY(nullptr),
	carAngle(nullptr)
{}


drawer_gl1::shared_data::~shared_data() {
	release(); 
}


drawer_gl1::shared_data& drawer_gl1::shared_data::realloc_frame(size_t carCountUpdate) {
	if (carCount != carCountUpdate) {
		release();
		carCount = carCountUpdate;
		auto mmCount = mm_count(carCount);
		carPosX = aalloc(mmCount);
		carPosY = aalloc(mmCount);
		carAngle = aalloc(mmCount);
	}
	return *this;
}


void drawer_gl1::shared_data::release() {
	_mm_free( carPosX);
	_mm_free(carPosY);
	_mm_free(carAngle);
}
