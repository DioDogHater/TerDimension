#include "td_math.h"

// Macro creates a function which equates to
// TD_FUNC TD_Vec3 TD_Vec3_(name)(TD_Vec3 a, TD_Vec3 b){
//		return (TD_Vec3) (a (op) b)
// }
#define TD_VEC3_OPER(name,op) \
TD_FUNC TD_Vec3 TD_Vec3_##name(TD_Vec3 a, TD_Vec3 b){\
	return (TD_Vec3){a.x op b.x, a.y op b.y, a.z op b.z};\
}
#define TD_VEC2_OPER(name,op) \
TD_FUNC TD_Vec2 TD_Vec2_##name(TD_Vec2 a, TD_Vec2 b){\
	return (TD_Vec2){a.x op b.x, a.y op b.y};\
}

//======== Vec3 operations ===========
TD_VEC3_OPER(add,+)
TD_VEC3_OPER(sub,-)
TD_VEC3_OPER(div,/)
TD_VEC3_OPER(mult,*)
TD_FUNC TD_Vec3 TD_Vec3_abs(TD_Vec3 v){
	return (TD_Vec3){fabsf(v.x), fabsf(v.y), fabsf(v.z)};
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
TD_FUNC TD_Vec3 TD_Vec3_div_scale(TD_Vec3 a, float b){
	if(b == 0.f) return TD_Vec3ZERO;
	return (TD_Vec3){a.x/b,a.y/b,a.z/b};
}
TD_FUNC float TD_Vec3_magnitude(TD_Vec3 v){
	return sqrt(TD_Vec3_dot(v,v));
}
TD_FUNC float TD_Vec3_distance_squared(TD_Vec3 a, TD_Vec3 b){
	TD_Vec3 v = TD_Vec3_sub(a, b);
	return TD_Vec3_dot(v,v);
}
TD_FUNC TD_Vec3 TD_Vec3_normalize(TD_Vec3 v){
	float m = TD_Vec3_magnitude(v);
	if(m == 0.f)
		return TD_Vec3ZERO;
	return (TD_Vec3){v.x/m, v.y/m, v.z/m};
}
TD_FUNC bool TD_Vec3_cmp(TD_Vec3 a, TD_Vec3 b){
	return (a.x == b.x && a.y == b.y && a.z == b.z);
}

//========= Vec2 operations ===========
TD_VEC2_OPER(add,+)
TD_VEC2_OPER(sub,-)
TD_VEC2_OPER(div,/)
TD_VEC2_OPER(mult,*)
TD_FUNC TD_Vec2 TD_Vec2_scale(TD_Vec2 v, float s){
	return (TD_Vec2){v.x*s,v.y*s};
}
TD_FUNC TD_Vec2 TD_Vec2_div_scale(TD_Vec2 v, float s){
	return (TD_Vec2){v.x/s,v.y/s};
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
	return TD_Vec3_add(t->position,r);
}

//=========== 3D Rendering Maths =============
// Camera transformation
TD_FUNC TD_Vec3 TD_Camera_transform(TD_Vec3* v){
	TD_Vec3 r = TD_Vec3_sub(*v,TD_camera.position);
	return TD_Vec3_rotationZYX(&TD_camera.rotation,&r);
}

// Simple perspective
TD_FUNC TD_Vec3 TD_simple_perspective(TD_Vec3* v){
	if(v->z <= 0) return *v;
	return (TD_Vec3){v->x/v->z,v->y/v->z,v->z};
}
