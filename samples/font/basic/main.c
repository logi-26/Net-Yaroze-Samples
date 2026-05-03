/******************************************************************************
    NET YAROZE: FONT DISPLAY SAMPLE
    ---------------------------------
    Loads the 8DOTFONT.TIM from RAM using the AUTO file dload command
    and renders text on screen using GsSortBoxFill pixel-by-pixel rendering

    The font characters are 8x8 pixels, there are 32 columns and 16 rows
	
	Lowercase fonts do not have numbers
	Japanes characters from the sprite sheet are not being parsed in this demo
******************************************************************************/

#include <libps.h>
#include "font.h"

#define SCRNW     320
#define SCRNH     256
#define OT_LENGTH 9

GsOT     WorldOT[2];
GsOT_TAG WorldTags[2][1 << OT_LENGTH];
PACKET   packetArea[2][65536];


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

    // Load font pixel data from RAM
    FontLoad();

    buf = GsGetActiveBuff();
	
    while (1)
    {
        GsSetWorkBase((PACKET*)packetArea[buf]);
        GsClearOt(0, 0, &WorldOT[buf]);
		
		/*****************************************************
		Demonstrate font style, font colour and font print
		*****************************************************/
		
		FontSetColour(255, 255, 255);
		FontSetStyle(FONT_STYLE_0);
		FontPrint(16, 2, "ABCDEFGHIJ 0123456789", &WorldOT[buf], 0);
		FontPrint(16, 16,  "!@#$%^&*()+-=_+[]{}'\|>.?/;:,<", &WorldOT[buf], 0);
		
		FontSetColour(255, 0, 0);
		FontSetStyle(FONT_STYLE_1);
		FontPrint(16, 32, "ABCDEFGHIJ 0123456789", &WorldOT[buf], 0);
		FontPrint(16, 48,  "!@#$%^&*()+-=_+[]{}'\|>.?/;:,<", &WorldOT[buf], 0);

		FontSetColour(0, 255, 0);
		FontSetStyle(FONT_STYLE_2);
		FontPrint(16, 64, "ABCDEFGHIJ 0123456789", &WorldOT[buf], 0);
		FontPrint(16, 80,  "!@#$%^&*()+-=_+[]{}'\|>.?/;:,<", &WorldOT[buf], 0);

		FontSetColour(0, 0, 255);
		FontSetStyle(FONT_STYLE_3);
		FontPrint(16, 96, "ABCDEFGHIJ", &WorldOT[buf], 0);
		FontPrint(16, 112,  "!@#$%^&*()+-=_+[]{}'\|>.?/;:,<", &WorldOT[buf], 0);
		
		FontSetColour(255, 255, 0);
		FontSetStyle(FONT_STYLE_4);
		FontPrint(16, 128, "ABCDEFGHIJ", &WorldOT[buf], 0);
		FontPrint(16, 144,  "!@#$%^&*()+-=_+[]{}'\|>.?/;:,<", &WorldOT[buf], 0);
		
		FontSetColour(255, 0, 255);
		FontSetStyle(FONT_STYLE_5);
		FontPrint(16, 160, "ABCDEFGHIJ 0123456789", &WorldOT[buf], 0);
		FontPrint(16, 176,  "!@#$%^&*()+-=_+[]{}'\|>.?/;:,<", &WorldOT[buf], 0);
		
		FontSetColour(0, 255, 255);
		FontSetStyle(FONT_STYLE_6);
		FontPrint(16, 192, "ABCDEFGHIJ", &WorldOT[buf], 0);
		FontPrint(16, 208,  "!@#$%^&*()+-=_+[]{}'\|>.?/;:,<", &WorldOT[buf], 0);

        DrawSync(0);
        VSync(0);

        GsSwapDispBuff();
        GsSortClear(0, 0, 0, &WorldOT[buf]);
        GsDrawOt(&WorldOT[buf]);

        buf ^= 1;
    }

    return 0;
}