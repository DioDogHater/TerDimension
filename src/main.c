#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Render in the terminal
#define TD_TERMINAL

// Uncomment this following line if you want to render without color
// Also speeds up the program, so fast that input needs to be adjusted
// I recommend quadrupling the speed of all input interactions
//#define TD_NO_COLOR

// Comment out this line to disable RGB (if color doesn't work)
#define TD_COLOR_RGB

// To remove unicode characters (top half block characater)
// uncomment this following line
// (BTW this will also speed up the program)
//#define TD_NO_UNICODE

// To remove input detection, uncomment this following line
//#define TD_DISABLE_INPUT

#include "terdimension.h"
#include "td_time.h"

// Fragment shader
TD_Color test_shader(TD_ShaderInfo* si){
	return si->color;
}

// Hard coded cube mesh
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
	(TD_Transform){
		(TD_Vec3){0.f,0.f,5.f},
		TD_Vec3ZERO,
		TD_Vec3IDENTITY
	}
};

int main(void){
	// Change the resolution
	// 200x200 and beyond is too laggy
	if(!TD_init(150,100))
		return 1;

	// Time variables
	TD_time_t last_frame = TD_get_ticks();
	float deltaTime = 0.f, FPS = 1.f;

	TD_clear_screen();

	while(1){
		// Render the cube on screen
		TD_render_mesh(&cube_mesh,test_shader);

		// Display changes + Clear buffers
		TD_update_screen();

		// Rotate cube
		cube_mesh.transform.rotation.y += 3.5f*deltaTime;
		cube_mesh.transform.rotation.x += 4.f*deltaTime;

		// Handle input
		// If you wish to disable input, please omit this part of the code
		#define PLAYER_SPEED 4.f
		#define PLAYER_LOOK 2.f
		char c;
		TD_Vec3 movement_vector = TD_Vec3ZERO;
		while(TD_get_input(&c)){
			switch(c){
			case TD_CTRL_C:
				exit(0);
			case 'w':
				movement_vector.z = PLAYER_SPEED*deltaTime;
				break;
			case 's':
				movement_vector.z -= PLAYER_SPEED*deltaTime;
				break;
			case 'a':
				movement_vector.x -= PLAYER_SPEED*deltaTime;
				break;
			case 'd':
				movement_vector.x += PLAYER_SPEED*deltaTime;
				break;
			case 'e':
				movement_vector.y += PLAYER_SPEED*deltaTime;
				break;
			case 'q':
				movement_vector.y -= PLAYER_SPEED*deltaTime;
				break;
			case 'i':
				TD_camera.rotation.x += PLAYER_LOOK*deltaTime;
				break;
			case 'k':
				TD_camera.rotation.x -= PLAYER_LOOK*deltaTime;
				break;
			case 'j':
				TD_camera.rotation.y -= PLAYER_LOOK*deltaTime;
				break;
			case 'l':
				TD_camera.rotation.y += PLAYER_LOOK*deltaTime;
				break;
			}
		}
		
		TD_Vec3 inverse_camera_rotation = TD_Vec3_scale(TD_camera.rotation,-1.f);
		movement_vector = TD_Vec3_rotationZYX(&inverse_camera_rotation,&movement_vector);
		TD_camera.position = TD_Vec3_add(TD_camera.position,movement_vector);

		// Update the FPS counter and deltaTime
		deltaTime = TD_get_deltaTime(&last_frame);
		FPS = TD_GET_FPS(deltaTime);
		printf("FPS: %.f   \n",FPS);
		TD_Vec3_print("pos: ",TD_camera.position);
		TD_Vec3_print("rot: ",TD_Vec3_scale(TD_camera.rotation,180/3.1415f));
	}

	TD_quit();
}
