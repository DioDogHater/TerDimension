#ifndef TERDIMENSION_TIME_H
#define TERDIMENSION_TIME_H

#include "td_definitions.h"
#include <sys/time.h>

// Type for ticks
typedef unsigned long long TD_time_t;

#define TD_TO_MILLISECONDS(t) ((t)/1000)
#define TD_GET_FPS(dt) (1.f/(dt))

TD_FUNC TD_time_t TD_get_ticks();
TD_FUNC float TD_get_deltaTime(TD_time_t*);

#endif
