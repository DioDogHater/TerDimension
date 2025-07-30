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
		(int)TD_MAX(TD_MIN(a.x,TD_MIN(b.x,c.x)),0),
		(int)TD_MIN(TD_MAX(a.x,TD_MAX(b.x,c.x)),TD_SW),
		(int)TD_MAX(TD_MIN(a.y,TD_MIN(b.y,c.y)),0),
		(int)TD_MIN(TD_MAX(a.y,TD_MAX(b.y,c.y)),TD_SH)
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
	float w0 = TD_edge_function(b,c,&p);
	float w1 = TD_edge_function(c,a,&p);
	float w2 = TD_edge_function(a,b,&p);
	if(TD_CHECK_EDGE(w0) && TD_CHECK_EDGE(w1) && TD_CHECK_EDGE(w2)){
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

TD_FUNC float TD_clip_edge(TD_Vec3* a, TD_Vec3* b){
	float u = (TD_NEAR_CLIP - a->z)/(b->z - a->z);
	*a = (TD_Vec3){
		a->x + (b->x - a->x)*u,
		a->y + (b->y - a->y)*u,
		TD_NEAR_CLIP
	};
	return u;
}

TD_FUNC TD_Color TD_interpolate_2_colors(TD_Color* c1, TD_Color* c2, float u){
	return (TD_Color){
		(unsigned char)(c1->r + (c2->r - c1->r)*(u)),
		(unsigned char)(c1->g + (c2->g - c1->g)*(u)),
		(unsigned char)(c1->b + (c2->b - c1->b)*(u))
	};
}

TD_FUNC TD_Vec2 TD_interpolate_2_vec2(TD_Vec2* a, TD_Vec2* b, float u){
	return (TD_Vec2){
		a->x + (b->x - a->x) * u,
		a->y + (b->y - a->y) * u
	};
}

#define TD_SHOULD_CLIP2(_a, _b, _c) ((_a).z < TD_NEAR_CLIP && (_b).z < TD_NEAR_CLIP && (_c).z >= TD_NEAR_CLIP)
#define TD_SHOULD_CLIP1(_a, _b, _c) ((_a).z < TD_NEAR_CLIP && (_b).z >= TD_NEAR_CLIP && (_c).z >= TD_NEAR_CLIP)

TD_FUNC void TD_render_triangle(TD_Vec3 a, TD_Vec3 b, TD_Vec3 c, TD_Color* colors, TD_Vec2* uvs, TD_Texture* texture, TD_Vec3* normal, TD_Face* f){
	// Cull face if it's partially behind camera
	if(a.z >= TD_NEAR_CLIP && b.z >= TD_NEAR_CLIP && c.z >= TD_NEAR_CLIP);
	else if(a.z < TD_NEAR_CLIP && b.z < TD_NEAR_CLIP && c.z < TD_NEAR_CLIP){
		return;
	}else if(TD_SHOULD_CLIP2(a, b, c)){
		float a_u = TD_clip_edge(&a, &c);
		float b_u = TD_clip_edge(&b, &c);
		TD_Color inter_colors[3];
		TD_Vec2 inter_uvs[3];
		if(colors){
			inter_colors[0] = TD_interpolate_2_colors(&colors[f->d], &colors[f->f], a_u);
			inter_colors[1] = TD_interpolate_2_colors(&colors[f->e], &colors[f->f], b_u);
			inter_colors[2] = colors[f->f];
			colors = inter_colors;
		}
		if(uvs){
			inter_uvs[0] = TD_interpolate_2_vec2(&uvs[f->d], &uvs[f->f], a_u);
			inter_uvs[1] = TD_interpolate_2_vec2(&uvs[f->e], &uvs[f->f], b_u);
			inter_uvs[2] = uvs[f->f];
			uvs = inter_uvs;
		}
		TD_Face new_face = *f;
		new_face.d = 0;
		new_face.e = 1;
		new_face.f = 2;
		TD_render_triangle(a, b, c, colors, uvs, texture, normal, &new_face);
		return;
	}else if(TD_SHOULD_CLIP2(b, c, a)){
		float a_u = TD_clip_edge(&b, &a);
		float b_u = TD_clip_edge(&c, &a);
		TD_Color inter_colors[3];
		TD_Vec2 inter_uvs[3];
		if(colors){
			inter_colors[0] = TD_interpolate_2_colors(&colors[f->e], &colors[f->d], a_u);
			inter_colors[1] = TD_interpolate_2_colors(&colors[f->f], &colors[f->d], b_u);
			inter_colors[2] = colors[f->d];
			colors = inter_colors;
		}
		if(uvs){
			inter_uvs[0] = TD_interpolate_2_vec2(&uvs[f->e], &uvs[f->d], a_u);
			inter_uvs[1] = TD_interpolate_2_vec2(&uvs[f->f], &uvs[f->d], b_u);
			inter_uvs[2] = uvs[f->d];
			uvs = inter_uvs;
		}
		TD_Face new_face = *f;
		new_face.d = 2;
		new_face.e = 0;
		new_face.f = 1;
		TD_render_triangle(a, b, c, colors, uvs, texture, normal, &new_face);
		return;
	}else if(TD_SHOULD_CLIP2(c, a, b)){
		float a_u = TD_clip_edge(&c, &b);
		float b_u = TD_clip_edge(&a, &b);
		TD_Color inter_colors[3];
		TD_Vec2 inter_uvs[3];
		if(colors){
			inter_colors[0] = TD_interpolate_2_colors(&colors[f->f], &colors[f->e], a_u);
			inter_colors[1] = TD_interpolate_2_colors(&colors[f->d], &colors[f->e], b_u);
			inter_colors[2] = colors[f->e];
			colors = inter_colors;
		}
		if(uvs){
			inter_uvs[0] = TD_interpolate_2_vec2(&uvs[f->f], &uvs[f->e], a_u);
			inter_uvs[1] = TD_interpolate_2_vec2(&uvs[f->d], &uvs[f->e], b_u);
			inter_uvs[2] = uvs[f->e];
			uvs = inter_uvs;
		}
		TD_Face new_face = *f;
		new_face.d = 1;
		new_face.e = 2;
		new_face.f = 0;
		TD_render_triangle(a, b, c, colors, uvs, texture, normal, &new_face);
		return;
	}else if(TD_SHOULD_CLIP1(a,b,c)){
		TD_Vec3 d = a;
		float a_u = TD_clip_edge(&a, &b);
		float d_u = TD_clip_edge(&d, &c);
		TD_Color inter_colors[4];
		TD_Vec2 inter_uvs[4];
		if(colors){
			inter_colors[0] = TD_interpolate_2_colors(&colors[f->d], &colors[f->e], a_u);
			inter_colors[1] = TD_interpolate_2_colors(&colors[f->d], &colors[f->f], d_u);
			inter_colors[2] = colors[f->e];
			inter_colors[3] = colors[f->f];
			colors = inter_colors;
		}
		if(uvs){
			inter_uvs[0] = TD_interpolate_2_vec2(&uvs[f->d], &uvs[f->e], a_u);
			inter_uvs[1] = TD_interpolate_2_vec2(&uvs[f->d], &uvs[f->f], d_u);
			inter_uvs[2] = uvs[f->e];
			inter_uvs[3] = uvs[f->f];
			uvs = inter_uvs;
		}
		TD_Face new_face = *f;
		new_face.d = 0;
		new_face.e = 2;
		new_face.f = 3;
		TD_render_triangle(a, b, c, colors, uvs, texture, normal, &new_face);
		new_face.d = 0;
		new_face.e = 3;
		new_face.f = 1;
		TD_render_triangle(a, c, d, colors, uvs, texture, normal, &new_face);
		return;
	}else if(TD_SHOULD_CLIP1(b,c,a)){
		TD_Vec3 d = b;
		float a_u = TD_clip_edge(&b, &c);
		float d_u = TD_clip_edge(&d, &a);
		TD_Color inter_colors[4];
		TD_Vec2 inter_uvs[4];
		if(colors){
			inter_colors[0] = TD_interpolate_2_colors(&colors[f->e], &colors[f->f], a_u);
			inter_colors[1] = TD_interpolate_2_colors(&colors[f->e], &colors[f->d], d_u);
			inter_colors[2] = colors[f->f];
			inter_colors[3] = colors[f->d];
			colors = inter_colors;
		}
		if(uvs){
			inter_uvs[0] = TD_interpolate_2_vec2(&uvs[f->e], &uvs[f->f], a_u);
			inter_uvs[1] = TD_interpolate_2_vec2(&uvs[f->e], &uvs[f->d], d_u);
			inter_uvs[2] = uvs[f->f];
			inter_uvs[3] = uvs[f->d];
			uvs = inter_uvs;
		}
		TD_Face new_face = *f;
		new_face.d = 0;
		new_face.e = 2;
		new_face.f = 3;
		TD_render_triangle(b, c, a, colors, uvs, texture, normal, &new_face);
		new_face.d = 0;
		new_face.e = 3;
		new_face.f = 1;
		TD_render_triangle(b, a, d, colors, uvs, texture, normal, &new_face);
		return;
	}else if(TD_SHOULD_CLIP1(c,a,b)){
		TD_Vec3 d = c;
		float a_u = TD_clip_edge(&c, &a);
		float d_u = TD_clip_edge(&d, &b);
		TD_Color inter_colors[4];
		TD_Vec2 inter_uvs[4];
		if(colors){
			inter_colors[0] = TD_interpolate_2_colors(&colors[f->f], &colors[f->d], a_u);
			inter_colors[1] = TD_interpolate_2_colors(&colors[f->f], &colors[f->e], d_u);
			inter_colors[2] = colors[f->d];
			inter_colors[3] = colors[f->e];
			colors = inter_colors;
		}
		if(uvs){
			inter_uvs[0] = TD_interpolate_2_vec2(&uvs[f->f], &uvs[f->d], a_u);
			inter_uvs[1] = TD_interpolate_2_vec2(&uvs[f->f], &uvs[f->e], d_u);
			inter_uvs[2] = uvs[f->d];
			inter_uvs[3] = uvs[f->e];
			uvs = inter_uvs;
		}
		TD_Face new_face = *f;
		new_face.d = 0;
		new_face.e = 2;
		new_face.f = 3;
		TD_render_triangle(c, a, b, colors, uvs, texture, normal, &new_face);
		new_face.d = 0;
		new_face.e = 3;
		new_face.f = 1;
		TD_render_triangle(c, b, d, colors, uvs, texture, normal, &new_face);
		return;
	}else
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

	float area = TD_edge_function(&a,&b,&c);

	// Parse through each pixel where the triangle should be
	for(int y = bounds.ymin; y < bounds.ymax; y++){
		for(int x = bounds.xmin; x < bounds.xmax; x++){
			// Create the info for our shader
			TD_ShaderInfo si;

			// Get the world space position of current pixel
			si.pos = TD_to_world(x,y);

			// Barycentric coordinates
			si.bc = TD_triangle_pixel(&a,&b,&c,si.pos);

			// Check if they are not zero
			if(!TD_Vec3_cmp(si.bc,TD_Vec3ZERO)){
				// Divide barycentric coordinates with area
				si.bc = TD_Vec3_div_scale(si.bc,area);

				// Interpolated position
				si.pos.z = 1.f / (si.bc.x / a.z + si.bc.y / b.z + si.bc.z / c.z);

				// Only render pixel if the depth is lesser than the one in depth buffer
				if(si.pos.z > 0.f && si.pos.z < TD_sample_depth(x,y)){
					// Interpolated color (if there is color)
					if(f->d >= 0 && f->e >= 0 && f->f >= 0 && colors)
						si.color = (TD_Color){
							(unsigned char)(
								((colors[f->d].r/a.z)*si.bc.x + (colors[f->e].r/b.z)*si.bc.y + (colors[f->f].r/c.z)*si.bc.z) * si.pos.z),
							(unsigned char)(
								((colors[f->d].g/a.z)*si.bc.x + (colors[f->e].g/b.z)*si.bc.y + (colors[f->f].g/c.z)*si.bc.z) * si.pos.z),
							(unsigned char)(
								((colors[f->d].b/a.z)*si.bc.x + (colors[f->e].b/b.z)*si.bc.y + (colors[f->f].b/c.z)*si.bc.z) * si.pos.z),
						};
					else
						si.color = TD_WHITE;

					// Interpolated UV coordinates
					if(f->d >= 0 && f->e >= 0 && f->f >= 0 && uvs){
						si.uv = (TD_Vec2){
							((uvs[f->d].x/a.z)*si.bc.x + (uvs[f->e].x/b.z)*si.bc.y + (uvs[f->f].x/c.z)*si.bc.z) * si.pos.z,
							((uvs[f->d].y/a.z)*si.bc.x + (uvs[f->e].y/b.z)*si.bc.y + (uvs[f->f].y/c.z)*si.bc.z) * si.pos.z
						};
					}else
						si.uv = TD_Vec2ZERO;

					// Give the texture
					if(texture) si.texture = texture;

					// Interpolated normals
					if(normal)
						si.normal = *normal;
					else
						si.normal = TD_Vec3ZERO;

					// Fragment shader
					if(TD_shader){
						TD_set_pixel(x,y,TD_shader(&si));
					}else
						TD_set_pixel(x,y,TD_WHITE);
					TD_set_depth(x,y,si.pos.z);
				}
			}
		}
	}
}

// Render a single face of a mesh on the screen
TD_FUNC void TD_render_face(TD_Mesh* m, TD_Face* f){
	// 3D transformations
	TD_Vec3 a = TD_Transform_apply(&m->transform,&m->vertices[f->a]);
	TD_Vec3 b = TD_Transform_apply(&m->transform,&m->vertices[f->b]);
	TD_Vec3 c = TD_Transform_apply(&m->transform,&m->vertices[f->c]);

	// Apply camera transformation
	a = TD_Camera_transform(&a);
	b = TD_Camera_transform(&b);
	c = TD_Camera_transform(&c);

	// Rotated normal
	TD_Vec3 normal;
	if(m->normals)
		normal = TD_Vec3_rotationZYX(&m->transform.rotation, &m->normals[f->normal]);

	TD_render_triangle(a, b, c, m->colors, m->uvs, m->texture, (m->normals) ? &normal : NULL, f);
}

TD_FUNC void TD_render_mesh(TD_Mesh* m){
	if(m->faces == NULL || m->face_count == 0 || m->vertices == NULL) return;
	for(int i = 0; i < m->face_count; i++){
		TD_render_face(m,&m->faces[i]);
	}
}

#endif
