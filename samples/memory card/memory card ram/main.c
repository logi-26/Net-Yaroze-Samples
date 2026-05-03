/******************************************************************************
    NET YAROZE: MEMORY CARD SAMPLE - (RAM version)
    ------------------------------------

    Uses 3 preloaded TIM files in RAM:
        AUTO file:
        local dload data\icon1.tim 800C8000
        local dload data\icon2.tim 800C9000
        local dload data\icon3.tim 800CA000

    Each TIM must be:
        - 16x16 pixels
        - 4-bit indexed
        - has CLUT
        - same palette across all 3 files
******************************************************************************/

#include <libps.h>
#include <string.h>
#include "pad.h"

// Screen definitions
#define SCRNW 320
#define SCRNH 256
#define OT_LENGTH 9

// Save format
#define BLOCK_SIZE 8192
#define SAVE_BLOCKS 1
#define SAVE_SIZE (BLOCK_SIZE * SAVE_BLOCKS)

// Save identifiers
#define SAVE_REGION "BE" // PAL
#define SAVE_PRODCODE "NETYA"
#define SAVE_IDENTIFIER "0001MYGAME"
#define SAVE_FILENAME SAVE_REGION SAVE_PRODCODE "-" SAVE_IDENTIFIER

// TIM locations in RAM
#define ICON1_ADDR ((unsigned char*)0x800C8000)
#define ICON2_ADDR ((unsigned char*)0x800C9000)
#define ICON3_ADDR ((unsigned char*)0x800CA000)

// Save file definitions
#define SAVE_TITLE_LEN 64
#define ICON_FRAME_SIZE 128
#define ICON_CLUT_SIZE 32
#define ICON_MAX_FRAMES 3

// Memory card definitions
#define CARD_TOTAL_BLOCKS 15
#define CARD_MAX_FILES 15

// Memory card paths
static char cardPath0[] = "bu00:" SAVE_FILENAME;
static char cardPath1[] = "bu10:" SAVE_FILENAME;

// Ordering tables and font-id
GsOT WorldOT[2];
GsOT_TAG WorldTags[2][1 << OT_LENGTH];
static PACKET packetArea[2][24 * 100];
int fntID;


// Raw memory card save buffer
typedef struct
{
    unsigned char data[SAVE_SIZE];
} SaveData;

// Global instance of the save buffer used for all save/load operations
SaveData saveData;

// Struct for the save game
typedef struct
{
    char          filename[64];								// Full device path: "bu00:BENETYA-0001MYGAME"
    char          title[SAVE_TITLE_LEN + 1];    			// Save title text stored in header
    int           blocks;									// Number of 8 KB memory card blocks used
    int           iconFrames;								// Number of icon frames (1=staic, 3=animated)
    unsigned char clut[ICON_CLUT_SIZE];						// 16-colour palette used by the icon
    unsigned char icon[ICON_MAX_FRAMES][ICON_FRAME_SIZE];   // Raw pixel data for up to three icon frames
} SaveInfo;


// Returns the full memory card file path for the requested slot
char *GetCardPath(int card)
{
    if (card == 0)
        return cardPath0;
    else
        return cardPath1;
}


// Reads a TIM image already loaded in memory
int ExtractTim(unsigned char *tim,
               unsigned char *clutOut,
               unsigned char *iconOut)
{
    unsigned long *p;
    unsigned long flags;
    unsigned long size;

	// Treat TIM data as 32-bit values for easier header access
    p = (unsigned long*)tim;

	// TIM file magic number must be 0x10
    if (p[0] != 0x10)
        return 0;

	// TIM flags field: bit 3 set = CLUT present
    flags = p[1];

    // This sample requires a CLUT-based image
    if ((flags & 0x08) == 0)
        return 0;

	// Move past TIM header (magic + flags)
    p += 2;

    /******************************************************************
    CLUT BLOCK
    
    p[0] = total block size in bytes
    Palette data begins 12 bytes into the block
    ******************************************************************/
    size = p[0];

    // Copy first 16 colours (32 bytes)
    memcpy(clutOut, ((unsigned char*)p) + 12, 32);

	// Advance pointer to next block (image block)
    p = (unsigned long*)
        (((unsigned char*)p) + size);

    /******************************************************************
    IMAGE BLOCK
    
    p[0] = total block size in bytes
    Pixel data begins 12 bytes into the block
    ******************************************************************/
    size = p[0];

    // Copy 16x16 4-bit image: 256 pixels / 2 pixels per byte = 128 bytes
    memcpy(iconOut,
           ((unsigned char*)p) + 12,
           128);

    return 1;
}


