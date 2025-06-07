#include <stdio.h>
#include <math.h>

#define TD_TERMINAL

#include "terdimension.h"

#include <unistd.h>
#include <sys/time.h>

float time = 0.f;

TD_Color test_shader(TD_ShaderInfo* si){
	return (TD_Color){(sin(time)*0.5f+0.5f)*si->bc.x*255,(cos(time)*0.5f+0.5f)*si->bc.y*255,(sin(-time)*0.5f+0.5f)*si->bc.z*255};
}

int main(void){
	if(!TD_init(100,100))
		return 1;

	unsigned long long now, last_frame=0;

	while(1){
		TD_clear_screen();

		TD_raster_triangle(
			(TD_Vec3){-0.5f,-0.5f,0.f},
			(TD_Vec3){0.5f,-0.5f,0.f},
			(TD_Vec3){0.f,0.5f,0.f},
			test_shader
		);

		struct timeval tv;
		gettimeofday(&tv, NULL);
		now = (unsigned long long)(tv.tv_sec) * 1000000 +(unsigned long long)(tv.tv_usec);
		printf("%g\n",1000000.f/(float)(now-last_frame));
		last_frame = now;

		TD_update_screen();
		time += 0.01f;

		usleep(5000);
	}

	TD_quit();
}