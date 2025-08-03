#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// Other options for TerDimension
// To enable them, define them before "TD/terdimension.h"
// TD_NO_TEXTURES - no textures, no stb_image.h
// TD_DISABLE_INPUT - no input, only detect if CTRL+C or CTRL+Z are pressed

#include "TD/td_definitions.h"
#include "TD/terdimension.h"
#include "TD/td_time.h"

float time = 0.f;

#include "example_meshes.h"
#include "example_raymarching.h"

int main(void){
	// Change the resolution
	// 200x200 and beyond is slow
	if(!TD_init(200,120))
		return 1;

	// Time variables
	TD_time_t last_frame = TD_get_ticks();
	float deltaTime = 0.f, FPS = 1.f;	

	if(!TD_load_texture("assets/lebron_img.jpg",&lebron_texture))
		exit(0);

	// Hide the text before
	TD_clear_screen();

	// Set rendering to default (change if you want something else)
	TD_set_render_flags(TD_RENDER_DEFAULT);

	while(1){
		// Update the FPS counter and deltaTime
		deltaTime = TD_get_deltaTime(&last_frame);
		FPS = TD_GET_FPS(deltaTime);
		deltaTime = TD_CLAMP(deltaTime, 0.f, 1.f);

		// Advance time
		time += deltaTime;

		TD_clear_buffers();

		// Render the cube on screen
		TD_use_shader(diffuse_lighting_shader);
		TD_render_mesh(&multicolor_cube);

		// Render the textured plane on screen
		TD_use_shader(texture_shader);
		TD_render_mesh(&textured_plane);

		//TD_use_shader(raymarch_shader);
		//TD_render_mesh(&raymarch_plane);

		// Display changes
		TD_update_screen();

		// Rotate cube
		multicolor_cube.transform.rotation.y += 3.5f*deltaTime;
		multicolor_cube.transform.rotation.x += 4.f*deltaTime;

		// Handle input
		// If you wish to disable input, please omit this part of the code
		#define PLAYER_SPEED 0.25f
		#define PLAYER_LOOK 0.05f
		char c;
		TD_Vec3 movement_vector = TD_Vec3ZERO;
		while(TD_get_input(&c)){
			switch(c){
			case TD_CTRL_C:
				TD_free_texture(&lebron_texture);
				exit(0);
			case 'w':
				movement_vector.z = PLAYER_SPEED;
				break;
			case 's':
				movement_vector.z = -PLAYER_SPEED;
				break;
			case 'a':
				movement_vector.x = -PLAYER_SPEED;
				break;
			case 'd':
				movement_vector.x = PLAYER_SPEED;
				break;
			case ' ':
				movement_vector.y = PLAYER_SPEED;
				break;
			case 'c':
				movement_vector.y = -PLAYER_SPEED;
				break;
			case 'i':
				TD_camera.rotation.x += PLAYER_LOOK;
				break;
			case 'k':
				TD_camera.rotation.x -= PLAYER_LOOK;
				break;
			case 'j':
				TD_camera.rotation.y -= PLAYER_LOOK;
				break;
			case 'l':
				TD_camera.rotation.y += PLAYER_LOOK;
				break;
			case 't':
				TD_render_flags = ((TD_render_flags & TD_RENDER_RGB) ? TD_RENDER_COLOR | TD_RENDER_BRIGHT_COLORS : TD_RENDER_RGB) | (TD_render_flags & TD_RENDER_UNICODE);
				break;
			case 'y':
				TD_render_flags ^= TD_RENDER_UNICODE;
				break;
			case 'g':
				TD_render_flags = (TD_render_flags & (TD_RENDER_RGB | TD_RENDER_COLOR)) ? TD_RENDER_NO_COLOR : TD_RENDER_DEFAULT;
				break;
			case 'r':
				TD_camera = TD_CameraDEFAULT;
				time = 0.f;
				break;
			}
		}
		
		// Applies movement
		TD_Vec3 inverse_camera_rotation = (TD_Vec3){0.f,-TD_camera.rotation.y,0.f};
		movement_vector = TD_Vec3_rotationZYX(&inverse_camera_rotation,&movement_vector);
		TD_camera.position = TD_Vec3_add(TD_camera.position,movement_vector);

		// Print information
		printf("FPS: %.2f    Time: %.2f     \n",FPS,time);
		printf("Controls: WASD to move, SPACE and C to go up and down, IJKL to look around. R to reset camera."
				" T and Y to toggle rendering modes. G to toggle colors. CTRL+C to quit.\n");
	}

	TD_quit();
}
