#ifndef TERDIMENSION_H
#define TERDIMENSION_H

#include <math.h>

// This library will have this prefix in all names
// TD_*
// You can change it, by changing this macro
// NOTE: Changing this macro will NOT change other macros names
// Example: To change prefix to TURD_*
// #define TD_(name) TURD_##name
#define TD_(name) TD_##name

//====== MACROS ======
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

//====== Types =======

// 3D Vector
typedef struct {
	float x, y, z;
} TD_(Vec3);

// Triangle face
typedef struct {
	int a, b, c;	// Indices of vertex, uv, color in the model
	//int normal;	// Index of the normal vector in the model
} TD_(Face);

// Colors
typedef struct {
	unsigned char r, g, b;	// Ranges from 0 - 255 (inclusive)
	//unsigned char a;		- IMPLEMENTED LATER
} TD_(Color);

// 3D Transform (For simple transformations)
typedef struct{
	TD_(Vec3) position;
	TD_(Vec3) rotation;
	TD_(Vec3) scale;
} TD_(Transform);

// 3D Model
typedef struct {
	TD_(Vec3)* vertices;
	//TD_(Vec3)* normals;	- IMPLEMENTED LATER
	//TD_(Vec3)* uvs;		- IMPLEMENTED LATER
	TD_(Face)* faces;
} TD_(Model);

//===== Globals ======

// Screen width / height
unsigned int TD_(SW) = 0;
unsigned int TD_(SH) = 0;

// Terminal implementation
#ifdef TD_TERMINAL

// Create the frame buffer (Color buffer)
// Private to avoid memory insecurity
static TD_(Color)* TD_(frame_buffer) = NULL;

// Depth buffer
// Private to avoid memory insecurity
static float* TD_(depth_buffer) = NULL;

// Initialize the whole library
// w : Screen Width, h : Screen Height
_Bool TD_(init)(unsigned int w, unsigned int h){
	TD_(SW) = w;	// Set the screen properties
	TD_(SH) = h;

	// Allocate frame buffer
	if(TD_(frame_buffer) != NULL)
		TD_FREE(TD_(frame_buffer));
	TD_(frame_buffer) = (TD_(Color)*) TD_MALLOC(sizeof(TD_(Color))*w*h);

	// Check if memory successfully allocated
	if(TD_(frame_buffer) == NULL){
		TD_LOG("TD_LOG: Failed to allocate frame buffer with size: %lu\n",(size_t)(sizeof(TD_(Color))*w*h));
		return 0;
	}

	// Allocate depth buffer
	if(TD_(depth_buffer) != NULL)
		TD_FREE(TD_(depth_buffer));
	TD_(depth_buffer) = (float*) TD_MALLOC(sizeof(float)*w*h);

	// Check if memory successfully allocated
	if(TD_(depth_buffer) == NULL){
		TD_LOG("TD_LOG: Failed to allocate depth buffer with size: %lu\n",(size_t)(sizeof(float)*w*h));
		return 0;
	}

	// Fill buffers with zeros
	TD_MEMSET(TD_(frame_buffer),0,sizeof(TD_(Color))*w*h);
	TD_MEMSET(TD_(depth_buffer),0,sizeof(float)*w*h);

	// Return "success"
	return 1;
}

void TD_(quit)(void){
	TD_(SW) = 0;	// Set the screen to be 0x0 px
	TD_(SH) = 0;

	// Free the buffers
	if(TD_(frame_buffer))
		TD_FREE(TD_(frame_buffer));
	if(TD_(depth_buffer))
		TD_FREE(TD_(depth_buffer));
}

#endif

#endif