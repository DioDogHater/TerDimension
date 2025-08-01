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

#include "td_definitions.h"

//=============== Terminal implementation ===================

// Include the terminal rendering header
#include "td_render.h"

// Char buffer for stdout (to improve performance)
static char* TD_STDOUT_BUFFER = NULL;

// Create the frame buffer (Color buffer)
// Private to avoid memory insecurity
static TD_Color* TD_frame_buffer = NULL;

// Depth buffer
// Private to avoid memory insecurity
static float* TD_depth_buffer = NULL;

// Texture handling
#ifndef TD_NO_TEXTURES
#include "td_texture.h"
#endif

// Input handling
#include "td_input.h"

// Initialize the whole library
// w : Screen Width, h : Screen Height
// If w or h are odd numbers, there might be rendering problems
TD_FUNC _Bool TD_init(unsigned int w, unsigned int h){
	TD_SW = w;	// Set the screen properties
	TD_SH = h;

	TD_SW2 = w/2;	//	Precalculate SW/2 and SH/2
	TD_SH2 = h/2;

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
	printf(TD_CLEAR_COLOR TD_CLEAR_TERMINAL);

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
		return TD_NEAR_CLIP;
	return TD_depth_buffer[y*TD_SW+x];
}

TD_FUNC void TD_set_depth(int x, int y, float depth){
	if(x < 0 || y < 0 || x >= TD_SW || y >= TD_SH)
		return;
	TD_depth_buffer[y*TD_SW+x] = depth;
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
			TD_render_pixel(uc,bc);

			// Clear current pixels
			*uc = TD_background_color;
			*bc = TD_background_color;
		}

		if((TD_render_flags & TD_RENDER_COLOR) || (TD_render_flags & TD_RENDER_RGB))
			puts(TD_CLEAR_COLOR);
		else
			putchar('\n');

		uc += TD_SW;
		bc += TD_SW;
		depth_ptr += TD_SW;
	}
	// Flush the buffer into stdout
	fflush(stdout);
}

TD_FUNC void TD_clear_buffers(void){
	TD_Color* c = TD_frame_buffer;
	float* d = TD_depth_buffer;
	for(long i = 0; i < TD_SW*TD_SH; i++, c++, d++){
		*c = TD_BLACK;
		*d = TD_FAR_CLIP;
	}
}

//========== Math functions ======
#include "td_math.h"

//========== Rasterization =======
#include "td_raster.h"

#endif
