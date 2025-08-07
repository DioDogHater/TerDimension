#ifndef EXAMPLE_MESHES_H
#define EXAMPLE_MESHES_H

#include "TD/td_definitions.h"
#include "TD/terdimension.h"

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
	TD_TextureEMPTY,
	// v = vertex index, c = color / uv index
	// n = normal index
	// {v1,v2,v3, c1,c2,c3, n1,n2,3}
	(TD_Face[]){
		// Back
		{2,1,0, 2,1,0, 0,0,0},
		{2,3,1, 2,3,1, 0,0,0},
		// Front
		{6,5,4, 3,0,1, 1,1,1},
		{6,7,5, 3,2,0, 1,1,1},
		// Left
		{0,1,6, 0,1,3, 2,2,2},
		{7,6,1, 2,3,1, 2,2,2},
		// Right
		{4,3,2, 1,3,2, 3,3,3},
		{5,3,4, 0,3,1, 3,3,3},
		// Top
		{3,7,1, 3,2,1, 4,4,4},
		{3,5,7, 3,0,2, 4,4,4},
		// Bottom
		{0,6,2, 0,3,2, 5,5,5},
		{6,4,2, 3,1,2, 5,5,5}
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
		{-1.f,-1.f,0.f},
		{1.f,-1.f,0.f},
		{-1.f,1.f,0.f},
		{1.f,1.f,0.f}
	},
	NULL,
	NULL,
	(TD_Vec2[]){
		{0.f,0.f},
		{2.f,0.f},
		{0.f,2.f},
		{2.f,2.f}
	},
	TD_TextureEMPTY,
	(TD_Face[]){
		{0,1,2, 0,1,2},
		{1,3,2, 1,3,2}
	},
	2,
	(TD_Transform){
		(TD_Vec3){-2.f,0.f,3.f},
		TD_Vec3ZERO,
		TD_Vec3IDENTITY
	}
};

#endif