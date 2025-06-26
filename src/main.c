#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// OTHER OPTIONS:

// TD_SDL (not implemented yet) - renders in an SDL window

// TD_NO_UNICODE - disables unicode characters (faster)

// TD_DISABLE_BRIGHT_COLORS - disables bright colors in 4bit color mode
// (when TD_COLOR_RGB and TD_NO_COLOR are both undefined)

// TD_NO_COLOR - no color, just ascii characters (fastest)

// TD_NO_TEXTURES - no textures, no stb_image.h

// TD_DISABLE_INPUT - no input

// Render in the terminal
#define TD_TERMINAL

// Comment out this line to disable RGB (if color doesn't work)
#define TD_COLOR_RGB

#include "TD/terdimension.h"
#include "TD/td_time.h"

// Light source
#define LIGHT_SOURCE (TD_Vec3){10.f,10.f,-10.f}
#define DIFFUSE 0.8f
#define AMBIENT 0.2f

// Fragment shaders
TD_FUNC TD_Color test_shader(TD_ShaderInfo* si){
	TD_Vec3 light_dir = TD_Vec3_normalize(TD_Vec3_sub(LIGHT_SOURCE,si->pos));
	float diffuse = TD_MAX(TD_Vec3_dot(TD_Vec3_normalize(si->normal),light_dir),0.f);
	return (TD_Color){
		(DIFFUSE*diffuse+AMBIENT)*si->color.r,
		(DIFFUSE*diffuse+AMBIENT)*si->color.g,
		(DIFFUSE*diffuse+AMBIENT)*si->color.b
	};
}
static TD_Color texture_shader(TD_ShaderInfo* si){
	return TD_sample_texture(si->uv.x,si->uv.y,si->texture);
}

// Texture of lebron's divine face
TD_Texture lebron_texture = TD_TextureEMPTY;

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
		TD_WHITE
	},
	(TD_Vec3[]){
		TD_Vec3BACK,
		TD_Vec3FRONT,
		TD_Vec3LEFT,
		TD_Vec3RIGHT,
		TD_Vec3UP,
		TD_Vec3DOWN
	},
	NULL,
	NULL,
	// v = vertex index, c = color / uv index
	// {v1,v2,v3, c1,c2,c3, normal index}
	(TD_Face[]){
		// Back
		{2,1,0, 2,1,0, 0},
		{2,3,1, 2,3,1, 0},
		// Front
		{6,5,4, 3,0,1, 1},
		{6,7,5, 3,2,0, 1},
		// Left
		{0,1,6, 0,1,3, 2},
		{7,6,1, 2,3,1, 2},
		// Right
		{4,3,2, 1,3,2, 3},
		{5,3,4, 0,3,1, 3},
		// Top
		{3,7,1, 3,2,1, 4},
		{3,5,7, 3,0,2, 4},
		// Bottom
		{0,6,2, 0,3,2, 5},
		{6,4,2, 3,1,2, 5}
	},
	12,
	(TD_Transform){
		(TD_Vec3){0.f,0.f,5.f},
		TD_Vec3ZERO,
		TD_Vec3IDENTITY
	}
};

TD_Mesh textured_plane = (TD_Mesh){
	(TD_Vec3[]){
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{-0.5f,0.5f,0.f},
		{0.5f,0.5f,0.f}
	},
	NULL,
	NULL,
	(TD_Vec2[]){
		{0.f,0.f},
		{1.f,0.f},
		{0.f,1.f},
		{1.f,1.f}
	},
	&lebron_texture,
	(TD_Face[]){
		{0,1,2, 0,1,2},
		{1,3,2, 1,3,2},
		{2,1,0, 2,1,0},
		{2,3,1, 2,3,1}
	},
	4,
	(TD_Transform){
		(TD_Vec3){-2.f,0.f,4.f},
		TD_Vec3ZERO,
		TD_Vec3IDENTITY
	}
};

int main(void){
	// Change the resolution
	// 200x200 and beyond is too laggy
	if(!TD_init(200,120))
		return 1;

	// Time variables
	TD_time_t last_frame = TD_get_ticks();
	float deltaTime = 0.f, FPS = 1.f;
	float time = 0.f;

	if(!TD_load_texture("assets/lebron_img.jpg",&lebron_texture))
		exit(0);

	TD_clear_screen();

	while(1){
		// Clear buffers
		TD_clear_buffers();

		// Render the cube on screen
		TD_use_shader(test_shader);
		TD_render_mesh(&cube_mesh);

		// Render the textured plane on screen
		TD_use_shader(texture_shader);
		TD_render_mesh(&textured_plane);

		// Display changes
		TD_update_screen();

		// Rotate cube
		cube_mesh.transform.rotation.y += 3.5f*deltaTime;
		cube_mesh.transform.rotation.x += 4.f*deltaTime;

		// Rotate the plane
		textured_plane.transform.rotation.y += 2.f*deltaTime;
		textured_plane.transform.rotation.z += 0.25f*deltaTime;
		textured_plane.transform.scale = TD_Vec3_scale(TD_Vec3IDENTITY,sin(time)*0.5f+0.75f);

		// Handle input
		// If you wish to disable input, please omit this part of the code
		#define PLAYER_SPEED 4.f
		#define PLAYER_LOOK 2.f
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
		
		// Applies movement
		movement_vector = TD_Vec3_scale(movement_vector,deltaTime);
		TD_Vec3 inverse_camera_rotation = (TD_Vec3){0.f,-TD_camera.rotation.y,0.f};
		movement_vector = TD_Vec3_rotationZYX(&inverse_camera_rotation,&movement_vector);
		TD_camera.position = TD_Vec3_add(TD_camera.position,movement_vector);

		// Advance time
		time += deltaTime;

		// Update the FPS counter and deltaTime
		deltaTime = TD_get_deltaTime(&last_frame);
		FPS = TD_GET_FPS(deltaTime);
		printf("FPS: %.f   \n",FPS);
		TD_Vec3_print("pos: ",TD_camera.position);
		TD_Vec3_print("rot: ",TD_Vec3_scale(TD_camera.rotation,180/3.1415f));
	}

	TD_quit();
}
