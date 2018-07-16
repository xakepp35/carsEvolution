#include "sheduler.h"
#include "physics.h"

#include <iostream>

// cpuFreq = 4.2 GHz CPU
void benchmark_physics(double cpuFreq = 4.2E9) {
	std::cout <<
		"Car racing physics engine benchmark\n"
		"\n"
		"Date : Feb 2018\n"
		"Author : xakepp35@gmail.com\n"
		"License : FreeBSD(ISC) for non - commercial(personal, educational) use.\n"
		"\n"
		;

	sheduler s;
	s.set_realtime_thread_priority();

	physics p(1024, 4, 4);

	competition::score_chart sc(1024);

	do {
		auto prevStamp = s.now();
		do {
			p.update_situation(sc,1024);
			s.step();
		}
		while ((s.now() - prevStamp) < s.freq() );
		std::cout << s._nStep << " it/s\t(" << cpuFreq / s._nStep << " CPU cycles/it)\n";
		s._nStep = 0;
	} while (true);
}


int main() {
	benchmark_physics();
	return 0;
}
