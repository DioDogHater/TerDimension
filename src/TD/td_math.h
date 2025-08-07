#ifndef TERDIMENSION_MATH_H
#define TERDIMENSION_MATH_H

#include "td_definitions.h"
#include <math.h>

#define TD_to_deg(r) ((r)/3.1415f*180.f)
#define TD_to_rad(d) ((d)/180.f*3.1415f)

TD_FUNC TD_Vec3 TD_Vec3_add(TD_Vec3, TD_Vec3);
TD_FUNC TD_Vec3 TD_Vec3_sub(TD_Vec3, TD_Vec3);
TD_FUNC TD_Vec3 TD_Vec3_mult(TD_Vec3, TD_Vec3);
TD_FUNC TD_Vec3 TD_Vec3_div(TD_Vec3, TD_Vec3);
TD_FUNC TD_Vec3 TD_Vec3_abs(TD_Vec3);
TD_FUNC TD_Vec3 TD_Vec3_cross(TD_Vec3, TD_Vec3);
TD_FUNC float TD_Vec3_dot(TD_Vec3, TD_Vec3);
TD_FUNC TD_Vec3 TD_Vec3_scale(TD_Vec3, float);
TD_FUNC TD_Vec3 TD_Vec3_div_scale(TD_Vec3, float);
TD_FUNC float TD_Vec3_magnitude(TD_Vec3);
TD_FUNC float TD_Vec3_distance_squared(TD_Vec3, TD_Vec3);
TD_FUNC TD_Vec3 TD_Vec3_normalize(TD_Vec3);
TD_FUNC bool TD_Vec3_cmp(TD_Vec3, TD_Vec3);

#define TD_Vec3FORMAT "%g %g %g"
#define TD_Vec3ARGS(v) v.x, v.y, v.z

TD_FUNC TD_Vec2 TD_Vec2_add(TD_Vec2, TD_Vec2);
TD_FUNC TD_Vec2 TD_Vec2_sub(TD_Vec2, TD_Vec2);
TD_FUNC TD_Vec2 TD_Vec2_mult(TD_Vec2, TD_Vec2);
TD_FUNC TD_Vec2 TD_Vec2_div(TD_Vec2, TD_Vec2);
TD_FUNC TD_Vec2 TD_Vec2_scale(TD_Vec2, float);
TD_FUNC TD_Vec2 TD_Vec2_div_scale(TD_Vec2, float);

TD_FUNC TD_Vec3 TD_Vec3_rotationZYX(TD_Vec3*, TD_Vec3*);
TD_FUNC TD_Vec3 TD_Vec3_rotationXYZ(TD_Vec3*, TD_Vec3*);

TD_FUNC TD_Vec3 TD_Transform_scale(TD_Vec3*, TD_Vec3*);
TD_FUNC TD_Vec3 TD_Transform_apply(TD_Transform*, TD_Vec3*);

TD_FUNC TD_Vec3 TD_Camera_transform(TD_Vec3*);
TD_FUNC TD_Vec3 TD_simple_perspective(TD_Vec3*);

#endif
