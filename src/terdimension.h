#ifndef TERDIMENSION_H
#define TERDIMENSION_H

#include <stdio.h>
#include <stdlib.h>

// This library will have this prefix in all names
// TD_*

//========= MACROS =========
/*
TD_TERMINAL :
	If defined before including this header, will render in the terminal using ASCII characters.
TD_SDL :
	If defined before including this header, will render in a SDL window (slow).
*/

// Memory allocation macro (can be overwritten before including this header)
#ifndef TD_MALLOC
#define TD_MALLOC(sz) malloc((sz))
#endif

// Memory freeing macro (can be overwritten before including this header)
#ifndef TD_FREE
#define TD_FREE(ptr) free((ptr))
#endif

// memset (can be overwritten)
#ifndef TD_MEMSET
#include <string.h>
#define TD_MEMSET(ptr,c,sz) memset((ptr),(c),(sz));
#endif

// Logging macro (prints by default)
#ifndef TD_LOG
#define TD_LOG(fmt,...) printf("TD_LOG: " fmt,##__VA_ARGS__);
#endif

// Gives smaller value
#define TD_MIN(a,b) ((a)<(b)?(a):(b))

// Gives bigger value
#define TD_MAX(a,b) ((a)>(b)?(a):(b))

// Clamps number to limits
#define TD_CLAMP(n,a,b) (TD_MIN(TD_MAX(n,a),b))

// Square of a number
#define TD_SQUARE(x) ((x)*(x))

//========= Types ==========

// 3D Vector
typedef struct {
	float x, y, z;
} TD_Vec3;

// Triangle face
typedef struct {
	unsigned int a, b, c;	// Indices of vertex, uv, color in the model
	//int normal;			// Index of the normal vector in the model
} TD_Face;

// Colors
typedef struct {
	unsigned char r, g, b;	// Ranges from 0 - 255 (inclusive)
	//unsigned char a;		- IMPLEMENTED LATER
} TD_Color;

// 3D Transform (For simple transformations)
typedef struct{
	TD_Vec3 position;
	TD_Vec3 rotation;
	TD_Vec3 scale;
} TD_Transform;

// 3D camera
typedef struct{
	TD_Vec3 position;
	TD_Vec3 rotation;
} TD_Camera;

// 3D Model
typedef struct {
	TD_Vec3* vertices;
	TD_Color* colors;
	//TD_Vec3* normals;		- IMPLEMENTED LATER
	//TD_Vec3* uvs;
	TD_Face* faces;
	unsigned int face_count;
	TD_Transform transform;
} TD_Mesh;

//===== Function prototypes ======
#define TD_FUNC static inline

TD_FUNC _Bool TD_init(unsigned int, unsigned int);
TD_FUNC void TD_quit(void);
TD_FUNC void TD_set_pixel(int,int,TD_Color);
TD_FUNC void TD_update_screen(void);
TD_FUNC void TD_clear_screen(void);

//========== Constants ===========

// Colors
#define TD_BLACK	(TD_Color){0,0,0}
#define TD_WHITE	(TD_Color){255,255,255}
#define TD_RED		(TD_Color){255,0,0}
#define TD_GREEN	(TD_Color){0,255,0}
#define TD_BLUE		(TD_Color){0,0,255}

// Vector constants
#define TD_Vec3ZERO 	(TD_Vec3){0.f,0.f,0.f}
#define TD_Vec3IDENTITY (TD_Vec3){1.f,1.f,1.f}
#define TD_Vec3UP 		(TD_Vec3){0.f,1.f,0.f}
#define TD_Vec3DOWN 	(TD_Vec3){0.f,-1.f,0.f}
#define TD_Vec3LEFT 	(TD_Vec3){-1.f,0.f,0.f}
#define TD_Vec3RIGHT 	(TD_Vec3){1.f,0.f,0.f}
#define TD_Vec3FRONT 	(TD_Vec3){0.f,0.f,1.f}
#define TD_Vec3BACK 	(TD_Vec3){0.f,0.f,-1.f}

