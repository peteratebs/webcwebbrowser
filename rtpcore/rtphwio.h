/*
|  RTPHWIO.H - Runtime Platform String Library
| 
|  EBSnet - 								
| 
|   $Author: sarah $
|   $Date: 2005/05/14 02:43:30 $
|   $Name:  $
|   $Revision: 1.1 $
| 
|  Copyright EBSnet Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __RTPHWIO_H__
#define __RTPHWIO_H__

void    rtp_io_delay(void);
        
/* SPRZ - added KS_ */
#define KS_OUTBYTE(ADD, VAL)  
#define KS_OUTWORD(ADD, VAL)  
#define KS_OUTDWORD(ADD, VAL) 
#define KS_INBYTE(ADD)        1
#define KS_INWORD(ADD)        1
#define KS_INDWORD(ADD)       1


#endif /* __RTPHWIO_H__ */
