#ifndef TERDIMENSION_RENDER_H
#define TERDIMENSION_RENDER_H

#include "td_definitions.h"
#include <math.h>

#define TD_SET_4B_FOREGROUND "\x1b[%dm"
#define TD_SET_4B_BACKGROUND "\x1b[%dm"

#define TD_SET_RGB_FOREGROUND "\033[38;2;%d;%d;%dm"
#define TD_SET_RGB_BACKGROUND "\033[48;2;%d;%d;%dm"

#define TD_COLOR_ENTRY(name,r,g,b,fc,gc) (TD_4bit_color){(unsigned char)(r),(unsigned char)(g),(unsigned char)(b),(unsigned char)(fc),(unsigned char)(gc)}
typedef struct{
	unsigned char r, g, b;
	unsigned char fgc, bgc;
} TD_4bit_color;

// 4 Bit color entry
// (Name, Visible RGB color, Foreground code, Background code)

static TD_4bit_color TD_4bit_colors[] = 
{
	TD_COLOR_ENTRY(Black,			0,0,0,			30,40),
	TD_COLOR_ENTRY(Red,				204,0,0,		31,41),
	TD_COLOR_ENTRY(Green,			78,154,6,		32,42),
	TD_COLOR_ENTRY(Yellow,			196,160,0,		33,43),
	TD_COLOR_ENTRY(Blue,			114,159,207,	34,44),
	TD_COLOR_ENTRY(Magenta,			117,80,123,		35,45),
	TD_COLOR_ENTRY(Cyan,			6,152,154,		36,46),
	TD_COLOR_ENTRY(White,			211,215,207,	37,47),
	TD_COLOR_ENTRY(Grey,			85,87,83,		90,100),
	TD_COLOR_ENTRY(Bright Red,		239,41,41,		91,101),
	TD_COLOR_ENTRY(Bright Green,	138,226,52,		92,102),
	TD_COLOR_ENTRY(Bright Yellow,	252,233,79,		93,103),
	TD_COLOR_ENTRY(Bright Blue,		50,175,255,		94,104),
	TD_COLOR_ENTRY(Bright Magenta,	173,127,168,	95,105),
	TD_COLOR_ENTRY(Bright Cyan,		52,226,226,		96,106),
	TD_COLOR_ENTRY(Bright Color,	255,255,255,	97,107)
};
#define TD_4bit_colors_count 16

TD_FUNC TD_4bit_color* TD_COLOR_GET(TD_Color c){
	TD_4bit_color* best_ptr;
	unsigned int biggest_accuracy = 999.f;
	TD_4bit_color* ptr = TD_4bit_colors;
	for(int i = 0; i < TD_4bit_colors_count; i++, ptr++){
		if(ptr->fgc >= 90 && !(TD_render_flags & TD_RENDER_BRIGHT_COLORS))
			continue;
		unsigned int accuracy = sqrt(
			abs(c.r-ptr->r)+
			abs(c.g-ptr->g)+
			abs(c.b-ptr->b)
		);
		if(accuracy < biggest_accuracy){
			biggest_accuracy = accuracy;
			best_ptr = ptr;
		}
	}
	return best_ptr;
}

static unsigned char TD_densities[] = " `````....---'':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4V";
#define TD_densities_count (sizeof(TD_densities)-1)

TD_FUNC void TD_render_pixel(TD_Color* uc, TD_Color* bc){
	// 4-Bit color
	if(TD_render_flags & TD_RENDER_COLOR){
		// With unicode characters
		if(TD_render_flags & TD_RENDER_UNICODE){
			printf(
				TD_SET_4B_FOREGROUND
				TD_SET_4B_BACKGROUND
				TD_TOP_HALF_BLOCK,
				TD_COLOR_GET(*uc)->fgc,
				TD_COLOR_GET(*bc)->bgc
			);
		}
		// Without unicode characters
		else{
			printf(
				TD_SET_4B_BACKGROUND " ",
				TD_COLOR_GET((TD_Color){
					(unsigned char)(((unsigned short)uc->r+(unsigned short)bc->r)>>1),
					(unsigned char)(((unsigned short)uc->g+(unsigned short)bc->g)>>1),
					(unsigned char)(((unsigned short)uc->b+(unsigned short)bc->b)>>1)
				})->bgc
			);
		}
	}
	// RGB color
	else if(TD_render_flags & TD_RENDER_RGB){
		// Change colors and print
		if(TD_render_flags & TD_RENDER_UNICODE)
			printf(
				TD_SET_RGB_FOREGROUND
				TD_SET_RGB_BACKGROUND
				TD_TOP_HALF_BLOCK,
				uc->r, uc->g, uc->b,
				bc->r, bc->g, bc->b
			);
		else{
			printf(
				TD_SET_RGB_BACKGROUND " ",
				(uc->r+bc->r)>>1,
				(uc->g+bc->g)>>1,
				(uc->b+bc->b)>>1
			);
		}
	}
	// No color
	else{
		int density;
		if(uc->r || uc->g || uc->b || bc->r || bc->g || bc->b)
			density = (
				((uc->r+bc->r)>>1)*0.2126f +
				((uc->g+bc->g)>>1)*0.7152f +
				((uc->b+bc->b)>>1)*0.0722f
			) / 300.f * (float)(TD_densities_count);
		else
			density = 0.f;
		putchar(TD_densities[TD_CLAMP(density,0,TD_densities_count)]);
	}
}

#endif
