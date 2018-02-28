#pragma once
#include <cstdint>

class sheduler {
public:

	typedef uint64_t	stamp;
	typedef	double		time;

	sheduler();
	void set_realtime_thread_priority();
	void step();


	stamp now() const;
	stamp freq() const;
	time map(const stamp& stampDelta) const;
	stamp unmap(const time& timeDelta) const;

	size_t	_nStep;

protected:

	
	stamp	_prevStamp;
	stamp	_stampFreq;
	time	_deltaLast;
	time	_deltaAverage;

};
