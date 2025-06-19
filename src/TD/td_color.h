#ifndef TERDIMENSION_COLOR_H
#define TERDIMENSION_COLOR_H

#include "terdimension.h"

// This is a header to handle colors and pixels on the Terminal

// To use RGB colors, define TD_COLOR_RGB

// Otherwise, 4-bit color will be used as default
// If 4-bit color is used, you can disable bright colors
// if they are not supported by defining TD_DISABLE_BRIGHT_COLORS

// To stop using Unicode characters (Top half block)
// define TD_NO_UNICODE before including terdimension.h

// To avoid using colors at all, define TD_NO_COLOR
// Will use characters with different "densities" to
// represent different brightness levels

#ifdef TD_COLOR_RGB
#define TD_SET_FOREGROUND	"\033[38;2;%d;%d;%dm"
#define TD_SET_BACKGROUND	"\033[48;2;%d;%d;%dm"
#endif

#if !defined(TD_COLOR_RGB) && !defined(TD_NO_COLOR)
#define TD_SET_FOREGROUND "\x1b[%dm"
#define TD_SET_BACKGROUND "\x1b[%dm"
#define TD_COLOR_ENTRY(name,r,g,b,fc,gc) (TD_4bit_color){(unsigned char)(r),(unsigned char)(g),(unsigned char)(b),(unsigned char)(fc),(unsigned char)(gc)}
typedef struct{
	unsigned char r, g, b;
	unsigned char fgc, bgc;
} TD_4bit_color;
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
#ifndef TD_DISABLE_BRIGHT_COLORS
	TD_COLOR_ENTRY(Grey,			85,87,83,		90,100),
	TD_COLOR_ENTRY(Bright Red,		239,41,41,		91,101),
	TD_COLOR_ENTRY(Bright Green,	138,226,52,		92,102),
	TD_COLOR_ENTRY(Bright Yellow,	252,233,79,		93,103),
	TD_COLOR_ENTRY(Bright Blue,		50,175,255,		94,104),
	TD_COLOR_ENTRY(Bright Magenta,	173,127,168,	95,105),
	TD_COLOR_ENTRY(Bright Cyan,		52,226,226,		96,106),
	TD_COLOR_ENTRY(Bright Color,	255,255,255,	97,107)
#endif
};
#ifndef TD_DISABLE_BRIGHT_COLORS
#define TD_4bit_colors_count 16
#else
#define TD_4bit_colors_count 9
#endif
TD_FUNC TD_4bit_color* TD_COLOR_GET(TD_Color c){
	TD_4bit_color* best_ptr;
	float biggest_accuracy = 999.f;
	TD_4bit_color* ptr = TD_4bit_colors;
	for(int i = 0; i < TD_4bit_colors_count; i++, ptr++){
		float accuracy = (
			fabs(c.r-ptr->r)+
			fabs(c.g-ptr->g)+
			fabs(c.b-ptr->b)
		);
		if(accuracy < biggest_accuracy){
			biggest_accuracy = accuracy;
			best_ptr = ptr;
		}
	}
	return best_ptr;
}
#endif


#ifdef TD_NO_COLOR
static unsigned char TD_densities[] = " `````....---'':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4V";
#define TD_densities_count (sizeof(TD_densities)/sizeof(unsigned char))
#endif

TD_FUNC void TD_COLOR_printchar(TD_Color* uc, TD_Color* bc){
	// When there is no color
	#ifdef TD_NO_COLOR

	int density;
	if(uc->r || uc->g || uc->b || bc->r || bc->g || bc->b)
		density = (
			((uc->r+bc->r)>>1)*0.2126f +
			((uc->g+bc->g)>>1)*0.7152f +
			((uc->b+bc->b)>>1)*0.0722f
		) / 256.f * (float)(TD_densities_count);
	else
		density = 0;
	putchar(TD_densities[TD_CLAMP(density,0,TD_densities_count)]);

	// 4-Bit color
	#elif !defined(TD_COLOR_RGB)

	// With unicode characters
	#ifndef TD_NO_UNICODE
	printf(
		TD_SET_FOREGROUND
		TD_SET_BACKGROUND
		TD_TOP_HALF_BLOCK,
		TD_COLOR_GET(*uc)->fgc,
		TD_COLOR_GET(*bc)->bgc
	);

	// Whithout unicode characters
	#else

	printf(
		TD_SET_BACKGROUND " ",
		TD_COLOR_GET((TD_Color){(uc->r+bc->r)>>1,(uc->g+bc->g)>>1,(uc->b+bc->b)>>1})->bgc
	);

	#endif

	// RGB color
	#else

	// Change colors and print
	#ifndef TD_NO_UNICODE
	printf(
		TD_SET_FOREGROUND
		TD_SET_BACKGROUND
		TD_TOP_HALF_BLOCK,
		uc->r, uc->g, uc->b,
		bc->r, bc->g, bc->b
	);
	#else
	printf(
		TD_SET_BACKGROUND " ",
		(uc->r+bc->r)>>1,
		(uc->g+bc->g)>>1,
		(uc->b+bc->b)>>1
	);
	#endif

	#endif
}

#endif