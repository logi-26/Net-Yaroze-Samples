/******************************************************************************
    NET YAROZE: MEMORY CARD SAMPLE (CD-ROM)
    ------------------------------------

    Uses 3 TIM files loaded from the CD-ROM:
        \\DATA\\ICON1.TIM
        \\DATA\\ICON2.TIM
        \\DATA\\ICON3.TIM

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
#define SCRNW       320
#define SCRNH       256
#define OT_LENGTH   9

// Save format
#define BLOCK_SIZE 8192
#define SAVE_BLOCKS 1
#define SAVE_SIZE (BLOCK_SIZE * SAVE_BLOCKS)

// Save identifiers
#define SAVE_REGION "BE" // PAL
#define SAVE_PRODCODE "NETYA"
#define SAVE_IDENTIFIER "0001MYGAME"
#define SAVE_FILENAME SAVE_REGION SAVE_PRODCODE "-" SAVE_IDENTIFIER

// Memory card definitions
#define CARD_TOTAL_BLOCKS 15
#define CARD_MAX_FILES 15

// Save file definitions
#define SAVE_TITLE_LEN 64
#define ICON_FRAME_SIZE 128
#define ICON_CLUT_SIZE 32
#define ICON_MAX_FRAMES 3

// Size of the buffer for the TIM files
#define TIM_BUFFER_SIZE 2048

// Message display duration
#define MSG_DURATION 20 

// Buffers for the TIM save file icons
unsigned char icon1Buf[TIM_BUFFER_SIZE];
unsigned char icon2Buf[TIM_BUFFER_SIZE];
unsigned char icon3Buf[TIM_BUFFER_SIZE];

// Memory card paths
static char cardPath0[] = "bu00:" SAVE_FILENAME;
static char cardPath1[] = "bu10:" SAVE_FILENAME;

// Ordering tables and font-id
GsOT WorldOT[2];
GsOT_TAG WorldTags[2][1 << OT_LENGTH];
static PACKET packetArea[2][24 * 100];
int fntID;

// Used for timed messages
static char msgText[64];
static int  msgTimer = 0;


// Raw memory card save buffer
typedef struct
{
    unsigned char data[SAVE_SIZE];
} SaveData;

// Global instance of the save buffer used for all save/load operations
SaveData saveData;


typedef struct
{
    char          filename[64];								// Full device path: "bu00:BENETYA-0001MYGAME"
    char          title[SAVE_TITLE_LEN + 1];    			// Save title text stored in header
    int           blocks;									// Number of 8 KB memory card blocks used
    int           iconFrames;								// Number of icon frames (1=staic, 3=animated)
    unsigned char clut[ICON_CLUT_SIZE];						// 16-colour palette used by the icon
    unsigned char icon[ICON_MAX_FRAMES][ICON_FRAME_SIZE];   // Raw pixel data for up to three icon frames
} SaveInfo;


// Sets a temporary on-screen message
void SetMsg(char *text)
{
    strncpy(msgText, text, 63);
    msgText[63] = '\0';
    msgTimer    = MSG_DURATION;
}


// Displays a temporary on-screen message
void PrintMsg(void)
{
    if (msgTimer > 0)
    {
        FntPrint(fntID, "~c099\n%s\n", msgText);
        msgTimer--;
    }
}


// Returns the full memory card file path for the requested slot
char *GetCardPath(int card)
{
    if (card == 0)
        return cardPath0;
    else
        return cardPath1;
}


// Loads an entire file from the CD-ROM into memory
int LoadFileFromCD(char *name, void *buffer)
{
	// Start asynchronous CD file read
    int bytes = CdReadFile(name, (u_long*)buffer, 0);
    
	// Wait until the read has fully completed
	CdReadSync(0, 0);
	
	// Return size of file loaded
    return bytes;
}


// Reads a TIM image already loaded into memory
int ExtractTim(unsigned char *tim,
               unsigned char *clutOut,
               unsigned char *iconOut)
{
    unsigned long magic, flags, size;
    unsigned char *p = tim;

	// TIM header, first 4 bytes must be TIM magic value 0x10
    memcpy(&magic, p, 4); p += 4;
    if (magic != 0x10)
        return 0;

	// Flags field describes image type and whether a CLUT exists
	// Bit 3 set = CLUT present
    memcpy(&flags, p, 4); p += 4;
    if ((flags & 0x08) == 0)
        return 0;

    /******************************************************************
    CLUT block
    
    Block layout:
        +0   block size
        +4   CLUT X
        +6   CLUT Y
        +8   width
        +10  height
        +12  palette data
    
    We copy the first 32 bytes:
    16 colours x 2 bytes each = 32 bytes
    ******************************************************************/
    memcpy(&size, p, 4);
    memcpy(clutOut, p + 12, 32);
    
	// Advance to next block (image data block)
	p += size;

    /******************************************************************
    Image block
    
    Same block header layout
    Pixel data begins at +12
    
    16x16 image @ 4-bit:
    256 pixels / 2 pixels per byte = 128 bytes
    ******************************************************************/
    memcpy(&size, p, 4);
    memcpy(iconOut, p + 12, 128);

    return 1;
}


