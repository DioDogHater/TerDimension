#ifndef TERDIMENSION_TIME_H
#define TERDIMENSION_TIME_H

#include "terdimension.h"

#include <sys/time.h>

// Type for ticks
typedef unsigned long long TD_time_t;

// Used to get current microseconds
static struct timeval TD_timeval;

#define TD_TO_MILLISECONDS(t) ((t)/1000)
#define TD_GET_FPS(dt) (1.f/(dt))

// Returns ticks (microseconds)
TD_FUNC TD_time_t TD_get_ticks(){
	gettimeofday(&TD_timeval,NULL);
	return (unsigned long long)(TD_timeval.tv_sec) * 1000000 + (unsigned long long)(TD_timeval.tv_usec);
}

// Get the deltaTime (in seconds)
TD_FUNC float TD_get_deltaTime(TD_time_t* last_tick){
	TD_time_t now = TD_get_ticks();
	float r = (float)(now-(*last_tick))/1000000.f;
	*last_tick = now;
	return r;
}

#endif