#include "td_texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Loads texture using stb_image into dest
TD_FUNC bool TD_load_texture(char* filename, TD_Texture* dest){
	if(!dest) { printf("TD_load_texture: NULL arg\n"); return false; }
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

// Frees / empties the texture pointed by dest
TD_FUNC void TD_free_texture(TD_Texture* dest){
	if(!dest) return;
	if(dest->pixels){
		stbi_image_free(dest->pixels);
		dest->pixels = NULL;
	}
	dest->width = 0;
	dest->height = 0;
}

// Sample a pixel on a texture
TD_FUNC TD_Color TD_sample_texture(float x, float y, TD_Texture* src){
	if(!src || !src->height || !src->width || !src->pixels) return TD_WHITE;
	// Loop the uv coordinates
	x = fmod(x, 1.f);
	y = fmod(y, 1.f);
	int px = (int)floor(x*(float)src->width);
	int py = (int)floor(y*(float)src->height);
	return src->pixels[TD_CLAMP(py,0,src->height)*src->width+TD_CLAMP(px,0,src->width)];

}