#include "sheduler.h"
#include "physics.h"

#include <iostream>

void benchmark() {
	sheduler s;
	s.set_realtime_thread_priority();

	physics p(1024);
	do {
		auto prevStamp = s.now();
		do {
			p.step_motion_integrator(1024);
			s.step();
		} while ((s.now() - prevStamp) < s.freq());
		auto cpuFreq = 4.0E9; // 4.0 GHz CPU
		std::cout << s._nStep << " it/s\t" << cpuFreq / s._nStep << " cycles/it\n";
		s._nStep = 0;
	} while (true);
}

int main() {
	std::cout <<
		"Natural Selection of Neural Network(Perceptrone) weights demo\n"
		"\n"
		"Date : Feb 2018\n"
		"Author : xakepp35@gmail.com\n"
		"License : FreeBSD(ISC) for non - commercial(personal, educational) use.\n"
		"\n"
		;

	sheduler s;
	s.set_realtime_thread_priority();

	physics p(1024);

	

	do {
		auto prevStamp = s.now();
		do {
			p.step_motion_integrator(1024);
			s.step();
		}
		while ((s.now() - prevStamp) < s.freq() );
		auto cpuFreq = 4.0E9; // 4.0 GHz CPU
		std::cout << s._nStep << " it/s\t" << cpuFreq / s._nStep << " cycles/it\n";
		s._nStep = 0;
	} while (true);

}

