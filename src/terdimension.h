#ifndef TERDIMENSION_H
#define TERDIMENSION_H

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
#include <stdlib.h>
#define TD_MALLOC(sz) malloc((sz))
#endif

// Memory freeing macro (can be overwritten before including this header)
#ifndef TD_FREE
#include <stdlib.h>
#define TD_FREE(ptr) free((ptr))
#endif

// memset (can be overwritten)
#ifndef TD_MEMSET
#include <string.h>
#define TD_MEMSET(ptr,c,sz) memset((ptr),(c),(sz));
#endif

// Logging macro (prints by default)
#ifndef TD_LOG
#include <stdio.h>
#define TD_LOG(fmt,...) printf((fmt),##__VA_ARGS__);
#endif

// Gives smaller value
#define TD_MIN(a,b) ((a)<(b)?(a):(b))

// Gives bigger value
#define TD_MAX(a,b) ((a)>(b)?(a):(b))

// Clamps number to limits
#define TD_CLAMP(n,a,b) (TD_MIN(TD_MAX(n,a),b))

//========= Types ==========

// 3D Vector
typedef struct {
	float x, y, z;
} TD_Vec3;

// Triangle face
typedef struct {
	int a, b, c;	// Indices of vertex, uv, color in the model
	//int normal;	// Index of the normal vector in the model
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

// 3D Model
typedef struct {
	TD_Vec3* vertices;
	//TD_Color* colors;		- IMPLEMENTED SOON
	//TD_Vec3* normals;		- IMPLEMENTED LATER
	//TD_Vec3* uvs;
	TD_Face* faces;
} TD_Model;

//===== Function prototypes ======
static inline _Bool TD_init(unsigned int, unsigned int);
static inline void TD_quit(void);
void TD_set_pixel(int,int,TD_Color);
static inline void TD_update_screen(void);
static inline void TD_clear_screen(void);

//=========== Globals ============

// Screen width / height
// DO NOT CHANGE MANUALLY (will cause segfault)
unsigned int TD_SW = 0;
unsigned int TD_SH = 0;

// Precalculated halves of SW and SH
unsigned int TD_SW2 = 0;
unsigned int TD_SH2 = 0;


//========== Constants ===========

#define TD_BLACK (TD_Color){0,0,0}
#define TD_WHITE (TD_Color){255,255,255}


//=============== Terminal implementation ===================
#ifdef TD_TERMINAL

// Constants for rendering
#define TD_TOP_HALF_BLOCK "\u2580"
#define TD_SET_FOREGROUND "\033[38;2;%d;%d;%dm"
#define TD_SET_BACKGROUND "\033[48;2;%d;%d;%dm"
#define TD_NO_COLORS "\033[0m"

// Char buffer for stdout (to improve performance)
static char* TD_STDOUT_BUFFER = NULL;

// Create the frame buffer (Color buffer)
// Private to avoid memory insecurity
static TD_Color* TD_frame_buffer = NULL;

// Depth buffer
// Private to avoid memory insecurity
static float* TD_depth_buffer = NULL;

// Initialize the whole library
// w : Screen Width, h : Screen Height
_Bool TD_init(unsigned int w, unsigned int h){
	TD_SW = w;	// Set the screen properties
	TD_SH = h;

	TD_SW2 = w/2;	// Precalculate SW/2
	TD_SH2 = h/2;	// 				SH/2

	// Allocate frame buffer
	if(TD_frame_buffer)
		TD_FREE(TD_frame_buffer);
	TD_frame_buffer = (TD_Color*) TD_MALLOC(sizeof(TD_Color)*w*h);

	// Check if memory successfully allocated
	if(TD_frame_buffer == NULL){
		TD_LOG("TD_LOG: Failed to allocate frame buffer with size: %lu\n",(size_t)(sizeof(TD_Color)*w*h));
		return 0;
	}

	// Allocate depth buffer
	if(TD_depth_buffer)
		TD_FREE(TD_depth_buffer);
	TD_depth_buffer = (float*) TD_MALLOC(sizeof(float)*w*h);

	// Check if memory successfully allocated
	if(TD_depth_buffer == NULL){
		TD_LOG("TD_LOG: Failed to allocate depth buffer with size: %lu\n",(size_t)(sizeof(float)*w*h));
		return 0;
	}

	// Allocate stdout buffer
	if(TD_STDOUT_BUFFER)
		TD_FREE(TD_STDOUT_BUFFER);
	TD_STDOUT_BUFFER = (char*) TD_MALLOC(w*h*10);	// We dedicate SW*SH*10 bytes

	// Memory check
	if(TD_STDOUT_BUFFER == NULL){
		TD_LOG("TD_LOG: Failed to allocate STDOUT buffer!\n");
		return 0;
	}

	// Fill buffers with zeros
	TD_MEMSET(TD_frame_buffer,0,sizeof(TD_Color)*w*h);
	TD_MEMSET(TD_depth_buffer,0,sizeof(float)*w*h);
	TD_MEMSET(TD_STDOUT_BUFFER,0,w*h*10);

	// Set the stdout buffer to TD_STDOUT_BUFFER
	setvbuf(stdout,TD_STDOUT_BUFFER,_IOFBF,w*h*10);

	// Hide cursor
	printf("\e[?25l");

	// Return "success"
	return 1;
}

void TD_quit(void){
	TD_SW = 0;	// Set the screen to be 0x0 px
	TD_SH = 0;

	// Free the buffers
	if(TD_frame_buffer)
		TD_FREE(TD_frame_buffer);
	if(TD_depth_buffer)
		TD_FREE(TD_depth_buffer);
	if(TD_STDOUT_BUFFER)
		TD_FREE(TD_STDOUT_BUFFER);

	// Make cursor visible again
	printf("\e[?25h");
}

void TD_set_pixel(int x, int y, TD_Color c){
	if(x < 0 || y < 0 || x >= TD_SW || y >= TD_SH)
		return;
	TD_frame_buffer[y*TD_SW+x] = c;
}

void TD_update_screen(void){
	for(int y = 0; y < TD_SH-1; y += 2){
		for(int x = 0; x < TD_SW; x++){
			// Upper half of character
			TD_Color uc = TD_frame_buffer[TD_SW*y+x];
			// Bottom half of character
			TD_Color bc = TD_frame_buffer[TD_SW*(y+1)+x];
			// Change colors and print
			printf(
				TD_SET_FOREGROUND
				TD_SET_BACKGROUND
				TD_TOP_HALF_BLOCK,
				uc.r, uc.g, uc.b,
				bc.r, bc.g, bc.b
			);
			TD_depth_buffer[TD_SW*y+x] = 100.f;
			TD_depth_buffer[TD_SW*(y+1)+x] = 100.f;
		} puts(TD_NO_COLORS);
	}
	fflush(stdout);
}

static inline void TD_clear_screen(void){
	printf("\e[1;1H\e[2J");
}

#else	// NO IMPLEMENTATION SELECTED -> ERROR

#error Please define either TD_TERMINAL or TD_SDL before including terdimension.h

#endif

//========== Math functions ============
#include "td_math.h"

//========== Rasterization =============
#include "td_raster.h"

#endif