#ifndef TERDIMENSION_RENDER_H
#define TERDIMENSION_RENDER_H

#include "td_definitions.h"
#include "td_math.h"
#include <math.h>
#include <stdio.h>

// Constants for rendering
#define TD_TOP_HALF_BLOCK 	"\u2580"
#define TD_CLEAR_COLOR		"\033[0m"
#define TD_CLEAR_TERMINAL	"\e[1;1H\e[2J"
#define TD_HIDE_CURSOR		"\e[?25l"
#define TD_SHOW_CURSOR		"\e[?25h"

// Type for an entry in the 4 bit color palette
#define TD_COLOR_ENTRY(name,r,g,b,fc,gc) (TD_4bit_color){(unsigned char)(r),(unsigned char)(g),(unsigned char)(b),(unsigned char)(fc),(unsigned char)(gc)}
typedef struct{
	unsigned char r, g, b;
	unsigned char fgc, bgc;
} TD_4bit_color;

#define TD_4bit_colors_count 16
extern const TD_4bit_color TD_4bit_colors[TD_4bit_colors_count];

TD_FUNC const TD_4bit_color* TD_get_4bit_color(TD_Color);
TD_FUNC void TD_print_uchar(unsigned char);
TD_FUNC void TD_print4bit_full(unsigned char, unsigned char);
TD_FUNC void TD_print4bit_half(unsigned char);
TD_FUNC void TD_printRGB(bool, TD_Color*);
TD_FUNC void TD_printRGB_full(TD_Color*, TD_Color*);
TD_FUNC void TD_printRGB_half(TD_Color);
TD_FUNC void TD_render_pixel(TD_Color*, TD_Color*);

#endif
