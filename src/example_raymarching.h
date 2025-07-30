#ifndef EXAMPLE_RAYMARCHING_H
#define EXAMPLE_RAYMARCHING_H

#include "TD/terdimension.h"
#include <math.h>

extern float time;

// iquilezles.org/articles/smin/
float smin( float a, float b, float k ){
	k *= 4.0;
	float h = TD_MAX( k-fabsf(a-b), 0.0 )/k;
	return TD_MIN(a,b) - h*h*k*(1.0/4.0);
}

float raymarch_map(TD_Vec3 p){
	float sphere = TD_Vec3_magnitude(p) - 1.f+sin(time*0.25f)*0.5f;

	// iquilezles.org/articles/distfunctions/
	TD_Vec3 rotation = (TD_Vec3){time, 0.f, time*0.5f};
	TD_Vec3 q = TD_Vec3_rotationZYX(&rotation, &p);
	q = TD_Vec3_abs(TD_Vec3_sub(q, (TD_Vec3){sin(time)*2.f,cos(time*0.5f)*0.25f,0.f}));
	float octahedron = (q.x + q.y + q.z - 0.75f)*0.57735f;

	return smin(sphere, octahedron, 0.1f);
}

TD_Color raymarch_shader(TD_ShaderInfo* si){
	TD_Vec3 ro = (TD_Vec3){0.f,0.f,-5.f};
	TD_Vec3 rd = TD_Vec3_normalize((TD_Vec3){si->uv.x-0.5f,si->uv.y-0.5f,1.f});

	float t = 0.f;

	for(unsigned int i = 0; i < 40; i++){
		TD_Vec3 p = TD_Vec3_add(ro, TD_Vec3_scale(rd, t));
		float d = raymarch_map(p);
		t += d;
		if(d < 0.01f)
			return (TD_Color){(unsigned char)(t * 16.f + i * 8.f), (unsigned char)(t * 16.f + i * 8.f), (unsigned char)(t * 16.f + i * 8.f)};
		else if(d > 10.f)
			return TD_WHITE;
	}
	return TD_WHITE;
}

TD_Mesh raymarch_plane = (TD_Mesh){
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
		{1.f,0.f},
		{0.f,1.f},
		{1.f,1.f}
	},
	NULL,
	(TD_Face[]){
		{0,1,2, 0,1,2},
		{1,3,2, 1,3,2},
		{2,1,0, 2,1,0},
		{2,3,1, 2,3,1}
	},
	4,
	(TD_Transform){
		(TD_Vec3){-2.f,0.f,3.f},
		TD_Vec3ZERO,
		TD_Vec3IDENTITY
	}
};

#endif
