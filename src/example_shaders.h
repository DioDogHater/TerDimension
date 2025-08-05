#ifndef EXAMPLE_SHADERS_H
#define EXAMPLE_SHADERS_H

#include "TD/terdimension.h"

// Light source
#define LIGHT_SOURCE (TD_Vec3){10.f,10.f,-10.f}
#define DIFFUSE 0.8f
#define AMBIENT 0.2f

// Fragment shaders
TD_Color diffuse_color_shader(TD_ShaderInfo* si){
	TD_Vec3 light_dir = TD_Vec3_normalize(TD_Vec3_sub(LIGHT_SOURCE,si->pos));
	float diffuse = TD_MAX(TD_Vec3_dot(TD_Vec3_normalize(si->normal),light_dir),0.f);
	return (TD_Color){
		(unsigned char)((DIFFUSE*diffuse+AMBIENT) * (float)si->color.r),
		(unsigned char)((DIFFUSE*diffuse+AMBIENT) * (float)si->color.g),
		(unsigned char)((DIFFUSE*diffuse+AMBIENT) * (float)si->color.b)
	};
}

TD_Color diffuse_texture_shader(TD_ShaderInfo* si){
	TD_Vec3 light_dir = TD_Vec3_normalize(TD_Vec3_sub(LIGHT_SOURCE,si->pos));
	float diffuse = TD_MAX(TD_Vec3_dot(TD_Vec3_normalize(si->normal),light_dir),0.f);
	TD_Color t_color = TD_sample_texture(si->uv.x, si->uv.y, si->texture);
	return (TD_Color){
		(unsigned char)((DIFFUSE*diffuse+AMBIENT) * (float)t_color.r),
		(unsigned char)((DIFFUSE*diffuse+AMBIENT) * (float)t_color.g),
		(unsigned char)((DIFFUSE*diffuse+AMBIENT) * (float)t_color.b)
	};
}

TD_Color texture_shader(TD_ShaderInfo* si){
	return TD_sample_texture(si->uv.x,si->uv.y,si->texture);
}

#endif
