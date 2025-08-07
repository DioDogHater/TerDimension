#include "td_render.h"

// Unicode character for Top Half block (a single "pixel")
static const char* TD_TOP_HALF_BLOCK_str = TD_TOP_HALF_BLOCK;

// 4 bit color palette, determines what colors are available and what colors
// they represent on your screen.
// (Name, Visible RGB color, Foreground code, Background code)

// To enter out your own color palette, just replace the RGB values below
// For further reference: https://en.m.wikipedia.org/wiki/ANSI_escape_code#Colors
const TD_4bit_color TD_4bit_colors[TD_4bit_colors_count] =
{
	// Regular colors
	TD_COLOR_ENTRY(Black,			0,0,0,			30,40),
	TD_COLOR_ENTRY(Red,				205,0,0,		31,41),
	TD_COLOR_ENTRY(Green,			0,205,0,		32,42),
	TD_COLOR_ENTRY(Yellow,			205,205,0,		33,43),
	TD_COLOR_ENTRY(Blue,			10,0,238,		34,44),
	TD_COLOR_ENTRY(Magenta,			205,0,205,		35,45),
	TD_COLOR_ENTRY(Cyan,			0,205,205,		36,46),
	TD_COLOR_ENTRY(White,			229,229,229,	37,47),

	// Bright colors
	TD_COLOR_ENTRY(Grey,			127,127,127,	90,100),
	TD_COLOR_ENTRY(Bright Red,		255,0,0,		91,101),
	TD_COLOR_ENTRY(Bright Green,	0,255,0,		92,102),
	TD_COLOR_ENTRY(Bright Yellow,	255,255,0,		93,103),
	TD_COLOR_ENTRY(Bright Blue,		92,92,255,		94,104),
	TD_COLOR_ENTRY(Bright Magenta,	255,0,255,		95,105),
	TD_COLOR_ENTRY(Bright Cyan,		0,255,255,		96,106),
	TD_COLOR_ENTRY(Bright White,	255,255,255,	97,107)
};

/*
TD_FUNC TD_Vec3 TD_get_color_hsv(TD_Color* c){
	float r = c->r / 255.f, g = c->g /255.f, b = c->b /255.f;
	float cmax = TD_MAX(r,TD_MAX(g,b));
	float cmin = TD_MIN(r,TD_MIN(g,b));
	float delta = cmax - cmin;

	TD_Vec3 hsv;

	if(delta == 0){
		return (TD_Vec3){0,0,cmax};
	}else if(cmax == r)
		hsv.x = fmodf((g-b)/delta,6.f);
	else if(cmax == g)
		hsv.x = (b-r)/delta+2;
	else
		hsv.x = (r-g)/delta+4;

	hsv.y = delta / cmax;

	hsv.z = cmax;

	return hsv;
}
*/

// Get the visually closest color in the 4 bit color palette
TD_FUNC const TD_4bit_color* TD_get_4bit_color(TD_Color c){
	const TD_4bit_color* best_ptr = TD_4bit_colors;
	float lowest_diff = 1e18f;

	const TD_4bit_color* ptr = TD_4bit_colors;
	for(int i = 0; i < TD_4bit_colors_count; i++, ptr++){
		if(ptr->fgc >= 90 && !(TD_render_flags & TD_RENDER_BRIGHT_COLORS))
			continue;

		float diff = TD_SQUARE((c.r*1.1f - ptr->r)*0.3f) + TD_SQUARE((c.g*1.1f - ptr->g)*0.59f) + TD_SQUARE((c.b*1.1f - ptr->b)*0.22f);

		if(diff < lowest_diff){
			lowest_diff = diff;
			best_ptr = ptr;
		}
	}

	return best_ptr;
}

// An array of ASCII characters, sorted by "density" (brightness)
static const unsigned char TD_char_densities[] = " `````````.........-------'''''':::::____,,,^^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4V";
#define TD_char_densities_count (sizeof(TD_char_densities)-1)