// Default transform
#define TD_TransformIDENTITY (TD_Transform){TD_Vec3ZERO,TD_Vec3ZERO,TD_Vec3IDENTITY}

// Default camera
#define TD_CameraDEFAULT (TD_Camera){TD_Vec3ZERO,TD_Vec3ZERO}

// Winding order
#define TD_CCW 0
#define TD_CW 1

// Amount of bytes allocated per pixel for stdout buffer
// 100 bytes per pixel takes about 1-2 MB for most resolutions and works well
#define TD_STDOUT_BYTES_PER_PIXEL 100

//=========== Globals ============

// Screen width / height
// DO NOT CHANGE MANUALLY (will cause segfault)
unsigned int TD_SW = 0;
unsigned int TD_SH = 0;

// Precalculated halves of SW and SH
unsigned int TD_SW2 = 0;
unsigned int TD_SH2 = 0;

// Precalculated aspect ratio
float TD_ASPECT_RATIO = 1.f;

// Camera
TD_Camera TD_camera = TD_CameraDEFAULT;

// Background color
TD_Color TD_background_color = TD_BLACK;

// Clockwise or Counter-Clockwise
_Bool TD_winding = TD_CCW;

//=============== Terminal implementation ===================
#ifdef TD_TERMINAL

// Constants for rendering
#define TD_TOP_HALF_BLOCK	"\u2580"
#define TD_NO_COLORS		"\033[0m"
#define TD_CLEAR_TERMINAL	"\e[1;1H\e[2J"
#define TD_HIDE_CURSOR		"\e[?25l"
#define TD_SHOW_CURSOR		"\e[?25h"

// Macros for rendering
#define TD_MOVE_CURSOR(x,y) printf("\033[%d;%dH", (y), (x))
TD_FUNC void  TD_clear_screen() { printf(TD_CLEAR_TERMINAL); }

// Include the terminal color header
#include "td_color.h"

// Char buffer for stdout (to improve performance)
static char* TD_STDOUT_BUFFER = NULL;

// Create the frame buffer (Color buffer)
// Private to avoid memory insecurity
static TD_Color* TD_frame_buffer = NULL;

// Depth buffer
// Private to avoid memory insecurity
static float* TD_depth_buffer = NULL;

// Input handling
#include "td_input.h"

// Initialize the whole library
// w : Screen Width, h : Screen Height
// If w or h are odd numbers, there might be rendering problems
TD_FUNC _Bool TD_init(unsigned int w, unsigned int h){
	TD_SW = w;	// Set the screen properties
	TD_SH = h;

	TD_SW2 = w/2;	// Precalculate SW/2
	TD_SH2 = h/2;	// 				SH/2

	TD_ASPECT_RATIO = (float)TD_SW/(float)TD_SH; // Precalculate aspect ratio

	// Allocate frame buffer
	if(TD_frame_buffer)
		TD_FREE(TD_frame_buffer);
	TD_frame_buffer = (TD_Color*) TD_MALLOC(sizeof(TD_Color)*w*h);

	// Check if memory successfully allocated
	if(TD_frame_buffer == NULL){
		TD_LOG("Failed to allocate frame buffer with size: %lu\n",(size_t)(sizeof(TD_Color)*w*h));
		return 0;
	}

	// Allocate depth buffer
	if(TD_depth_buffer)
		TD_FREE(TD_depth_buffer);
	TD_depth_buffer = (float*) TD_MALLOC(sizeof(float)*w*h);

	// Check if memory successfully allocated
	if(TD_depth_buffer == NULL){
		TD_LOG("Failed to allocate depth buffer with size: %lu\n",(size_t)(sizeof(float)*w*h));
		return 0;
	}

	// Allocate stdout buffer
	if(TD_STDOUT_BUFFER)
		TD_FREE(TD_STDOUT_BUFFER);
	TD_STDOUT_BUFFER = (char*) TD_MALLOC(w*h*TD_STDOUT_BYTES_PER_PIXEL);

	// Memory check
	if(TD_STDOUT_BUFFER == NULL){
		TD_LOG("Failed to allocate STDOUT buffer with size: %lu\n",(size_t)(w*h*TD_STDOUT_BYTES_PER_PIXEL));
		return 0;
	}

	// Fill buffers with zeros
	TD_MEMSET(TD_frame_buffer,0,sizeof(TD_Color)*w*h);
	TD_MEMSET(TD_depth_buffer,0,sizeof(float)*w*h);
	TD_MEMSET(TD_STDOUT_BUFFER,0,w*h*TD_STDOUT_BYTES_PER_PIXEL);

	// Set the stdout buffer to TD_STDOUT_BUFFER
	setvbuf(stdout,TD_STDOUT_BUFFER,_IOFBF,w*h*TD_STDOUT_BYTES_PER_PIXEL);

	// Hide cursor
	printf(TD_HIDE_CURSOR);

	// Register TD_quit as a function to run when program exits
	atexit(TD_quit);

	// Initialize input (or not input)
	TD_INPUT_INIT();

	// Return "success"
	return 1;
}

