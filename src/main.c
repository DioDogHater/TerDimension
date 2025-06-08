#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TD_TERMINAL
#define TD_COLOR_RGB

#include "terdimension.h"

#include <unistd.h>
#include <sys/time.h>

TD_Color test_shader(TD_ShaderInfo* si){
	return (TD_Color){si->bc.x*255,si->bc.y*255,si->bc.z*255};
}

TD_Mesh cube_mesh = (TD_Mesh){
	(TD_Vec3[]){
		{-0.5f,-0.5f,-0.5f},
		{-0.5f,0.5f,-0.5f},
		{0.5f,-0.5f,-0.5f},
		{0.5f,0.5f,-0.5f},
		{0.5f,-0.5f,0.5f},
		{0.5f,0.5f,0.5f},
		{-0.5f,-0.5f,0.5f},
		{-0.5f,0.5f,0.5f}
	},
	NULL,
	(TD_Face[]){
		// Front
		{2,1,0},
		{2,3,1},
		// Back
		{6,5,4},
		{6,7,5},
		// Left
		{0,1,6},
		{7,6,1},
		// Right
		{4,3,2},
		{5,3,4},
		// Top
		{3,7,1},
		{3,5,7},
		// Bottom
		{0,6,2},
		{6,4,2}
	},
	12,
	TD_TransformIDENTITY
};

int main(void){
	if(!TD_init(100,100))
		return 1;

	// Time variables
	unsigned long long now, last_frame=0;
	unsigned long long deltaTime = 0;
	float FPS = 1.f;

	// Used for sine waves
	float time = 0.f;

	TD_clear_screen();

	while(1){
		// Update the FPS counter and deltaTime
		struct timeval tv;
		gettimeofday(&tv, NULL);
		now = (unsigned long long)(tv.tv_sec) * 1000 +(unsigned long long)(tv.tv_usec)/1000;
		deltaTime = now - last_frame;
		FPS = 1000.f/(float)deltaTime;
		printf("%.f\n",FPS);
		last_frame = now;

		TD_render_mesh(&cube_mesh,test_shader);

		// Display changes + Clear buffers
		TD_update_screen();

		// Rotate cube
		cube_mesh.transform.rotation.y += 0.05f;
		cube_mesh.transform.rotation.x += 0.1f;

		// Translate it
		cube_mesh.transform.position.z = sin(time)*2.5f+5.f;
		cube_mesh.transform.position.x = cos(time);

		// Advance time
		time += 0.05f;

		// Slow down rendering to avoid jittering
		usleep(5000);
	}

	TD_quit();
}