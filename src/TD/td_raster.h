#ifndef TERDIMENSION_RASTERIZER_H
#define TERDIMENSION_RASTERIZER_H

#include "td_definitions.h"
#include "td_math.h"

// 2D integer vector
typedef struct{
	int x, y;
} TD_Vec2i;

// 2D bounds (minimum -> maximum)
typedef struct{
	int xmin, xmax;
	int ymin, ymax;
} TD_Bounds;

// Transform 2D (actually 3D) world coordinates into 2D screen coordinates
#define TD_TO_SCREEN_X(x) (int)((float)(x)*((float)TD_SW)/TD_ASPECT_RATIO+0.5f+(TD_SW2))
#define TD_TO_SCREEN_Y(y) (int)(-(y)*((float)TD_SH)+0.5f+(TD_SH2))

// Transform 2D screen coordinates into 2D (actually 3D) world coordinates
#define TD_TO_WORLD_X(x) (((float)(x)+0.5f)-(float)(TD_SW2))/(float)TD_SW*TD_ASPECT_RATIO
#define TD_TO_WORLD_Y(y) -(((float)(y)+0.5f)-(float)(TD_SH2))/(float)TD_SH

// Returns if x <= 0 if CCW or x >= 0 if CW
#define TD_CHECK_EDGE(x) ((TD_winding)?((x)>=0):((x)<=0))

TD_FUNC TD_Vec2i TD_to_screen(TD_Vec3*);
TD_FUNC TD_Vec3 TD_to_world(int, int);
TD_FUNC TD_Bounds TD_triangle_bounds(TD_Vec2i, TD_Vec2i, TD_Vec2i);
TD_FUNC float TD_edge_function(TD_Vec3*, TD_Vec3*, TD_Vec3*);
TD_FUNC TD_Vec3 TD_triangle_pixel(TD_Vec3*, TD_Vec3*, TD_Vec3*, TD_Vec3);
TD_FUNC _Bool TD_is_backface(TD_Vec3*, TD_Vec3*, TD_Vec3*);
TD_FUNC float TD_clip_edge(TD_Vec3*, TD_Vec3*);
TD_FUNC TD_Color TD_interpolate_2_colors(TD_Color*, TD_Color*, float);
TD_FUNC TD_Vec2 TD_interpolate_2_vec2(TD_Vec2*, TD_Vec2*, float);

#define TD_SHOULD_CLIP2(_a, _b, _c) ((_a).z < TD_NEAR_CLIP && (_b).z < TD_NEAR_CLIP && (_c).z >= TD_NEAR_CLIP)
#define TD_SHOULD_CLIP1(_a, _b, _c) ((_a).z < TD_NEAR_CLIP && (_b).z >= TD_NEAR_CLIP && (_c).z >= TD_NEAR_CLIP)

TD_FUNC void TD_render_triangle(TD_Vec3, TD_Vec3, TD_Vec3, TD_Color*, TD_Vec2*, TD_Texture*, TD_Vec3*, TD_Face*);
TD_FUNC void TD_render_face(TD_Mesh*, TD_Face*);
TD_FUNC void TD_render_mesh(TD_Mesh*);

#endif
