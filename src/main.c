#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Render in the terminal
#define TD_TERMINAL

// Comment out this line to disable RGB (if color doesn't work)
#define TD_COLOR_RGB

#include "terdimension.h"

// To calculate FPS
#include <sys/time.h>

TD_Color test_shader(TD_ShaderInfo* si){
	return si->color;
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
	(TD_Color[]){
		TD_RED,
		TD_BLUE,
		TD_GREEN,
		TD_WHITE,
		TD_BLUE,
		TD_RED,
		TD_WHITE,
		TD_GREEN,
	},
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
	// Change the resolution
	// 200x200 and beyond is too laggy
	if(!TD_init(150,100))
		return 1;

	// Time variables
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long now = 0, last_frame = (unsigned long long)(tv.tv_sec) * 1000000 + (unsigned long long)(tv.tv_usec);
	float deltaTime = 0.f, FPS = 1.f;

	// Used for sine waves
	float time = 0.f;

	TD_clear_screen();

	while(1){
		// Render the cube on screen
		TD_render_mesh(&cube_mesh,test_shader);

		// Display changes + Clear buffers
		TD_update_screen();

		// Rotate cube
		cube_mesh.transform.rotation.y += 3.5f*deltaTime;
		cube_mesh.transform.rotation.x += 4.f*deltaTime;

		// Translate it
		cube_mesh.transform.position.z = sin(time)*2.5f+5.f;
		cube_mesh.transform.position.x = cos(time);

		// Advance time
		time += (float)deltaTime;

		// Update the FPS counter and deltaTime
		gettimeofday(&tv, NULL);
		now = (unsigned long long)(tv.tv_sec) * 1000000 +(unsigned long long)(tv.tv_usec);
		deltaTime = (now-last_frame)/1000000.f;
		FPS = 1.f/(float)deltaTime;
		printf("%.f  \n",FPS);
		last_frame = now;
	}

	TD_quit();
}
