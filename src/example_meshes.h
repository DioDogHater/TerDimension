#ifndef EXAMPLE_MESHES_H
#define EXAMPLE_MESHES_H

#include "TD/terdimension.h"

// Light source
#define LIGHT_SOURCE (TD_Vec3){10.f,10.f,-10.f}
#define DIFFUSE 0.8f
#define AMBIENT 0.2f

// Fragment shaders
TD_Color diffuse_lighting_shader(TD_ShaderInfo* si){
	TD_Vec3 light_dir = TD_Vec3_normalize(TD_Vec3_sub(LIGHT_SOURCE,si->pos));
	float diffuse = TD_MAX(TD_Vec3_dot(TD_Vec3_normalize(si->normal),light_dir),0.f);
	return (TD_Color){
		(unsigned char)((DIFFUSE*diffuse+AMBIENT)*si->color.r),
		(unsigned char)((DIFFUSE*diffuse+AMBIENT)*si->color.g),
		(unsigned char)((DIFFUSE*diffuse+AMBIENT)*si->color.b)
	};
}

TD_Color texture_shader(TD_ShaderInfo* si){
	return TD_sample_texture(si->uv.x,si->uv.y,si->texture);
}

// Texture of lebron's divine face
TD_Texture lebron_texture = TD_TextureEMPTY;

// Hard coded cube mesh
TD_Mesh multicolor_cube = (TD_Mesh){
	(TD_Vec3[]){
		{-0.5f,-0.5f,-0.5f},
		{-0.5f,0.5f,-0.5f},
		{0.5f,-0.5f,-0.5f},
		{0.5f,0.5f,-0.5f},
		{0.5f,-0.5f,0.5f},
		{0.5f,0.5f,0.5f},
		{-0.5f,-0.5f,0.5f},
		{-0.5f,0.5f,0.5f}
	},
	(TD_Color[]){
		TD_RED,
		TD_BLUE,
		TD_GREEN,
		TD_WHITE
	},
	(TD_Vec3[]){
		TD_Vec3BACK,
		TD_Vec3FRONT,
		TD_Vec3LEFT,
		TD_Vec3RIGHT,
		TD_Vec3UP,
		TD_Vec3DOWN
	},
	NULL,
	NULL,
	// v = vertex index, c = color / uv index
	// {v1,v2,v3, c1,c2,c3, normal index}
	(TD_Face[]){
		// Back
		{2,1,0, 2,1,0, 0},
		{2,3,1, 2,3,1, 0},
		// Front
		{6,5,4, 3,0,1, 1},
		{6,7,5, 3,2,0, 1},
		// Left
		{0,1,6, 0,1,3, 2},
		{7,6,1, 2,3,1, 2},
		// Right
		{4,3,2, 1,3,2, 3},
		{5,3,4, 0,3,1, 3},
		// Top
		{3,7,1, 3,2,1, 4},
		{3,5,7, 3,0,2, 4},
		// Bottom
		{0,6,2, 0,3,2, 5},
		{6,4,2, 3,1,2, 5}
	},
	12,
	(TD_Transform){
		(TD_Vec3){0.f,0.f,5.f},
		TD_Vec3ZERO,
		TD_Vec3IDENTITY
	}
};

TD_Mesh textured_plane = (TD_Mesh){
	(TD_Vec3[]){
		{-0.5f,-0.5f,0.f},
		{0.5f,-0.5f,0.f},
		{-0.5f,0.5f,0.f},
		{0.5f,0.5f,0.f}
	},
	NULL,
	NULL,
	(TD_Vec2[]){
		{0.f,0.f},
		{2.f,0.f},
		{0.f,2.f},
		{2.f,2.f}
	},
	&lebron_texture,
	(TD_Face[]){
		{0,1,2, 0,1,2},
		{1,3,2, 1,3,2}
	},
	2,
	(TD_Transform){
		(TD_Vec3){0.f,-2.f,2.f},
		(TD_Vec3){TD_to_rad(90.f),0.f,0.f},
		(TD_Vec3){10.f,10.f,0.f}
	}
};

#endif
