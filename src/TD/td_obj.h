#ifndef TERDIMENSION_OBJ_H
#define TERDIMENSION_OBJ_H

// Loads obj meshes using fast_obj.h
// Thanks thisistherk, you are very helpful

#include "td_definitions.h"

#ifndef TD_NO_TEXTURES
#include "td_texture.h"
#endif

TD_FUNC bool TD_load_obj(const char*, TD_Mesh*);
TD_FUNC void TD_free_obj(TD_Mesh*);

#endif
