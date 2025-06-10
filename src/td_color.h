#ifndef TERDIMENSION_COLOR_H
#define TERDIMENSION_COLOR_H

#include "terdimension.h"

// This is a header to handle colors on the Terminal
// To use RGB colors, define TD_COLOR_RGB
// Otherwise, 4-bit color will be used as default

// Function prototype
TD_FUNC void TD_COLOR_printchar(TD_Color*,TD_Color*);

// RGB color implementation
#ifdef TD_COLOR_RGB

#define TD_SET_FOREGROUND	"\033[38;2;%d;%d;%dm"
#define TD_SET_BACKGROUND	"\033[48;2;%d;%d;%dm"

TD_FUNC void TD_COLOR_printchar(TD_Color* uc, TD_Color* bc){
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
}

#else

// 4-bit color implementation
// Ubuntu terminal color palette is used
// Change this to adjust to your terminal
// Please check out this image as a reference:
// https://i.sstatic.net/9UVnC.png

// To disable non-standard bright colors, Uncomment out this line:
//#define TD_DISABLE_BRIGHT_COLORS

// Macro to enter colors faster
// (c) : The FG
// NOTE: (name) just points out the color's name, nothing else
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

TD_FUNC TD_4bit_color* TD_COLOR_GET(TD_Color* c){
	TD_4bit_color* best_ptr;
	float biggest_accuracy = 999.f;
	TD_4bit_color* ptr = TD_4bit_colors;
	for(int i = 0; i < TD_4bit_colors_count; i++, ptr++){
		float accuracy = (
			fabs(c->r-ptr->r)+
			fabs(c->g-ptr->g)+
			fabs(c->b-ptr->b)
		);
		if(accuracy < biggest_accuracy){
			biggest_accuracy = accuracy;
			best_ptr = ptr;
		}
	}
	return best_ptr;
}

#define TD_SET_FOREGROUND "\x1b[%dm"
#define TD_SET_BACKGROUND "\x1b[%dm"

TD_FUNC void TD_COLOR_printchar(TD_Color* uc, TD_Color* bc){
	// Change colors and print
	#ifndef TD_NO_UNICODE
	printf(
		TD_SET_FOREGROUND
		TD_SET_BACKGROUND
		TD_TOP_HALF_BLOCK,
		TD_COLOR_GET(uc)->fgc,
		TD_COLOR_GET(bc)->bgc
	);
	#else
	TD_Color c = (TD_Color){(uc->r+bc->r)>>1,(uc->g+bc->g)>>1,(uc->b+bc->b)>>1};
	printf(
		TD_SET_BACKGROUND " ",
		TD_COLOR_GET(&c)->bgc
	);
	#endif
}

#endif

#endif