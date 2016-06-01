#include "clock.h"

#include <time.h>
#include <unistd.h>
#include <stdbool.h>

bool stubClock = false;
long long stubClockTime = 0;

long long getClock()
{
	if( stubClock ) {
		return stubClockTime;
	}
	long long ms;
	struct timespec spec;
	
	//TODO use CLOCK_REALTIME_RAW
	clock_gettime(CLOCK_MONOTONIC_RAW, &spec);
	
	ms = spec.tv_sec * 1000 + spec.tv_nsec / 1.0e6;
	return ms;
}

void setClock(long long value)
{
	stubClock = true;
	stubClockTime = value;
}

void addClock(long long value)
{
	if( !stubClock ) {
		stubClockTime = getClock();
		stubClock = true;
	}
	stubClockTime += value;
}

void resetClock()
{
	stubClock = false;
	stubClockTime = 0;
}
