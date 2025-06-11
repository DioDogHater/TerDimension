#ifndef TERDIMENSION_RASTERIZER_H
#define TERDIMENSION_RASTERIZER_H

#include "terdimension.h"
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
TD_FUNC TD_Vec2i TD_to_screen(TD_Vec3* v){
	return (TD_Vec2i){TD_TO_SCREEN_X(v->x),TD_TO_SCREEN_Y(v->y)};
}

// Transform 2D screen coordinates into 2D (actually 3D) world coordinates
#define TD_TO_WORLD_X(x) (((float)(x)+0.5f)-(float)(TD_SW2))/(float)TD_SW*TD_ASPECT_RATIO
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

// Returns if x <= 0 if CCW or x >= 0 if CW
#define TD_CHECK_EDGE(x) ((TD_winding)?((x)>=0):((x)<=0))

// Check pixel for triangle
TD_FUNC TD_Vec3 TD_triangle_pixel(TD_Vec3* a, TD_Vec3* b, TD_Vec3* c, TD_Vec3 p){
	float area = TD_edge_function(a,b,c);
	float w0 = TD_edge_function(b,c,&p);
	float w1 = TD_edge_function(c,a,&p);
	float w2 = TD_edge_function(a,b,&p);
	if(TD_CHECK_EDGE(w0) && TD_CHECK_EDGE(w1) && TD_CHECK_EDGE(w2)){
		w0 /= area;
		w1 /= area;
		w2 /= area;
		return (TD_Vec3){w0,w1,w2};
	}else{
		return TD_Vec3ZERO;
	}
}

// Basically, we check the point in the middle of the triangle
// If it's "outside", we discard the triangle because it's facing away
TD_FUNC _Bool TD_is_backface(TD_Vec3* a, TD_Vec3* b, TD_Vec3* c){
	TD_Vec3 middle = (TD_Vec3){
		(a->x+b->x+c->x)/3.f,
		(a->y+b->y+c->y)/3.f,
		0.f
	};
	return !TD_CHECK_EDGE(TD_edge_function(b,c,&middle));
}

// Render a single face of a mesh on the screen
TD_FUNC void TD_render_face(TD_Mesh* m, TD_Face* f, TD_Shader frag_shader){
	// 3D transformations
	TD_Vec3 a = TD_Transform_apply(&m->transform,&m->vertices[f->a]);
	TD_Vec3 b = TD_Transform_apply(&m->transform,&m->vertices[f->b]);
	TD_Vec3 c = TD_Transform_apply(&m->transform,&m->vertices[f->c]);

	// Apply camera transformation
	a = TD_Camera_transform(&a);
	b = TD_Camera_transform(&b);
	c = TD_Camera_transform(&c);

	// Cull face if it's completely behind camera
	if(a.z <= 0.f || b.z <= 0.f || c.z <= 0.f)
		return;

	// Apply simple perspective
	a = TD_simple_perspective(&a);
	b = TD_simple_perspective(&b);
	c = TD_simple_perspective(&c);

	// Backface culling
	if(TD_is_backface(&a,&b,&c))
		return;

	// Rasterization step
	TD_Bounds bounds = TD_triangle_bounds(TD_to_screen(&a),TD_to_screen(&b),TD_to_screen(&c));

	// If the bounding rect is not valid, dont render
	if(bounds.xmax - bounds.xmin <= 0 || bounds.ymax - bounds.ymin <= 0)
		return;

	// Parse through each pixel where the triangle should be
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
					// Interpolated color (if there is color)
					if(f->d >= 0 && f->e >= 0 && f->f >= 0 && m->colors)
						si.color = TD_Color_interpolate(&m->colors[f->d],&m->colors[f->e],&m->colors[f->f],&si.bc);
					if(f->normal >= 0 && m->normals)
						si.normal = TD_Vec3_rotationZYX(&m->transform.rotation,&m->normals[f->normal]);

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

TD_FUNC void TD_render_mesh(TD_Mesh* m, TD_Shader frag_shader){
	if(m->faces == NULL || m->face_count == 0 || m->vertices == NULL) return;
	for(int i = 0; i < m->face_count; i++){
		TD_render_face(m,&m->faces[i],frag_shader);
	}
}

#endif