// Build the game save with animated icon
void BuildSaveFromTIMs(int value)
{
	/******************************************************************
        Build standard PS1 memory card save header

        Offset  Size    Purpose
        ------  ----    ---------------------------------------------
        0x00    2       Magic ID ("SC")
        0x02    1       Icon display flag (0x13 = 3 animated frames)
        0x03    1       Number of 8 KB blocks used by this save
        0x04    64      Save title text
        0x60    32      CLUT (16-colour palette)
        0x80    128     Icon frame 1
        0x100   128     Icon frame 2
        0x180   128     Icon frame 3
    ******************************************************************/
	
	// Temporary buffers for extracted TIM data:
    // one shared palette (CLUT) and three icon animation frames
    unsigned char clut[32];
    unsigned char frame1[128];
    unsigned char frame2[128];
    unsigned char frame3[128];

	// Clear the entire save buffer before building the file
    memset((char*)saveData.data, 0, SAVE_SIZE);

	// Extract palette + frame data from the three TIM files loaded from CD-ROM
    // All three icons must use the same palette
    ExtractTim(icon1Buf, clut, frame1);
    ExtractTim(icon2Buf, clut, frame2);
    ExtractTim(icon3Buf, clut, frame3);
	
	// Save file identifier used by the memory card manager
    saveData.data[0x00] = 'S';
    saveData.data[0x01] = 'C';
	
	// 3-frame animated icon
    saveData.data[0x02] = 0x13;
	
	// Number of memory card blocks used by this save
    saveData.data[0x03] = (unsigned char)SAVE_BLOCKS;

	// Save title displayed in the memory card manager
    strncpy((char*)&saveData.data[0x04], "NET YAROZE SAVE", 64);

	// Copy the shared icon palette
    memcpy(&saveData.data[0x60], clut, 32);

	// Copy three animation frames
    memcpy(&saveData.data[0x080], frame1, 128);
    memcpy(&saveData.data[0x100], frame2, 128);
    memcpy(&saveData.data[0x180], frame3, 128);

    // Actual game save data begins after the 512-byte header
    // For this sample we only store one integer value
    *(int*)&saveData.data[0x200] = value;
}


// Check the available free blocks on the memory card
int CardFreeBlocks(int card)
{
    struct DIRENTRY  entry;
    struct DIRENTRY *found;
    int usedBlocks = 0;
    char *pattern = (card == 0) ? "bu00:*" : "bu10:*";
	
	// Get the first file on the memory card
    found = firstfile(pattern, &entry);

	// Loop while nextfile is available
    while (found != 0)
    {
        usedBlocks += entry.size / BLOCK_SIZE;
        found = nextfile(&entry);
    }

	// Return the available free blocks
    return CARD_TOTAL_BLOCKS - usedBlocks;
}


// Write the save data to the memory card
void CardWrite(int card, int value)
{
    int  fd;
    int  free;
	
	// Get the path for the selected memory card
    char *path = GetCardPath(card);

	// Check the available free blocks of data
    free = CardFreeBlocks(card);

	// If there is not enough available free space
    if (free < SAVE_BLOCKS)
    {
        SetMsg("SAVE FAILED: NOT ENOUGH SPACE");
        return;
    }

	// Build the save data with animated icon
    BuildSaveFromTIMs(value);

	// If there is an existing save file, delete it
    delete(path);

	// Open the memory card
    fd = open(path, 0x0202 | (SAVE_BLOCKS << 16));

    if (fd < 0)
    {
        SetMsg("SAVE FAILED: COULD NOT OPEN FILE");
        return;
    }

	// Write the save data to the memory card
    write(fd, (char*)saveData.data, SAVE_SIZE);
    close(fd);

	// Display a message to the user
	{
        char buf[64];
		sprintf(buf, "SAVE SUCCESSFUL: (%d)", value);
        SetMsg(buf);
    }
}


