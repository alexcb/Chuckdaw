#include "clock.h"

#include <time.h>
#include <unistd.h>

long long getClock()
{
	long long ms;
	struct timespec spec;
	
	//TODO use CLOCK_REALTIME_RAW
	clock_gettime(CLOCK_MONOTONIC_RAW, &spec);
	
	ms = spec.tv_sec * 1000 + spec.tv_nsec / 1.0e6;
	return ms;
}

