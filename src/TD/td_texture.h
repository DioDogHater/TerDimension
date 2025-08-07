#ifndef TERDIMENSION_TEXTURE_H
#define TERDIMENSION_TEXTURE_H

// Handle textures and their loading
#include "td_definitions.h"
#include <math.h>

TD_FUNC bool TD_load_texture(char*, TD_Texture*);
TD_FUNC void TD_free_texture(TD_Texture*);
TD_FUNC TD_Color TD_sample_texture(float, float, TD_Texture*);

#endif
