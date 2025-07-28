#ifndef TERDIMENSION_TEXTURE_H
#define TERDIMENSION_TEXTURE_H

// Handle textures and their loading
#include "td_definitions.h"
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

// Loads texture using stb_image
TD_FUNC _Bool TD_load_texture(char* filename, TD_Texture* dest){
	int width, height, channels = 3;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* img = stbi_load(filename,&width,&height,&channels,3);
	if(!img){
		TD_LOG("Failed to load texture %s\n",filename);
		return 0;
	}
	dest->width = width;
	dest->height = height;
	if(dest->pixels)
		stbi_image_free(dest->pixels);
	dest->pixels = (TD_Color*) img;
	return 1;
}

// Frees / empties a texture
TD_FUNC void TD_free_texture(TD_Texture* dest){
	if(dest->pixels){
		stbi_image_free(dest->pixels);
		dest->pixels = NULL;
	}
	dest->width = 0;
	dest->height = 0;
}

// Sample a pixel on a texture
TD_FUNC TD_Color TD_sample_texture(float x, float y, TD_Texture* src){
	if(!src) return TD_BLACK;
	// Loop the uv coordinates
	x = fmod(x, 1.f);
	y = fmod(y, 1.f);
	int px = (int)(x*(float)src->width+0.5f);
	int py = (int)(y*(float)src->height+0.5f);
	return src->pixels[py*src->width+px];

}

#endif
