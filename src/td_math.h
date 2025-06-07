#ifndef TERDIMENSION_MATH_H
#define TERDIMENSION_MATH_H

#include "terdimension.h"
#include <math.h>

// Vector filled with 0
#define TD_Vec3ZERO (TD_Vec3){0.f,0.f,0.f}

//======== Vec3 operations ===========
TD_Vec3 TD_Vec3_add(TD_Vec3 a, TD_Vec3 b){
	return (TD_Vec3){a.x+b.x,a.y+b.y,a.z+b.z};
}
TD_Vec3 TD_Vec3_sub(TD_Vec3 a, TD_Vec3 b){
	return (TD_Vec3){a.x-b.x,a.y-b.y,a.z-b.z};
}
TD_Vec3 TD_Vec3_cross(TD_Vec3 a, TD_Vec3 b){
	return (TD_Vec3){a.x*b.x,a.y*b.y,a.z*b.z};
}
float TD_Vec3_dot(TD_Vec3 a, TD_Vec3 b){
	return a.x*b.x+a.y*b.y+a.z*b.z; 
}
TD_Vec3 TD_Vec3_scale(TD_Vec3 a, float b){
	return (TD_Vec3){a.x*b,a.y*b,a.z*b};
}
TD_Vec3 TD_Vec3_div(TD_Vec3 a, TD_Vec3 b){
	return (TD_Vec3){a.x/b.x,a.y/b.y,a.z/b.z};
}
TD_Vec3 TD_Vec3_div_scale(TD_Vec3 a, float b){
	return (TD_Vec3){a.x/b,a.y/b,a.z/b};
}
float TD_Vec3_magnitude(TD_Vec3 v){
	return sqrt(TD_Vec3_dot(v,v));
}
TD_Vec3 TD_Vec3_normalize(TD_Vec3 v){
	return TD_Vec3_div_scale(v,TD_Vec3_magnitude(v));
}
_Bool TD_Vec3_cmp(TD_Vec3 a, TD_Vec3 b){
	return (a.x == b.x && a.y == b.y && a.z == b.z);
}
void TD_Vec3_print(TD_Vec3 v){
	printf("%g, %g, %g\n",v.x,v.y,v.z);
}

//============ Transform ==============
#define TD_TransformZERO (TD_Transform){TD_Vec3ZERO,TD_Vec3ZERO,TD_Vec3ZERO};

#endif