// Read the save data from the memory card
int CardRead(int card)
{
    int fd;
    int value;
	
	// Get the path for the selected memory card
    char *path = GetCardPath(card);

	// Open the memory card in O_RDONLY mode
    fd = open(path, 0x0001);

    if (fd < 0)
    {
        SetMsg("LOAD FAILED: UNABLE TO OPEN MEMORY CARD");
        return 777;
    }

	// Read the save file
    read(fd, (char*)saveData.data, SAVE_SIZE);
    close(fd);

	// Check for save data magic 
    if (saveData.data[0x00] != 'S' ||
        saveData.data[0x01] != 'C')
    {
        SetMsg("LOAD FAILED: SAVE DATA CORRUPTED");
        return 777;
    }

	// Read the saved value
    value = *(int*)&saveData.data[0x200];

	// Display a message to the user
	{
		char buf[64];
		sprintf(buf, "GAME LOADED: VALUE=%d", value);
		SetMsg(buf);
	}
	
	return value;
}


// Load the save icon TIM files from the CD-ROM
void LoadSaveIcons(void)
{
    LoadFileFromCD("\\DATA\\ICON1.TIM", icon1Buf);
    LoadFileFromCD("\\DATA\\ICON2.TIM", icon2Buf);
    LoadFileFromCD("\\DATA\\ICON3.TIM", icon3Buf);
}


// Print the status of the memory card
void PrintCardStatus(int card, long status)
{
    int freeBlocks;
    switch (status)
    {
        case 0:
            FntPrint(fntID, "~c900Card %d: None", card + 1);
            break;

        case 1:
            // Read the available free blocks
			freeBlocks = CardFreeBlocks(card);
            
			FntPrint(fntID,
                     "~c090Card %d: Present (%d free blocks)",
                     card + 1,
                     freeBlocks);
            break;

        case 2:
            FntPrint(fntID, "~c090Card %d: New", card + 1);
            break;

        case 3:
            FntPrint(fntID, "~c900Card %d: Error", card + 1);
            break;

        case 4:
            FntPrint(fntID, "~c990Card %d: Unformatted", card + 1);
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

    long cardStatus1;
    long cardStatus2;

    u_long pad = 0;
    u_long oldPad = 0;

    InitGfx();
    PadInit();

    // Load the savegame icons from the CD-ROM
    LoadSaveIcons();

    buffer = GsGetActiveBuff();

    while (1)
    {
		frame++;

        pad = PadRead();
        
		// Get the memory card status
		cardStatus1 = TestCard(0);
        cardStatus2 = TestCard(1);

        GsSetWorkBase((PACKET*)packetArea[buffer]);
        GsClearOt(0,0,&WorldOT[buffer]);
		
		
		FntPrint(fntID, "~c999NET YAROZE: MEMORY CARD SAMPLE\n\n");
        FntPrint(fntID, "~c999(CD-ROM VERSION)");
		FntPrint(fntID, "\n\n\n\n");
		
		FntPrint(fntID, "~c990FRAME=%d", frame);
		FntPrint(fntID, "\n\n\n\n");

		// Display the memory card status
        PrintCardStatus(0, cardStatus1);
		FntPrint(fntID, "\n\n");
        PrintCardStatus(1, cardStatus2);
		FntPrint(fntID, "\n\n\n\n\n");

		// If memory card 2 is present and has free space
        if (cardStatus2 == 1)
        {
            FntPrint(fntID, "~c009X Save   O Load");
			FntPrint(fntID, "\n\n\n\n\n");

			// Save the current frame number to the memory card
            if ((pad & PADcross) && !(oldPad & PADcross))
                CardWrite(1, frame);

			// Read the previously saved value from the memory card
            if ((pad & PADcircle) && !(oldPad & PADcircle))
                CardRead(1);
        }

        // Print the status message if one is active
        PrintMsg();

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