// Creates a complete PlayStation memory card save file with 3 TIM icons
void BuildSaveFromTIMs(int value)
{
	/******************************************************************
	The save contains:
	   0x000 - Header ("SC")
	   0x002 - Icon frame count (3 frames)
	   0x003 - Number of 8 KB blocks used
	   0x004 - Save title
	   0x060 - CLUT (palette)
	   0x080 - Icon frame 1
	   0x100 - Icon frame 2
	   0x180 - Icon frame 3
	   0x200 - Game save data
	******************************************************************/
	
	// Temporary storage for extracted icon data
    unsigned char clut[32];
    unsigned char frame1[128];
    unsigned char frame2[128];
    unsigned char frame3[128];

	// Clear the full save buffer before rebuilding it
    memset(saveData.data, 0, SAVE_SIZE);

	// Extract palette + three icon frames from TIM files in memory
	// All icons are expected to share the same palette
    ExtractTim(ICON1_ADDR, clut, frame1);
    ExtractTim(ICON2_ADDR, clut, frame2);
    ExtractTim(ICON3_ADDR, clut, frame3);
	
	// ------------------------------------------------------------
    // Standard PS1 save header
    // ------------------------------------------------------------

	// Save identifier
    saveData.data[0x00] = 'S';
    saveData.data[0x01] = 'C';
	
	// 0x13 = 3-frame animated icon
    saveData.data[0x02] = 0x13;

	// Number of memory card blocks used by this save
    saveData.data[0x03] = (unsigned char)SAVE_BLOCKS;

	// Save title shown in memory card manager
    strncpy((char*)&saveData.data[0x04], "NET YAROZE SAVE", 64);

	// Copy icon palette
    memcpy(&saveData.data[0x60], clut, 32);

	// Copy three animation frames
    memcpy(&saveData.data[0x080], frame1, 128);
    memcpy(&saveData.data[0x100], frame2, 128);
    memcpy(&saveData.data[0x180], frame3, 128);

	// Game-specific save data begins after the 512-byte header
    *(int*)&saveData.data[0x200] = value;
}


// Returns the number of free blocks on a memory card
int CardFreeBlocks(int card)
{
	/******************************************************************
	card = 0 - Slot 1 ("bu00:")
	card = 1 - Slot 2 ("bu10:")

	Method:
	- Scan every file on the selected card
	- Add up how many 8 KB blocks each file uses
	- Subtract total used blocks from the card capacity
	
	Standard PlayStation memory cards contain 15 usable blocks
	******************************************************************/
	
    struct DIRENTRY  entry;
    struct DIRENTRY *found;
    int usedBlocks = 0;
	
	// Search pattern for selected memory card slot
    char *pattern = (card == 0) ? "bu00:*" : "bu10:*";

	// Find first file on the card
    found = firstfile(pattern, &entry);

	// Walk through every file entry
    while (found != 0)
    {
		// Convert file size in bytes to 8 KB blocks
        usedBlocks += entry.size / BLOCK_SIZE;
		
		// Move to next file
        found = nextfile(&entry);
    }

	// Total card capacity minus used space
    return CARD_TOTAL_BLOCKS - usedBlocks;
}


