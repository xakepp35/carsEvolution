#include "sheduler.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


sheduler::sheduler():
	_nStep(0)
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_stampFreq));
}


void sheduler::set_realtime_thread_priority() {
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}


void sheduler::step() {
	//curStamp = getstamp();
	// stampDelta = curstamp - prevStamp;
	//_prevStamp = curstamp;
	// _deltaLast = maptime
	// _deltaAvg = averaging...
	_nStep++;
	// curStamp < next stamp?
	// then sleepex()...
}


void ss() {
	DWORD timeAdjustment = 0;
	DWORD clockInterval100Ns = 0;
	BOOL timeAdjustmentDisabled = 0;

	// Get the frequency of the clock interrupt: clockInterval100Ns
	GetSystemTimeAdjustment( &timeAdjustment, &clockInterval100Ns, &timeAdjustmentDisabled);
}


sheduler::stamp sheduler::now() {
	stamp hpC = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&hpC));
	return hpC;
}


sheduler::stamp sheduler::freq() const {
	return _stampFreq;
}


sheduler::time sheduler::map(const stamp& stampDelta) const {
	return static_cast<time>(stampDelta) / freq();
}


sheduler::stamp sheduler::unmap(const time& timeDelta) const {
	return static_cast<stamp>(timeDelta * freq());
}
