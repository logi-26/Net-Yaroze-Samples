/******************************************************************************
    NET YAROZE: FONT FX MODULE (SAMPLE)
    ---------------------------------
    Loads the 8DOTFONT.TIM from RAM using the AUTO file dload command
    and renders text on screen using GsSortBoxFill pixel-by-pixel rendering

    The font characters are 8x8 pixels, there are 32 columns and 16 rows
	
	Lowercase fonts do not have numbers
	Japanes characters from the sprite sheet are not being parsed in this demo
******************************************************************************/

#include <libps.h>
#include "font.h"
#include "colours.h"

#define SCRNW 320
#define SCRNH 256
#define OT_LENGTH 9

GsOT WorldOT[2];
GsOT_TAG WorldTags[2][1 << OT_LENGTH];
PACKET packetArea[2][65536];


// Initialise the graphics
void InitGfx(void)
{
    SetVideoMode(MODE_PAL);
    GsInitGraph(SCRNW, SCRNH, GsNONINTER | GsOFSGPU, 1, 0);
    GsDefDispBuff(0, 0, 0, SCRNH);

    WorldOT[0].length = OT_LENGTH;
    WorldOT[0].org    = WorldTags[0];
    WorldOT[1].length = OT_LENGTH;
    WorldOT[1].org    = WorldTags[1];
}


// Main
int main(void)
{
    int buf;
	
    InitGfx();
	
	buf = GsGetActiveBuff();

	// Load font pixel data from RAM
    FontFX_LoadFont();
	
    while (1)
    {
        GsSetWorkBase((PACKET*)packetArea[buf]);
        GsClearOt(0, 0, &WorldOT[buf]);
		
		/*****************************************************
		Demonstrate the font effects
		*****************************************************/
		
		// Update the font engine
		FontFX_Update();
		
		// Set standard font size
		FontFX_SetSize(1);
		
		// Demo the outline font effect
		FontFX_PrintOutline(16, 2, "FONT", FONT_STYLE_0, COL_BLUE, COL_YELLOW, &WorldOT[buf], 0);
		FontFX_PrintOutline(55, 2, "WITH", FONT_STYLE_0, COL_BLACK, COL_WHITE, &WorldOT[buf], 0);
		FontFX_PrintOutline(95, 2, "OUTLINE", FONT_STYLE_0, COL_WHITE, COL_RED, &WorldOT[buf], 0);
		FontFX_PrintOutline(160, 2, "EFFECT", FONT_STYLE_0, COL_LIME, COL_PURPLE, &WorldOT[buf], 0);
		
		// Demo the rainbow font effect
		FontFX_PrintRainbow(16, 16, "FONT WITH RAINBOW COLOUR CYCLE EFFECT", FONT_STYLE_1, 4, &WorldOT[buf], 0);
		
		// Demo the pulse font effect
		FontFX_PrintPulse(16, 32, "FONT WITH PULSING EFFECT", FONT_STYLE_2, COL_RED, 0, 255, 8, &WorldOT[buf], 0);
		
		// Demo the flash font effect
		FontFX_PrintFlash(16, 48, "FONT WITH FLASHING EFFECT", FONT_STYLE_3, COL_BLUE, 0, 255, 90, &WorldOT[buf], 0);
		
		// Demo the shaking font effect
		FontFX_PrintShake(16, 64, "FONT WITH SHAKING EFFECT", FONT_STYLE_4, COL_YELLOW, 1, 1, &WorldOT[buf], 0);
		
		// Demo the scrolling font effect
		FontFX_PrintScroll(16, 80, "FONT WITH SCROLLING EFFECT", FONT_STYLE_5, COL_GREEN, 320, 15, &WorldOT[buf], 0);
		
		// Demo the wave font effect
		FontFX_PrintWave(16, 96, "FONT WITH WAVING EFFECT", FONT_STYLE_5, COL_VIOLET, 2, 6, 1, &WorldOT[buf], 0);
		
		// Demo the typewritter font effect
		FontFX_PrintTypewriter(16, 112,  "FONT WITH TYPING EFFECT", FONT_STYLE_6, COL_CYAN, 1, &WorldOT[buf], 0);
		
		// Demo the fade-in font effect
		FontFX_PrintFade(16, 128, "FONT WITH FADE IN EFFECT", FONT_STYLE_0, COL_RED, 0, 255, 6, 0, &WorldOT[buf], 0);
		
		// Demo the fade-out font effect
		FontFX_PrintFade(16, 157, "FONT WITH FADE OUT EFFECT", FONT_STYLE_0, COL_RED, 255, 0, 6, 0, &WorldOT[buf], 0);
		
		// Demo stacking multiple font effects (shaking + rainbow)
		FontFX_FontBegin();
		FontFX_SetStyle(FONT_STYLE_0);
		FontFX_SetShake(1, 1);
		FontFX_SetRainbow(10);
		FontFX_Print(16, 144, "FONT WITH SHAKING + RAINBOW EFFECTS", &WorldOT[buf], 0);
		FontFX_FontEnd();
		
		// Demo stacking multiple font effects (waving + outline)
		FontFX_FontBegin();
		FontFX_SetStyle(FONT_STYLE_0);
		FontFX_SetColour(COL_RED);
		FontFX_SetOutline(COL_WHITE);
		FontFX_SetWave(2, 6, 1);
		FontFX_Print(16, 176, "FONT WITH WAVING + OUTLINE EFFECTS", &WorldOT[buf], 0);
		FontFX_FontEnd();
		
		// Demo stacking multiple font effects (scale + rainbow)
		FontFX_FontBegin();
		FontFX_SetStyle(FONT_STYLE_0);
		FontFX_SetColour(COL_RED);
		FontFX_SetSize(2);
		FontFX_SetRainbow(20);
		FontFX_Print(16, 200, "FONT SCALE EFFECT", &WorldOT[buf], 0);
		FontFX_SetSize(1);
		FontFX_FontEnd();
		
        DrawSync(0);
        VSync(0);

        GsSwapDispBuff();
        GsSortClear(0, 0, 0, &WorldOT[buf]);
        GsDrawOt(&WorldOT[buf]);

        buf ^= 1;
    }

    return 0;
}