// Prints out an unsigned char as a number
TD_FUNC void TD_print_uchar(unsigned char c){
	if(c >= 100){
		putchar_unlocked('0'+(c/100));
		c %= 100;
		if(c < 10) putchar_unlocked('0');
	}
	if(c >= 10){
		putchar_unlocked('0'+(c/10));
		c %= 10;
	}
	putchar_unlocked('0'+(c));
}

// Prints 2 1x1 pixels vertically with 4 bit colors
TD_FUNC void TD_print4bit_full(unsigned char fgc, unsigned char bgc){
	putchar_unlocked('\x1b');
	putchar_unlocked('[');
	TD_print_uchar(fgc);
	putchar_unlocked('m');
	putchar_unlocked('\x1b');
	putchar_unlocked('[');
	TD_print_uchar(bgc);
	putchar_unlocked('m');
	for(const char* unicode = TD_TOP_HALF_BLOCK_str; *unicode; unicode++){
		putchar_unlocked(*unicode);
	}
}

// Prints 1 1x2 pixel with 4 bit colors
TD_FUNC void TD_print4bit_half(unsigned char bgc){
	putchar_unlocked('\x1b');
	putchar_unlocked('[');
	TD_print_uchar(bgc);
	putchar_unlocked('m');
	putchar_unlocked(' ');
}

// Prints an ANSI escape code to change foreground / background color
TD_FUNC void TD_printRGB(bool foreground, TD_Color* c){
	putchar_unlocked('\x1b');
	putchar_unlocked('[');
	putchar_unlocked(foreground ? '3' : '4');
	putchar_unlocked('8');
	putchar_unlocked(';');
	putchar_unlocked('2');
	putchar_unlocked(';');
	TD_print_uchar(c->r);
	putchar_unlocked(';');
	TD_print_uchar(c->g);
	putchar_unlocked(';');
	TD_print_uchar(c->b);
	putchar_unlocked('m');
}

// Prints 2 1x1 pixels vertically using RGB color
TD_FUNC void TD_printRGB_full(TD_Color* uc, TD_Color* bc){
	TD_printRGB(true, uc);
	TD_printRGB(false, bc);
	for(const char* unicode = TD_TOP_HALF_BLOCK_str; *unicode; unicode++){
		putchar_unlocked(*unicode);
	}
}

// Prints 1 1x2 pixel using RGB color
TD_FUNC void TD_printRGB_half(TD_Color c){
	TD_printRGB(false, &c);
	putchar_unlocked(' ');
}

// Render a pair of pixels stacked vertically like this:
// [uc]
// [bc]
TD_FUNC void TD_render_pixel(TD_Color* uc, TD_Color* bc){
	// 4-Bit color
	if(TD_render_flags & TD_RENDER_COLOR){
		// With unicode characters
		if(TD_render_flags & TD_RENDER_UNICODE){
			TD_print4bit_full(
				TD_get_4bit_color(*uc)->fgc,
							  TD_get_4bit_color(*bc)->bgc
			);
		}
		// Without unicode characters
		else{
			TD_print4bit_half(
				TD_get_4bit_color((TD_Color){
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
			TD_printRGB_full(uc, bc);
		else{
			TD_printRGB_half((TD_Color){
				(unsigned char)(((unsigned short)uc->r+(unsigned short)bc->r)>>1),
				(unsigned char)(((unsigned short)uc->g+(unsigned short)bc->g)>>1),
				(unsigned char)(((unsigned short)uc->b+(unsigned short)bc->b)>>1)
			});
		}
	}
	// No color
	else{
		int density;
		if(uc->r || uc->g || uc->b || bc->r || bc->g || bc->b)
			density = (
				((uc->r+bc->r)>>1)*0.3f +
				((uc->g+bc->g)>>1)*0.59f +
				((uc->b+bc->b)>>1)*0.11f
			) / 256.f * (float)(TD_char_densities_count);
		else
			density = 0.f;
		putchar_unlocked(TD_char_densities[TD_CLAMP(density,0,TD_char_densities_count)]);
	}
}