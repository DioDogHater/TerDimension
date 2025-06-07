#include <stdio.h>
#include <math.h>

#define TD_TERMINAL

#include "terdimension.h"

#include <unistd.h>
#include <sys/time.h>

TD_Color test_shader(TD_ShaderInfo* si){
	return (TD_Color){
		si->bc.x * 255,
		si->bc.y * 255,
		si->bc.z * 255
	};
}

TD_Mesh test_mesh = (TD_Mesh){
	(TD_Vec3[]){
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{0.f,0.5f,0.f}
	},
	(TD_Face[]){
		{0,1,2}
	},
	TD_TransformIDENTITY
};

int main(void){
	if(!TD_init(150,150))
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

		TD_raster_face(
			&test_mesh,
			&test_mesh.faces[0],
			test_shader
		);

		// Display changes + Clear buffers
		TD_update_screen();

		// Rotate triangle
		test_mesh.transform.rotation.z += 0.1f;

		// Scale it
		test_mesh.transform.scale = TD_Vec3_scale(TD_Vec3IDENTITY,cos(time)*0.5f+1.f);

		// Translate it
		test_mesh.transform.position.x = sin(time)*0.25f;

		// Advance time
		time += 0.05f;

		// Slow down rendering to avoid jittering
		usleep(5000);
	}

	TD_quit();
}