// Reads basic information from a PlayStation memory card save file
int CardReadSaveInfo(char *path, SaveInfo *info)
{
	/******************************************************************
	path = full device path to the save file
	        Example: "bu00:BENETYA-0001MYGAME"
	
	info = destination structure filled with save metadata
	
	Reads only the first 512 bytes (0x200), which contains the standard PS1 save header:
		0x000 - "SC" magic
		0x002 - icon frame count
		0x003 - block count
		0x004 - title text
		0x060 - CLUT
		0x080 - icon frame 1
		0x100 - icon frame 2
		0x180 - icon frame 3
	
	Returns:
		1 = success
		0 = failed to open file or invalid save header
	******************************************************************/
	
    unsigned char header[0x200];
    int fd;
    int i;

	// Clear output structure before filling it
    memset(info, 0, sizeof(SaveInfo));

	// Open save file for reading
    fd = open(path, 0x0001);
    if (fd < 0)
        return 0;

    // Read first 512 bytes (entire save header)
    read(fd, (char*)header, sizeof(header));
    close(fd);

    // Validate standard PS1 save magic
    if (header[0x00] != 'S' || header[0x01] != 'C')
        return 0;

    // Store full file path
    strncpy(info->filename, path, 63);
    info->filename[63] = '\0';

    // Copy save title and guarantee null termination
    memcpy(info->title, &header[0x04], SAVE_TITLE_LEN);
    info->title[SAVE_TITLE_LEN] = '\0';

    // Number of memory card blocks used
    info->blocks     = (int)header[0x03];
	
	// Number of icon frames (1=static, 3=animated)
    info->iconFrames = (int)(header[0x02] & 0x0F);

    // Copy icon palette (16 colours = 32 bytes)
    memcpy(info->clut, &header[0x60], ICON_CLUT_SIZE);

    // Copy only the icon frames declared in the header
    for (i = 0; i < info->iconFrames && i < ICON_MAX_FRAMES; i++)
    {
        memcpy(info->icon[i],
               &header[0x80 + (i * ICON_FRAME_SIZE)],
               ICON_FRAME_SIZE);
    }

    return 1;
}


// Writes a save file to the selected memory card slot
void CardWrite(int card, int value)
{
	/******************************************************************
	card = 0 - Slot 1
	card = 1 - Slot 2
	
	value = sample game data stored inside the save

	Process:
	- Check free space on the card
	- Build a fresh save file in memory
	- Delete any previous save with the same filename
	- Create/open the new file
	- Write SAVE_SIZE bytes to the card
	******************************************************************/
	
    int  fd;
    int  free;
    char *path = GetCardPath(card);

	// Check available free blocks on the selected card
    free = CardFreeBlocks(card);

	// Not enough space for this save file
    if (free < SAVE_BLOCKS)
    {
        FntPrint(fntID,
            "CARD%d: NOT ENOUGH SPACE\n"
            "NEED %d FREE %d\n",
            card + 1, SAVE_BLOCKS, free);
        return;
    }

	// Build complete save file in RAM
    BuildSaveFromTIMs(value);

    /******************************************************************
    open() mode flags:

    0x0200 = write only
    0x0002 = create file if it does not exist

    Upper 16 bits specify how many 8 KB memory card
    blocks to allocate for the new file
    ******************************************************************/
	 
	// Remove any existing save with the same filename first
    delete(path);

	// Create/open file and allocate required blocks
    fd = open(path, 0x0202 | (SAVE_BLOCKS << 16));

	// Failed to create file
    if (fd < 0)
    {
        FntPrint(fntID, "WRITE OPEN ERR %d\n", fd);
        return;
    }

	// Write full save buffer to memory card
    write(fd, (char*)saveData.data, SAVE_SIZE);
	
	// Close file handle
    close(fd);
}


