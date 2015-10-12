#ifndef WIN32

#include "TargetFrameBuffer.h"
/**
*	This method is used to initialize the OS specific code for the frame buffer layer.
*   The threads needed will be started here and the display section will also be set.
*/

TargetFrameBuffer::TargetFrameBuffer(){
	//END OS_SPECIFIC CODE
}

/**
*	Destroys os specific threads and screen displaying mechanism.
*/
TargetFrameBuffer::~TargetFrameBuffer(){
}


WEBC_UINT8 *TargetFrameBuffer::GetBaseAddress(WEBC_INT whichBuffer)
{
	return (WEBC_UINT8 *) 0;
}

void TargetFrameBuffer::SwitchToBuffer(WEBC_INT whichBuffer)
{
	m_displayBuffer = whichBuffer;
}
/**
*	This method will take the internal display that WebC has been working with
*   and transform it to an understandable format for the target device.
*
*	Params: disp - internal webc bitmap for screen display, this bitmap represents what must be
*				   displayed to the screen device
*/
void TargetFrameBuffer::DoDisplay(void)
{
}

// Alert box for Javascript.. (badly in need of rework)
extern "C" void webc_user_alert_box(char *str)
{
WEBC_CHAR UnicodeStr[512];
#error Should be in the display package
//    webc_c_strcpy (&UnicodeStr[0],str);
//    MessageBox(0, (LPCTSTR) UnicodeStr, (LPCTSTR) L"webC Error", MB_OKCANCEL);
}
#endif // WIN32
