/******************************************************************************
    NET YAROZE: FONT SAMPLE
    --------------------------
    Renders text using the 8DOTFONT.TIM spritesheet loaded from the AUTO file.

    The font TIM is a 4bpp indexed image containing 8x8 pixel characters
    arranged in a 32-column grid starting at ASCII 33 (space+1)

    Characters are rendered pixel by pixel using GsSortBoxFill, which avoids
    the need for VRAM sprite uploads entirely

    AUTO file setup:
        local dload data\8dotfont.tim 800C8000
******************************************************************************/

#include "font.h"
#include <string.h>

// Pointer to raw 4bpp pixel data within the TIM buffer in RAM
static unsigned char *fontPixels = 0;
static int            fontLoaded = 0;

// Current draw colour
static unsigned char fontR = 255;
static unsigned char fontG = 255;
static unsigned char fontB = 255;

static int fontStyleOffset = 0;


void FontLoad(void)
{
	/*****************************************************
	FontLoad
	Parses the TIM file in RAM and stores a pointer to the pixel data.
	No VRAM upload is needed - pixels are read directly from RAM each frame.

	TIM layout (4bpp with CLUT):
		+0x00   magic + flags (8 bytes)
		+0x08   CLUT block (blockSize bytes)
		+0x08+blockSize   image block header (12 bytes)
		+0x08+blockSize+12   pixel data
	*****************************************************/
	
    unsigned char *p = FONT_TIM_ADDR + 8;
    unsigned long  blockSize;

    // Read CLUT block size and skip past it
    blockSize = (unsigned long)p[0]
              | ((unsigned long)p[1] << 8)
              | ((unsigned long)p[2] << 16)
              | ((unsigned long)p[3] << 24);

    p += blockSize;

    // p now points to the image block header
    // Pixel data begins 12 bytes into the image block
    fontPixels = p + 12;
    fontLoaded = 1;
}


// Set the font colour
void FontSetColour(unsigned char r, unsigned char g, unsigned char b)
{
    fontR = r;
    fontG = g;
    fontB = b;
}


// Set the font offset for selecting a font style 
void FontSetStyle(int offset)
{
    fontStyleOffset = offset;
}


// Draw a single char
void FontDrawChar(int screenX, int screenY, int c, GsOT *ot, int priority)
{
	/*****************************************************
	FontDrawChar
	Draws a single character at the given screen position.

	The font sheet is 256 pixels wide (128 bytes in 4bpp) x 128 pixels tall.
	Characters are 8x8 pixels arranged in 32 columns.

	4bpp pixel layout per byte:
		bits 7-4 = left/even pixel
		bits 3-0 = right/odd pixel
		value 0  = draw pixel (character)
		non-zero = background (skip)
	*****************************************************/
	
    int charIndex, col, row;
    int px, py;
    int byteOffset;
    unsigned char byteVal;
    unsigned char pixel;
    GsBOXF box;

	// If font graphics were not loaded, do nothing
    if (!fontLoaded || c < FONT_FIRST_CHAR)
        return;

    // Set the character index based on the offsets
	charIndex = c - FONT_FIRST_CHAR + fontStyleOffset;
	
	// Set the column and row
    col = charIndex % FONT_COLS;
    row = charIndex / FONT_COLS;

	// Initialise 1x1 flat-shaded sprite used as a single pixel
    box.attribute = 0;
	
	// Width and height of sprite (1 pixel)
    box.w = 1;
    box.h = 1;
	
	// Set sprite colour using current font RGB values
    box.r = fontR;
    box.g = fontG;
    box.b = fontB;

    for (py = 0; py < FONT_CHAR_H; py++)
    {
        for (px = 0; px < FONT_CHAR_W; px++)
        {
			/*****************************************************
			Font sheet is 256 pixels wide = 128 bytes per row (4bpp)
			Byte offset = (sheet_row * 128) + (sheet_col_in_pixels / 2)
			sheet_row    = row * FONT_CHAR_H + py
			sheet_col    = col * FONT_CHAR_W + px
			*****************************************************/

			// Calculate byte position inside 4bpp font image data
			// Divide X by 2 because each byte stores 2 pixels
            byteOffset = (row * FONT_CHAR_H + py) * 128
                       + (col * FONT_CHAR_W + px) / 2;

			// Read packed pixel byte from font texture data
            byteVal = fontPixels[byteOffset];

            // Even pixel = low nibble, odd pixel = high nibble
            if (px & 1)
				pixel = (byteVal >> 4) & 0x0F;
			else
				pixel = byteVal & 0x0F;

            // Value 0 = character pixel, draw it
            if (pixel == 0)
            {
                box.x = (short)(screenX + px);
                box.y = (short)(screenY + py);
                GsSortBoxFill(&box, ot, priority);
            }
	   }
    }
}


// Draw a string
void FontPrint(int screenX, int screenY, char *text, GsOT *ot, int priority)
{
	/*****************************************************
	FontPrint
	Draws a null-terminated string at the given screen position
	Supports '\n' for newlines.
	Each character is drawn using FontDrawChar()
	*****************************************************/
	
    int i, len, curX, curY;
    int c;

	// If font graphics were not loaded, do nothing
    if (!fontLoaded)
        return;

	// Get string length
    len  = strlen(text);
	
	// Start drawing at requested screen position
    curX = screenX;
    curY = screenY;

	// Process each character in the string
    for (i = 0; i < len; i++)
    {
        c = (unsigned char)text[i];

		// Handle newline character
        if (c == '\n')
        {
			// Return to starting X position
            curX = screenX;
			
			// Move down one text row
            curY += FONT_CHAR_H;
			
            continue;
        }

		// Draw current character
        FontDrawChar(curX, curY, c, ot, priority);
		
		// Move cursor right for next character
        curX += FONT_CHAR_W;
    }
}