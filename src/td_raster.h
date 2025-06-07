#ifndef TERDIMENSION_RASTERIZER_H
#define TERDIMENSION_RASTERIZER_H

#include "terdimension.h"
#include "td_math.h"

typedef struct{
	TD_Vec3 bc;
	TD_Vec3 pos;
	//TD_Color color;	- IMPLEMENTED SOON
	//TD_Vec3 normal;	- IMPLEMENTED LATER
	//TD_Vec3 uv;
	//TD_... texture
} TD_ShaderInfo;

typedef struct{
	int x, y;
} TD_Vec2i;

typedef struct{
	int xmin, xmax;
	int ymin, ymax;
} TD_Bounds;

// Transform 2D (actually 3D) world coordinates into 2D screen coordinates
#define TD_TO_SCREEN_X(x) (int)((x)*(TD_SW)+0.5f+(TD_SW2))
#define TD_TO_SCREEN_Y(y) (int)(-(y)*(TD_SH)+0.5f+(TD_SH2))
TD_FUNC TD_Vec2i TD_to_screen(TD_Vec3* v){
	return (TD_Vec2i){TD_TO_SCREEN_X(v->x),TD_TO_SCREEN_Y(v->y)};
}

// Transform 2D screen coordinates into 2D (actually 3D) world coordinates
#define TD_TO_WORLD_X(x) (((float)(x)+0.5f)-(float)(TD_SW2))/(float)TD_SW
#define TD_TO_WORLD_Y(y) -(((float)(y)+0.5f)-(float)(TD_SH2))/(float)TD_SH
TD_FUNC TD_Vec3 TD_to_world(int x, int y){
	return (TD_Vec3){TD_TO_WORLD_X(x),TD_TO_WORLD_Y(y),0.f};
}

// Get bounds of a triangle
TD_FUNC TD_Bounds TD_triangle_bounds(TD_Vec2i a, TD_Vec2i b, TD_Vec2i c){
	return (TD_Bounds){
		TD_MAX(TD_MIN(a.x,TD_MIN(b.x,c.x)),0),
		TD_MIN(TD_MAX(a.x,TD_MAX(b.x,c.x)),TD_SW),
		TD_MAX(TD_MIN(a.y,TD_MIN(b.y,c.y)),0),
		TD_MIN(TD_MAX(a.y,TD_MAX(b.y,c.y)),TD_SH)
	};
}

// Get the result of an edge function for a triangle
TD_FUNC float TD_edge_function(TD_Vec3* a, TD_Vec3* b, TD_Vec3* c){
	return (c->x-a->x) * (b->y-a->y) - (c->y-a->y) * (b->x-a->x);
}

// Check pixel for triangle
TD_FUNC TD_Vec3 TD_triangle_pixel(TD_Vec3* a, TD_Vec3* b, TD_Vec3* c, TD_Vec3 p){
	float area = TD_edge_function(a,b,c);
	float w0 = TD_edge_function(b,c,&p);
	float w1 = TD_edge_function(c,a,&p);
	float w2 = TD_edge_function(a,b,&p);
	if(w0 <= 0 && w1 <= 0 && w2 <= 0){
		w0 /= area;
		w1 /= area;
		w2 /= area;
		return (TD_Vec3){w0,w1,w2};
	}else{
		return TD_Vec3ZERO;
	}
}

// Rasterize a single basic triangle on the screen
TD_FUNC void TD_raster_face(TD_Mesh* m, TD_Face* f, TD_Color (*frag_shader)(TD_ShaderInfo*)){
	TD_Vec3 a = TD_Transform_apply(&m->transform,&m->vertices[f->a]);
	TD_Vec3 b = TD_Transform_apply(&m->transform,&m->vertices[f->b]);
	TD_Vec3 c = TD_Transform_apply(&m->transform,&m->vertices[f->c]);
	TD_Bounds bounds = TD_triangle_bounds(TD_to_screen(&a),TD_to_screen(&b),TD_to_screen(&c));
	for(int y = bounds.ymin; y < bounds.ymax; y++){
		for(int x = bounds.xmin; x < bounds.xmax; x++){
			// Create the info for our shader
			TD_ShaderInfo si;

			// Barycentric coordinates
			si.bc = TD_triangle_pixel(&a,&b,&c,TD_to_world(x,y));

			// Check if they are not zero
			if(!TD_Vec3_cmp(si.bc,TD_Vec3ZERO)){
				// Interpolated position
				si.pos = TD_Vec3_interpolate(&a,&b,&c,&si.bc);

				// Only render pixel if the depth is lesser than the one in depth buffer
				if(si.pos.z >= 0.f && TD_sample_depth(x,y) > si.pos.z){
					// Interpolated other data
					//if(m->colors) ...			- IMPLEMENTED SOON

					// Fragment shader
					if(frag_shader){
						TD_set_pixel(x,y,frag_shader(&si));
					}else
						TD_set_pixel(x,y,(TD_Color){255*si.bc.x,255*si.bc.y,255*si.bc.z});
				}
			}
		}
	}
}

#endif