// Reads a save file from the selected memory card slot
int CardRead(int card)
{
	/******************************************************************
	card = 0 - Slot 1
	card = 1 - Slot 2
	
	Loads the save into the global saveData buffer, verifies the
	standard PS1 save header, then returns the sample integer value
	stored at offset 0x200
	
	Returns:
	  Stored integer value on success
	  777 if the file could not be opened or the save is invalid
	******************************************************************/
	
    int fd;
    char *path = GetCardPath(card);

	// Open save file for reading
    fd = open(path, 0x0001);

	// File not found / open failed
    if (fd < 0)
        return 777;

	// Read entire save file into RAM
    read(fd, (char*)saveData.data, SAVE_SIZE);
    close(fd);

	// Validate standard PS1 save magic ("SC")
    if (saveData.data[0x00] != 'S' ||
        saveData.data[0x01] != 'C')
        return 777;

	// Return game data stored after the 512-byte header
    return *(int*)&saveData.data[0x200];
}


// Print the status of the memory card
void PrintCardStatus(int card, long status)
{
    int freeBlocks;
    switch (status)
    {
        case 0:
            FntPrint(fntID, "Card %d: None\n\n", card + 1);
            break;

        case 1:
            // Read the available free blocks
			freeBlocks = CardFreeBlocks(card);
            
			FntPrint(fntID,
                     "Card %d: Present (%d free blocks)\n\n",
                     card + 1,
                     freeBlocks);
            break;

        case 2:
            FntPrint(fntID, "Card %d: New\n\n", card + 1);
            break;

        case 3:
            FntPrint(fntID, "Card %d: Error\n\n", card + 1);
            break;

        case 4:
            FntPrint(fntID, "Card %d: Unformatted\n\n", card + 1);
            break;
    }
}


// Initialise the graphics
void InitGfx(void)
{
    SetVideoMode(MODE_PAL);

    GsInitGraph(SCRNW, SCRNH,
        GsNONINTER | GsOFSGPU,
        1, 0);

    GsDefDispBuff(0,0,0,SCRNH);

    WorldOT[0].length = OT_LENGTH;
    WorldOT[0].org    = WorldTags[0];

    WorldOT[1].length = OT_LENGTH;
    WorldOT[1].org    = WorldTags[1];

    FntLoad(960,256);
    fntID = FntOpen(10,20,300,220,0,512);
}


// Main
int main(void)
{
    int buffer;
    int frame = 0;

    int loaded = 0;

    long cardStatus1;
    long cardStatus2;

    int card1_free;
    int card2_free;

    u_long pad = 0;
    u_long oldPad = 0;

    InitGfx();
    PadInit();

    buffer = GsGetActiveBuff();

    while (1)
    {
        frame++;

        pad         = PadRead();
        cardStatus1 = TestCard(0);
        cardStatus2 = TestCard(1);

        GsSetWorkBase((PACKET*)packetArea[buffer]);
        GsClearOt(0,0,&WorldOT[buffer]);

        FntPrint(fntID, "NET YAROZE: MEMORY CARD SAMPLE\n");
        FntPrint(fntID, "(RAM VERSION)\n\n\n");

        FntPrint(fntID, "FRAME=%d\n\n\n", frame);

		// Display the memory card status
        PrintCardStatus(0, cardStatus1);
        PrintCardStatus(1, cardStatus2);

		// If memory card 2 is present and has free space
        if (cardStatus2 == 1)
        {
			FntPrint(fntID, "\nX Save   O Load\n\n\n");
			
			// Save the current frame number to the memory card
            if ((pad & PADcross) && !(oldPad & PADcross))
                CardWrite(1, frame);

			// Read the previously saved value from the memory card
            if ((pad & PADcircle) && !(oldPad & PADcircle))
                loaded = CardRead(1);
        }

        FntPrint(fntID, "LOADED VALUE: %d\n", loaded);

        oldPad = pad;

        FntFlush(fntID);

        DrawSync(0);
        VSync(0);

        GsSwapDispBuff();
        GsSortClear(40,40,40, &WorldOT[buffer]);
        GsDrawOt(&WorldOT[buffer]);

        buffer ^= 1;
    }

    return 0;
}