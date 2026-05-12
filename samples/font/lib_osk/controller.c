#include "controller.h"

// Globals
volatile u_char *bb0, *bb1;
cntrl_data *ctrlBuffer1;
cntrl_data *ctrlBuffer2;
u_long pad = 0;
u_long oldPad = 0;


/*****************************************************
Ctrl_Init
Initialises both the PadRead and GetPadBuf
*****************************************************/
void Ctrl_Init(void)
{
    // Low-level buffers for PadRead()
    GetPadBuf(&bb0, &bb1);

    // High-level buffers for cntrl_data
    ctrlBuffer1 = (cntrl_data *)bb0;
    ctrlBuffer2 = (cntrl_data *)bb1;
}


/*****************************************************
Ctrl_Update
Updates pad and oldPad so BTN_PRESSED/BTN_HELD/BTN_RELEASED work
*****************************************************/
void Ctrl_Update(void)
{
    oldPad = pad;
    pad = Ctrl_Read();
}


/*****************************************************
Ctrl_Update
Returns the raw button bitmask for the current frame
*****************************************************/
u_long Ctrl_Read(void)
{
    return ~( *(bb0 + 3)        |
              *(bb0 + 2) <<  8  |
              *(bb1 + 3) << 16  |
              *(bb1 + 2) << 24 );
}


/*****************************************************
Ctrl_IsConnected
Checks if a controller is connected
*****************************************************/
int Ctrl_IsConnected(cntrl_data *buf)
{
    return (buf != NULL) && (buf->status != CTRL_DISCONNECTED);
}


/*****************************************************
Ctrl_GetTypeName
Get the connected controller type
*****************************************************/
const char *Ctrl_GetTypeName(int type)
{
    switch (type)
    {
        case CTRL_MOUSE:      return "MOUSE";
        case CTRL_NEGCON:     return "NEGCON";
        case CTRL_NORMAL:     return "DIGITAL PAD";
        case CTRL_ANALOG_JOY: return "ANALOG JOYSTICK";
        case CTRL_GUNCON:     return "GUNCON";
        case CTRL_ANALOG:     return "ANALOG PAD";
        default:              return "UNKNOWN";
    }
}