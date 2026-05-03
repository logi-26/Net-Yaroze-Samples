#ifndef FONT_H
#define FONT_H
 
#include <libps.h>
 
// RAM address of the TIM file loaded by dload
#define FONT_TIM_ADDR ((unsigned char*)0x800C8000)
 
// Font character dimensions
#define FONT_CHAR_W 8
#define FONT_CHAR_H 8
#define FONT_COLS 32

// Offset into font sheet
#define FONT_FIRST_CHAR 33

// Font offsets to switch between font styles
#define FONT_STYLE_0 0    	// Uppercase font
#define FONT_STYLE_1 64   	// Uppercase font
#define FONT_STYLE_2 128  	// Uppercase font
#define FONT_STYLE_3 160  	// Lowercase font (no numbers)
#define FONT_STYLE_4 192	// Lowercase font (no numbers)
#define FONT_STYLE_5 416  	// Uppercase font
#define FONT_STYLE_6 448  	// Lowercase font (no numbers)

/************* FUNCTION PROTOTYPES *******************/
void FontLoad(void);
void FontSetColour(unsigned char r, unsigned char g, unsigned char b);
void FontSetStyle(int offset);
void FontDrawChar(int screenX, int screenY, int c, GsOT *ot, int priority);
void FontPrint(int screenX, int screenY, char *text, GsOT *ot, int priority);
/*****************************************************/

#endif