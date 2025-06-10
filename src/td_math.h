#ifndef TERDIMENSION_MATH_H
#define TERDIMENSION_MATH_H

#include "terdimension.h"
#include <math.h>

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
	if(b == 0.f) return TD_Vec3ZERO;
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
TD_FUNC TD_Color TD_Color_interpolate(TD_Color* a, TD_Color* b, TD_Color* c, TD_Vec3* bc){
	return (TD_Color){
		(unsigned char)(a->r*bc->x+b->r*bc->y+c->r*bc->z),
		(unsigned char)(a->g*bc->x+b->g*bc->y+c->g*bc->z),
		(unsigned char)(a->b*bc->x+b->b*bc->y+c->b*bc->z)
	};
}

//============ Transform ==============

// Apply rotation to Vec3 in order ZYX
TD_FUNC TD_Vec3 TD_Vec3_rotationZYX(TD_Vec3* rot, TD_Vec3* v){
	TD_Vec3 r = *v;
	float tcos = 0.f, tsin = 0.f;

	// Z rotation
	if(rot->z != 0.f){
		tcos = cos(rot->z);
		tsin = sin(rot->z);
		float tmp = r.x * tcos - r.y * tsin;
		r.y =		r.x * tsin + r.y * tcos;
		r.x = tmp;
	}

	// Y rotation
	if(rot->y != 0.f){
		tcos = cos(rot->y);
		tsin = sin(rot->y);
		float tmp = r.x * tcos - r.z * tsin;
		r.z =		r.x * tsin + r.z * tcos;
		r.x = tmp;
	}

	// X rotation
	if(rot->x != 0.f){
		tcos = cos(rot->x);
		tsin = sin(rot->x);
		float tmp = r.y * tcos - r.z * tsin;
		r.z =		r.y * tsin + r.z * tcos;
		r.y = tmp;
	}

	// Return the result
	return r;
}

TD_FUNC TD_Vec3 TD_Vec3_rotationXYZ(TD_Vec3* rot, TD_Vec3* v){
	TD_Vec3 r = *v;
	float tcos = 0.f, tsin = 0.f;

	// X rotation
	if(rot->x != 0.f){
		tcos = cos(rot->x);
		tsin = sin(rot->x);
		float tmp = r.y * tcos - r.z * tsin;
		r.z =		r.y * tsin + r.z * tcos;
		r.y = tmp;
	}

	// Y rotation
	if(rot->y != 0.f){
		tcos = cos(rot->y);
		tsin = sin(rot->y);
		float tmp = r.x * tcos - r.z * tsin;
		r.z =		r.x * tsin + r.z * tcos;
		r.x = tmp;
	}

	// Z rotation
	if(rot->z != 0.f){
		tcos = cos(rot->z);
		tsin = sin(rot->z);
		float tmp = r.x * tcos - r.y * tsin;
		r.y =		r.x * tsin + r.y * tcos;
		r.x = tmp;
	}

	// Return the result
	return r;

}

TD_FUNC TD_Vec3 TD_Transform_scale(TD_Vec3* s, TD_Vec3* v){
	return (TD_Vec3){v->x*s->x,v->y*s->y,v->z*s->z};
}

TD_FUNC TD_Vec3 TD_Transform_apply(TD_Transform* t, TD_Vec3* v){
	TD_Vec3 r = TD_Transform_scale(&t->scale,v);
	r = TD_Vec3_rotationZYX(&t->rotation,&r);
	return TD_Vec3_add(t->position,r);;
}

//=========== 3D Rendering Maths =============
// Camera transformation
TD_FUNC TD_Vec3 TD_Camera_transform(TD_Vec3* v){
	TD_Vec3 r = TD_Vec3_sub(*v,TD_camera.position);
	return TD_Vec3_rotationXYZ(&TD_camera.rotation,&r);
}

// Simple perspective
TD_FUNC TD_Vec3 TD_simple_perspective(TD_Vec3* v){
	if(v->z <= 0) return *v;
	return (TD_Vec3){v->x/v->z,v->y/v->z,v->z};
}

#endif