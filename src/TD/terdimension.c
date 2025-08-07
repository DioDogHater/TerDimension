#include "terdimension.h"

//=========== Globals ============

// Screen width / height
// DO NOT CHANGE MANUALLY (will cause segfault)
unsigned int TD_SW;
unsigned int TD_SH;

// Precalculated halves of SW and SH
unsigned int TD_SW2;
unsigned int TD_SH2;

// Precalculated aspect ratio
float TD_ASPECT_RATIO = 1.f;

// Camera
TD_Camera TD_camera = TD_CameraDEFAULT;

// Background color
TD_Color TD_background_color = TD_BLACK;

// Clockwise or Counter-Clockwise
bool TD_winding = TD_CCW;
#define TD_WIND_CW TD_winding = TD_CW
#define TD_WIND_CCW TD_winding = TD_CCW

// Rendering flags
TD_RenderFlags TD_render_flags = TD_RENDER_DEFAULT;

// Current fragment shader used
TD_Shader TD_shader = NULL;
#define TD_use_shader(f) TD_shader = (f)

static bool TD_initialized = false;

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
		return false;
	}

	// Allocate depth buffer
	if(TD_depth_buffer)
		TD_FREE(TD_depth_buffer);
	TD_depth_buffer = (float*) TD_MALLOC(sizeof(float)*w*h);

	// Check if memory successfully allocated
	if(TD_depth_buffer == NULL){
		TD_LOG("Failed to allocate depth buffer with size: %lu\n",(size_t)(sizeof(float)*w*h));
		return false;
	}

	// Allocate stdout buffer
	if(TD_STDOUT_BUFFER)
		TD_FREE(TD_STDOUT_BUFFER);
	TD_STDOUT_BUFFER = (char*) TD_MALLOC(w*h*TD_STDOUT_BYTES_PER_PIXEL);

	// Memory check
	if(TD_STDOUT_BUFFER == NULL){
		TD_LOG("Failed to allocate STDOUT buffer with size: %lu\n",(size_t)(w*h*TD_STDOUT_BYTES_PER_PIXEL));
		return false;
	}

	// Fill buffers with zeros
	TD_MEMSET(TD_frame_buffer,0,sizeof(TD_Color)*w*h);
	TD_MEMSET(TD_depth_buffer,0,sizeof(float)*w*h);
	TD_MEMSET(TD_STDOUT_BUFFER,0,w*h*TD_STDOUT_BYTES_PER_PIXEL);

	// Set the stdout buffer to TD_STDOUT_BUFFER
	setvbuf(stdout,TD_STDOUT_BUFFER,_IOFBF,w*h*TD_STDOUT_BYTES_PER_PIXEL);

	// Hide cursor and reset colors to their defaults
	printf(TD_HIDE_CURSOR "\x1b[39;49m");
	fflush(stdout);

	// Register TD_quit as a function to run when program exits
	if(!TD_initialized)
		atexit(TD_quit);

	// Initialize input (or not input)
	TD_INPUT_INIT();

	TD_initialized = true;

	// Return "success"
	return true;
}

TD_FUNC void TD_quit(void){
	if(!TD_initialized) return;
	TD_initialized = false;

	// Quit input
	TD_INPUT_QUIT();

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
	TD_MOVE_CURSOR(0,0);

	// Pointer to current Upper-Color and Bottom-Color
	TD_Color *uc = TD_frame_buffer, *bc = TD_frame_buffer+TD_SW;

	// Loop through each pixel pair in the color buffer
	for(int y = 0; y < TD_SH-1; y += 2){
		for(int x = 0; x < TD_SW; x++, uc++, bc++){
			TD_render_pixel(uc,bc);
		}

		if((TD_render_flags & TD_RENDER_COLOR) || (TD_render_flags & TD_RENDER_RGB)){
			putchar_unlocked('\x1b');
			putchar_unlocked('[');
			putchar_unlocked('0');
			putchar_unlocked('m');
			putchar_unlocked('\n');
		}else
			putchar_unlocked('\n');

		uc += TD_SW;
		bc += TD_SW;
	}
	// Flush the buffer into stdout
	fflush(stdout);
}

TD_FUNC void TD_clear_screen(){
	printf(TD_CLEAR_TERMINAL);
	fflush(stdout);
}

TD_FUNC void TD_clear_buffers(void){
	TD_Color fill_value = (TD_render_flags & (TD_RENDER_COLOR | TD_RENDER_RGB)) ? TD_background_color : TD_BLACK;
	TD_Color* c = TD_frame_buffer;
	float* d = TD_depth_buffer;
	for(long i = 0; i < TD_SW*TD_SH; i++, c++, d++){
		*c = fill_value;
		*d = TD_FAR_CLIP;
	}
}