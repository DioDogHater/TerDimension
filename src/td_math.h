#ifndef TERDIMENSION_MATH_H
#define TERDIMENSION_MATH_H

#include "terdimension.h"
#include <math.h>

// Vector constants
#define TD_Vec3ZERO 	(TD_Vec3){0.f,0.f,0.f}
#define TD_Vec3IDENTITY (TD_Vec3){1.f,1.f,1.f}
#define TD_Vec3UP 		(TD_Vec3){0.f,1.f,0.f}
#define TD_Vec3DOWN 	(TD_Vec3){0.f,-1.f,0.f}
#define TD_Vec3LEFT 	(TD_Vec3){-1.f,0.f,0.f}
#define TD_Vec3RIGHT 	(TD_Vec3){1.f,0.f,0.f}
#define TD_Vec3FRONT 	(TD_Vec3){0.f,0.f,1.f}
#define TD_Vec3BACK 	(TD_Vec3){0.f,0.f,-1.f}

//======== Vec3 operations ===========
TD_FUNC TD_Vec3 TD_Vec3_add(TD_Vec3 a, TD_Vec3 b){
	return (TD_Vec3){a.x+b.x,a.y+b.y,a.z+b.z};
}
TD_FUNC TD_Vec3 TD_Vec3_sub(TD_Vec3 a, TD_Vec3 b){
	return (TD_Vec3){a.x-b.x,a.y-b.y,a.z-b.z};
}
TD_FUNC TD_Vec3 TD_Vec3_cross(TD_Vec3 a, TD_Vec3 b){
	return (TD_Vec3){(a.y*b.z)-(a.z*b.y),(a.z*b.x)-(a.x*b.z),(a.x*b.y)-(a.y*b.x)};
}
TD_FUNC float TD_Vec3_dot(TD_Vec3 a, TD_Vec3 b){
	return a.x*b.x+a.y*b.y+a.z*b.z; 
}
TD_FUNC TD_Vec3 TD_Vec3_scale(TD_Vec3 a, float b){
	return (TD_Vec3){a.x*b,a.y*b,a.z*b};
}
TD_FUNC TD_Vec3 TD_Vec3_div(TD_Vec3 a, TD_Vec3 b){
	return (TD_Vec3){a.x/b.x,a.y/b.y,a.z/b.z};
}
TD_FUNC TD_Vec3 TD_Vec3_div_scale(TD_Vec3 a, float b){
	return (TD_Vec3){a.x/b,a.y/b,a.z/b};
}
TD_FUNC float TD_Vec3_magnitude(TD_Vec3 v){
	return sqrt(TD_Vec3_dot(v,v));
}
TD_FUNC TD_Vec3 TD_Vec3_normalize(TD_Vec3 v){
	return TD_Vec3_div_scale(v,TD_Vec3_magnitude(v));
}
TD_FUNC _Bool TD_Vec3_cmp(TD_Vec3 a, TD_Vec3 b){
	return (a.x == b.x && a.y == b.y && a.z == b.z);
}
TD_FUNC void TD_Vec3_print(TD_Vec3 v){
	printf("%g, %g, %g\n",v.x,v.y,v.z);
}
TD_FUNC TD_Vec3 TD_Vec3_interpolate(TD_Vec3* a, TD_Vec3* b, TD_Vec3* c, TD_Vec3* bc){
	return (TD_Vec3){
		a->x*bc->x+b->x*bc->y+c->x*bc->z,
		a->y*bc->x+b->y*bc->y+c->y*bc->z,
		a->z*bc->x+b->z*bc->y+c->z*bc->z
	};
}

//============ Transform ==============
// Default transform
#define TD_TransformIDENTITY (TD_Transform){TD_Vec3ZERO,TD_Vec3ZERO,TD_Vec3IDENTITY}

// Apply rotation to Vec3 in order ZYX
TD_FUNC TD_Vec3 TD_Transform_rotationZYX(TD_Transform* t, TD_Vec3* v){
	TD_Vec3 r = *v;
	float tcos = 0.f, tsin = 0.f;

	// Z rotation
	if(t->rotation.z != 0.f){
		tcos = cos(t->rotation.z);
		tsin = sin(t->rotation.z);
		r.x = v->x * tcos - v->y * tsin;
		r.y = v->x * tsin + v->y * tcos;
	}

	// Y rotation
	if(t->rotation.y != 0.f){
		tcos = cos(t->rotation.y);
		tsin = sin(t->rotation.y);
		r.x = v->x * tcos - v->z * tsin;
		r.z = v->x * tsin + v->z * tcos;
	}

	// X rotation
	if(t->rotation.x != 0.f){
		tcos = cos(t->rotation.x);
		tsin = sin(t->rotation.x);
		r.z = v->z * tcos - v->y * tsin;
		r.y = v->z * tsin + v->y * tcos;
	}

	// Return the result
	return r;
}

TD_FUNC TD_Vec3 TD_Transform_scale(TD_Transform* t, TD_Vec3* v){
	return (TD_Vec3){v->x*t->scale.x,v->y*t->scale.y,v->z*t->scale.z};
}

TD_FUNC TD_Vec3 TD_Transform_apply(TD_Transform* t, TD_Vec3* v){
	TD_Vec3 r = TD_Transform_scale(t,v);
	r = TD_Transform_rotationZYX(t,&r);
	r = TD_Vec3_add(t->position,r);
	return r;
}

#endif