TD_FUNC void TD_quit(void){
	// Quit input
	TD_INPUT_QUIT();

	// Clear the terminal
	printf(TD_NO_COLORS TD_CLEAR_TERMINAL);

	// Make cursor visible again
	printf(TD_SHOW_CURSOR);

	// Flush stdout one last time
	fflush(stdout);

	// Set stdout's buffering to be line buffering
	setvbuf(stdout,NULL,_IOLBF,0);

	TD_SW = 0;	// Set the screen to be 0x0 px
	TD_SH = 0;

	// Free the buffers
	if(TD_frame_buffer)
		TD_FREE(TD_frame_buffer);
	TD_frame_buffer = NULL;
	if(TD_depth_buffer)
		TD_FREE(TD_depth_buffer);
	TD_depth_buffer = NULL;
	if(TD_STDOUT_BUFFER)
		TD_FREE(TD_STDOUT_BUFFER);
	TD_STDOUT_BUFFER = NULL;
}

TD_FUNC float TD_sample_depth(int x, int y){
	if(x < 0 || y < 0 || x >= TD_SW || y >= TD_SH)
		return 999.f;
	return TD_depth_buffer[y*TD_SW+x];
}

TD_FUNC void TD_set_pixel(int x, int y, TD_Color c){
	if(x < 0 || y < 0 || x >= TD_SW || y >= TD_SH)
		return;
	TD_frame_buffer[y*TD_SW+x] = c;
}

TD_FUNC void TD_update_screen(void){
	// Move cursor to top left of screen
	TD_MOVE_CURSOR(1,1);

	// Pointer to current Upper-Color and Bottom-Color
	TD_Color *uc = TD_frame_buffer, *bc = TD_frame_buffer+TD_SW;

	// Pointer to current depth buffer pixel
	float* depth_ptr = TD_depth_buffer;

	// Loop through each pixel pair in the color buffer
	for(int y = 0; y < TD_SH-1; y += 2){
		for(int x = 0; x < TD_SW; x++, uc++, bc++, depth_ptr++){
			TD_COLOR_printchar(uc,bc);

			// Clear current pixels
			*uc = TD_background_color;
			*bc = TD_background_color;

			// Set depth buffer pixels to max distance
			*depth_ptr = 999.f;
			*(depth_ptr+TD_SW) = 999.f;
		}
		puts(TD_NO_COLORS);
		uc += TD_SW;
		bc += TD_SW;
		depth_ptr += TD_SW;
	}
	// Flush the buffer into stdout
	fflush(stdout);
}

#elif defined(TD_SDL)

//============ SDL Implementation ===============

#error TD_SDL not implemented yet!


#else	// NO IMPLEMENTATION SELECTED -> ERROR

#error Please define either TD_TERMINAL or TD_SDL before including terdimension.h

#endif

//========== Math functions ======
#include "td_math.h"

//========== Rasterization =======
#include "td_raster.h"

#endif