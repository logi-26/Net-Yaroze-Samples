#include <libps.h>
#include "font.h"
#include "colours.h"
#include "controller.h"
#include "keyboard.h"

#define SCRNW 320
#define SCRNH 256
#define OT_LENGTH 9

GsOT WorldOrderingTable[10];
GsOT_TAG WorldTags[2][1 << OT_LENGTH];
PACKET packetArea[2][65536];

// State variables for the keyboard
static int showKeyboard = 0;
static char savedName[KB_MAX_INPUT + 1] = {0};

// Variable for the theme toggle
int currentThemeIdx = 0;

// Define your own theme
KBTheme MyTheme1;
KBTheme MyTheme2;


void UserDefinedThemes(void)
{
    // Start with the defaults theme for both
    KB_DefaultTheme(&MyTheme1);
    KB_DefaultTheme(&MyTheme2);

    // Customise Theme 1
    MyTheme1.keyButtonR = 255;
    MyTheme1.keyButtonG = 0;
    MyTheme1.keyButtonB = 0;

    // Customise Theme 2
    MyTheme2.keyButtonR = 0;
    MyTheme2.keyButtonG = 0;
    MyTheme2.keyButtonB = 255;
}


// Initialise the graphics
void InitGfx(void)
{
    SetVideoMode(MODE_PAL);
    GsInitGraph(SCRNW, SCRNH, GsNONINTER | GsOFSGPU, 1, 0);
    GsDefDispBuff(0, 0, 0, SCRNH);

    WorldOrderingTable[0].length = OT_LENGTH;
    WorldOrderingTable[0].org    = WorldTags[0];
    WorldOrderingTable[1].length = OT_LENGTH;
    WorldOrderingTable[1].org    = WorldTags[1];
}


// Main
int main(void)
{
    int activeBuffer;

    InitGfx();

    // Load font pixel data from RAM
    FontFX_LoadFont();
	
	// Initialise the controller system
	Ctrl_Init();
	
	// Initialise the keyboard
	KB_Init();
	
	// Define our own custom themes for the keyboard
	UserDefinedThemes();

	// Get the active buffer
    activeBuffer = GsGetActiveBuff();
	
    while (1)
    {
        GsSetWorkBase((PACKET*)packetArea[activeBuffer]);
        GsClearOt(0, 0, &WorldOrderingTable[activeBuffer]);
		
		// Update the font system
		FontFX_Update();
		
		// Update the controller system
		Ctrl_Update();
		
		/*****************************************************
		Demonstrate the keyboard
		*****************************************************/
		
		// Display the title
		FontFX_FontBegin();
		FontFX_SetStyle(FONT_STYLE_0);
		FontFX_SetColour(COL_MIDBLUE);
		FontFX_SetOutline(COL_WHITE);
		FontFX_SetSize(2);
		FontFX_Print(50, 20, "KEYBOARD DEMO", &WorldOrderingTable[activeBuffer], 0);
		FontFX_SetSize(1);
		FontFX_FontEnd();
		
		// Display previously entered name
		FontFX_PrintWithoutEffects(16, 80, "NAME:", FONT_STYLE_0, COL_CHARCOAL, &WorldOrderingTable[activeBuffer], 0);
		
		// If the user types something on the keyboard
		// (this would be much simpler if the font sheet contained a single style with uppercase and lowercase chars)
		if (savedName[0] != '\0')
		{
			int i;
			int tx = 16 + (6 * FONT_CHAR_W);

			for (i = 0; savedName[i] != '\0'; i++)
			{
				char c = savedName[i];
				char str[2];
				int style;

				str[0] = c;
				str[1] = '\0';

				// Use lowercase font atlas only for lowercase letters
				if (c >= 'a' && c <= 'z')
				{
					style = kbTheme.textLowerFontStyle;

					// Lowercase atlases are aligned to uppercase ASCII positions
					str[0] = c - 32;
				}
				else
				{
					// Numbers/symbols/uppercase always use primary font atlas
					style = kbTheme.textFontStyle;
				}

				FontFX_PrintWithoutEffects(tx, 80, str, style, COL_WHITE, &WorldOrderingTable[activeBuffer], 0);

				tx += FONT_CHAR_W;
			}
		}

		// L1 toggles through the pre-defined themes
		if (BTN_PRESSED(PADL1))
		{
			// Increment the index and wrap around using modulo
			currentThemeIdx = (currentThemeIdx + 1) % MAX_THEMES;
			
			// Apply the new theme to the global kbTheme
			KB_SetPredefinedTheme(currentThemeIdx);
		}
		
		// If the keyboard is not displayed
		if (!showKeyboard)
		{
			// Cross button opens keyboard with default theme
			if (BTN_PRESSED(PADcross))
			{
				KB_SetPosition(60, 100);
				KB_Open();
				showKeyboard = 1;
			}
			
			// Circle button opens keyboard with user theme 1
			if (BTN_PRESSED(PADcircle))
			{
				KB_SetTheme(MyTheme1);
				KB_SetPosition(60, 100);
				KB_Open();
				showKeyboard = 1;
			}
			
			// Square button opens keyboard with user theme 2
			if (BTN_PRESSED(PADsquare))
			{
				KB_SetTheme(MyTheme2);
				KB_SetPosition(60, 100);
				KB_Open();
				showKeyboard = 1;
			}

			// Triangle button opens keyboard with the white theme
			if (BTN_PRESSED(PADtriangle))
			{
				KB_SetPredefinedTheme(KB_THEME_WHITE);
				KB_SetPosition(60, 100);
				KB_Open();
				showKeyboard = 1;
			}
			
			FontFX_PrintPulse(80, 140, "CROSS TO ENTER NAME", FONT_STYLE_0, COL_MIDBLUE, 50, 250, 20, &WorldOrderingTable[activeBuffer], 0);
		}
		else
		{
			int result = KB_Update();

			// Display the keyboard
			KB_RenderAtStoredPos(&WorldOrderingTable[activeBuffer], 0);

			if (result == KB_RESULT_CONFIRM)
			{
				// Copy the input from the keyboard
				strncpy(savedName, KB_GetInput(), KB_MAX_INPUT);
				savedName[KB_MAX_INPUT] = '\0';
				
				// Hide the keyboard
				showKeyboard = 0;
			}
			else if (result == KB_RESULT_CANCEL)
			{
				showKeyboard = 0;
			}
		}
		
        DrawSync(0);
        VSync(0);

        GsSwapDispBuff();
        GsSortClear(COL_CHOCOLATE, &WorldOrderingTable[activeBuffer]);
        GsDrawOt(&WorldOrderingTable[activeBuffer]);

        activeBuffer ^= 1;
    }

    return 0;
}