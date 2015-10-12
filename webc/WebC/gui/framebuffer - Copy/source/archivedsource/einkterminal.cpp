#include "webc.h"

#include "webcmem.h"
#include "FBConfig.h"
#include "rtpprint.h"

#include "rtpnet.h"

#include "rtpprint.h"
#include "einkterminal.h"

#define INCLUDE_EINKDISPLAY (1)



extern int Disp_DrawFullImage_EBS(void *pImageData,int pleft,int ptop,int pwidth,int pheight);

int send_EinkDisplayBuffer(WEBC_UINT8 *srcPtr,HTMLRect rect,WEBC_BOOL bPartialImage)
{
int width,height;

	if (rect.right > (HORIZONTAL_RESOLUTION )) 
		   rect.right = (HORIZONTAL_RESOLUTION);
		
	if (rect.bottom > (VERTICAL_RESOLUTION ) ) 
		  rect.bottom = (VERTICAL_RESOLUTION );
	if (rect.top < 0) 
		   rect.top = 0;
	if (rect.left < 0) 
		   rect.left = 0;

	rtp_printf("send_EinkDisplayBuffer:bPartialImagew==%d:srcPtr:0x%lx:top:%d:left:%d:right:%d:bot:%d:\r\n"
	          , bPartialImage,srcPtr,rect.top,rect.left,rect.right,rect.bottom);

    if(bPartialImage != true)
	{

	    rect.top = 0;
	    rect.left = 0;
	    rect.right = (HORIZONTAL_RESOLUTION -1);//319;
	    rect.bottom = (VERTICAL_RESOLUTION -1);//239;
	    rtp_printf("send_EinkDisplayBuffer:FULLRECTFORCED:%d:left:%d:right:%d:bot:%d:\r\n"
	              ,rect.top,rect.left,rect.right,rect.bottom);
				  width = ((rect.right - rect.left )+ 1);	
				  height = ((rect.bottom - rect.top) + 1);
 	}
	else
	{
	    width = ((rect.right - rect.left )+ 1);
		height = ((rect.bottom - rect.top) + 1);
		
		if(width <= 0)
		{
		   width = HORIZONTAL_RESOLUTION;
		}

        if(height <= 0)
		{
		   height = VERTICAL_RESOLUTION;
		}

		if(width>HORIZONTAL_RESOLUTION)
		{
		 width = HORIZONTAL_RESOLUTION;
		}

		if(height>VERTICAL_RESOLUTION)
		{
          height = VERTICAL_RESOLUTION;
		}

	    rtp_printf("send_EinkDisplayBuffer:fullImageDraw==FALSE:top:%d:left:%d:right:%d:bot:%d:\r\n"
	              ,rect.top,rect.left,rect.right,rect.bottom);

	}


	rtp_printf("send_EinkDisplayBuffer:callingeink:fl:left:%d:top:%d:wid:%d:height:%d\r\n"
	          ,rect.left,rect.top,width,height);
#if (INCLUDE_EINKDISPLAY)
	Disp_DrawFullImage_EBS(srcPtr,rect.left,rect.top,width,height);
#else
    rtp_printf("send_EinkDisplayBuffer:DISABLED EINK\r\n");
#endif
 return 1;
}

