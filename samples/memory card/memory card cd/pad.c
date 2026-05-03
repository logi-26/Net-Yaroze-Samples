#include <libps.h>
#include "pad.h"

// Pad buffers:s
volatile u_char *bb0, *bb1;


// Pad initialisation
void PadInit (void)
{
	GetPadBuf(&bb0, &bb1);
}


// Read pad data into the buffers
u_long PadRead(void)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}