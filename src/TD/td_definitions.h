#ifndef TERDIMENSION_DEFINITIONS_H
#define TERDIMENSION_DEFINITIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

#ifndef TD_REALLOC
#define TD_REALLOC(ptr,sz) realloc((ptr),(sz))
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

typedef struct {
	float x, y;
} TD_Vec2;

// Triangle face
typedef struct {
	unsigned int a, b, c;	// Indices of vertices
	unsigned int d, e, f;	// Indices of vertex attributes
	unsigned int n1, n2, n3;// Indices of vertex normals
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
typedef TD_Transform TD_Camera;

// Texture
typedef struct{
	unsigned int width, height;
	TD_Color* pixels;
} TD_Texture;

// 3D Model
typedef struct {
	TD_Vec3* vertices;
	TD_Color* colors;
	TD_Vec3* normals;
	TD_Vec2* uvs;
	TD_Texture texture;
	TD_Face* faces;
	size_t face_count;
	TD_Transform transform;
} TD_Mesh;

// Struct containing information given to the fragment shader
typedef struct{
	TD_Vec3 bc;
	TD_Vec3 pos;
	TD_Color color;
	TD_Vec3 normal;
	TD_Vec2 uv;
	TD_Texture* texture;
} TD_ShaderInfo;

// Fragment shader (function)
typedef TD_Color (*TD_Shader)(TD_ShaderInfo*);

// Enum for render flags
typedef enum{
	TD_RENDER_RGB = 1,			// RGB Full Color, compatible with only some terminals
	TD_RENDER_COLOR = 2,		// 4 bit color, combine with TD_RENDER_BRIGHT_COLORS for more colors
	TD_RENDER_NO_COLOR = 0, 	// No color, just ASCII characters, full compatibility
	TD_RENDER_UNICODE = 4,		// Render at twice the resolution, compatible with only some terminals
	TD_RENDER_BRIGHT_COLORS = 8	// Only for 4 bit color, compatible with only some terminals
} TD_RenderFlags;

//========== Constants ===========

// Colors
#define TD_BLACK	(TD_Color){0,0,0}
#define TD_WHITE	(TD_Color){255,255,255}
#define TD_RED		(TD_Color){255,0,0}
#define TD_GREEN	(TD_Color){0,255,0}
#define TD_BLUE		(TD_Color){0,0,255}

// Vector3 constants
#define TD_Vec3ZERO 	(TD_Vec3){0.f,0.f,0.f}
#define TD_Vec3IDENTITY (TD_Vec3){1.f,1.f,1.f}
#define TD_Vec3UP 		(TD_Vec3){0.f,1.f,0.f}
#define TD_Vec3DOWN 	(TD_Vec3){0.f,-1.f,0.f}
#define TD_Vec3LEFT 	(TD_Vec3){-1.f,0.f,0.f}
#define TD_Vec3RIGHT 	(TD_Vec3){1.f,0.f,0.f}
#define TD_Vec3FRONT 	(TD_Vec3){0.f,0.f,1.f}
#define TD_Vec3BACK 	(TD_Vec3){0.f,0.f,-1.f}

// Vector2 constants
#define TD_Vec2ZERO		(TD_Vec2){0.f,0.f}
#define TD_Vec2IDENTITY	(TD_Vec2){1.f,1.f}
#define TD_Vec2UP		(TD_Vec2){0.f,1.f}
#define TD_Vec2DOWN		(TD_Vec2){0.f,-1.f}
#define TD_Vec2LEFT		(TD_Vec2){-1.f,0.f}
#define TD_Vec2RIGHT	(TD_Vec2){1.f,0.f}

// Default transform
#define TD_TransformIDENTITY (TD_Transform){TD_Vec3ZERO,TD_Vec3ZERO,TD_Vec3IDENTITY}

// Empty mesh
#define TD_MeshEMPTY (TD_Mesh){NULL,NULL,NULL,NULL,NULL,NULL,0,TD_TransformIDENTITY};

// Default camera
#define TD_CameraDEFAULT TD_TransformIDENTITY

// Empty texture
#define TD_TextureEMPTY (TD_Texture){0,0,NULL}

// Winding order
#define TD_CCW 0
#define TD_CW 1

// Default render flags
#define TD_RENDER_DEFAULT (TD_RenderFlags)(TD_RENDER_RGB | TD_RENDER_UNICODE)
#define TD_set_render_flags(f) ({TD_render_flags = (f);})

// Default depth
#define TD_FAR_CLIP 50.f
#define TD_NEAR_CLIP 0.1f

// No shader (more readable)
#define TD_NO_SHADER NULL

// Amount of bytes allocated per pixel for stdout buffer
// 100 bytes per pixel takes about 1-2 MB for most resolutions and works well
#define TD_STDOUT_BYTES_PER_PIXEL 100

// Function type
#define TD_FUNC extern

//===== Function prototypes ======

TD_FUNC bool TD_init(unsigned int, unsigned int);
TD_FUNC void TD_quit(void);
TD_FUNC void TD_set_pixel(int,int,TD_Color);
TD_FUNC float TD_sample_depth(int,int);
TD_FUNC void TD_set_depth(int,int,float);
TD_FUNC void TD_update_screen(void);
TD_FUNC void TD_clear_screen(void);
TD_FUNC void TD_clear_buffers(void);

//=========== Globals ============

// Screen width / height
// DO NOT CHANGE MANUALLY (will cause segfault)
extern unsigned int TD_SW;
extern unsigned int TD_SH;

// Precalculated halves of SW and SH
extern unsigned int TD_SW2;
extern unsigned int TD_SH2;

// Precalculated aspect ratio
extern float TD_ASPECT_RATIO;

// Camera
extern TD_Camera TD_camera;

// Background color
extern TD_Color TD_background_color;

// Clockwise or Counter-Clockwise
extern bool TD_winding;
#define TD_WIND_CW TD_winding = TD_CW
#define TD_WIND_CCW TD_winding = TD_CCW

// Rendering flags
extern TD_RenderFlags TD_render_flags;

// Current fragment shader used
extern TD_Shader TD_shader;
#define TD_use_shader(f) TD_shader = (f)

// Macros for printing text
#define TD_MOVE_CURSOR(x,y) printf("\033[%d;%dH", (y), (x))
#define TD_PRINT(x,y,txt,...) ({TD_MOVE_CURSOR((x),(y)); printf((txt),##__VA_ARGS__);})
#define TD_UPDATE_TEXT() fflush(stdout)

#endif
