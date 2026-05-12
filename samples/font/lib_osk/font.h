#ifndef FONT_H
#define FONT_H
 
#include <libps.h>
 
// RAM address of the TIM file loaded by dload
#define FONT_TIM_ADDR ((unsigned char*)0x80108000)
 
// Font character dimensions
#define FONT_CHAR_W 8
#define FONT_CHAR_H 8
#define FONT_COLS 32

// Offset into font sheet (first char is at offset 33)
#define FONT_FIRST_CHAR 33

// Font offsets to switch between font styles
#define FONT_STYLE_0 0    	// Uppercase font
#define FONT_STYLE_1 64   	// Uppercase font
#define FONT_STYLE_2 128  	// Uppercase font
#define FONT_STYLE_3 160  	// Lowercase font (no numbers)
#define FONT_STYLE_4 192	// Lowercase font (no numbers)
#define FONT_STYLE_5 416  	// Uppercase font
#define FONT_STYLE_6 448  	// Lowercase font (no numbers)

// Font effect flags
#define FONT_FX_FLASH     (1 << 0)
#define FONT_FX_RAINBOW   (1 << 1)
#define FONT_FX_TYPE      (1 << 2)
#define FONT_FX_WAVE      (1 << 3)
#define FONT_FX_SHAKE     (1 << 4)
#define FONT_FX_PULSE     (1 << 6) 
#define FONT_FX_SCROLL    (1 << 7) 
#define FONT_FX_FADE      (1 << 8)
#define FONT_FX_OUTLINE   (1 << 9)


/************* FUNCTION PROTOTYPES *******************/

// Core functions
void FontFX_LoadFont(void);
void FontFX_FontBegin(void);
void FontFX_FontEnd(void);
void FontFX_SetSize(int size);
void FontFX_SetColour(unsigned char r, unsigned char g, unsigned char b);
void FontFX_SetStyle(int offset);
void FontFX_DrawChar(int screenX, int screenY, int c, GsOT *ot, int priority);
void FontFX_Print(int x, int y, char *text, GsOT *ot, int pri);
void FontFX_PrintWithoutEffects(int screenX, int screenY, char *text, int style, int r, int g, int b, GsOT *ot, int priority);
void FontFX_Update(void);

// Font effect functions
void FontFX_SetFlash(int min, int max, int speed);
void FontFX_SetPulse(int min, int max, int speed);
void FontFX_SetRainbow(int speed);
void FontFX_SetWave(int amplitude, int speed, int spacing);
void FontFX_SetShake(int intensity, int speed);
void FontFX_SetTypewriter(int speed);
void FontFX_SetScroll(int width, int speed);
void FontFX_SetFade(int start, int end, int speed, int loop);
void FontFX_SetOutline(int r, int g, int b);
void FontFX_SetAlpha(int alfa);

// Wrapper functions
void FontFX_PrintPulse(int x, int y, char *text, int style, int r, int g, int b, int min, int max, int speed, GsOT *ot, int pri);
void FontFX_PrintFlash(int x, int y, char *text, int style, int r, int g, int b, int min, int max, int speed, GsOT *ot, int pri);
void FontFX_PrintWave(int x, int y, char *text, int style, int r, int g, int b, int amplitude, int speed, int spacing, GsOT *ot, int pri);
void FontFX_PrintShake(int x, int y, char *text, int style, int r, int g, int b, int intensity, int speed, GsOT *ot, int pri);
void FontFX_PrintRainbow(int x, int y, char *text, int style, int speed, GsOT *ot, int pri);
void FontFX_PrintTypewriter(int x, int y, char *text, int style, int r, int g, int b, int speed, GsOT *ot, int pri);
void FontFX_PrintScroll(int x, int y, char *text, int style, int r, int g, int b, int width, int speed, GsOT *ot, int pri);
void FontFX_PrintFade(int x, int y, char *text, int style, int r, int g, int b, int start, int end, int speed, int loop, GsOT *ot, int pri);
void FontFX_PrintOutline(int x, int y, char *text, int style, int r, int g, int b, int ro, int go, int bo, GsOT *ot, int pri);
/*****************************************************/

#endif