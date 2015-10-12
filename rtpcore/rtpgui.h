 /*
 | RTPGUI.H - Runtime Platform Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE
 |
 | EBS - RT-Platform
 |
 |  $Author: vmalaiya $
 |  $Date: 2006/07/17 15:29:00 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBS Inc. , 2006
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

#ifndef __RTPGUI_H__
#define __RTPGUI_H__

#include "rtp.h"

#define RTP_LIST_STYLE_MENU		0x00
#define RTP_LIST_STYLE_HELP		0x01
#define RTP_LIST_STYLE_LISTBOX	0x02
#define RTP_LIST_STYLE_NOFORMAT	0x04
#define RTP_LIST_ITEM_STYLE_NOBREAK 0x01
#define RTP_LIST_ITEM_STYLE_SUBMENU 0x02
#define RTP_LIST_ITEM_STYLE_COLLAPSED 0x04
#define RTP_LIST_ITEM_STYLE_EXPANDED  0x08

/************************************************************************
 * Terminal API                                                         *
 ************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
void *rtp_gui_list_init(void *pguictx, int style, char *title, char *subtitle, char *prompt);
void rtp_gui_list_destroy(void *plist);
void *rtp_gui_list_add_int_item(void *plist, char *item, int itemid, int indentlevel, int isgreyed, int style);
void *rtp_gui_list_add_item(void *plist, char *item, void *itemid, int indentlevel, int isgreyed, int style);
int rtp_gui_list_check_duplicate_item(void *vplist, char  *item);
char *rtp_gui_list_item_value(void *plist, void *itemid,int strdup_item);
void *rtp_gui_list_execute_item(void *vplist);
void *rtp_gui_list_execute_void(void *plist);
int  rtp_gui_list_execute_int(void *plist);
int rtp_gui_list_size(void *plist);
void rtp_gui_prompt_text(char *indent, char *prompt, char *inbuffer);
void rtp_gui_conout(char *status);
void rtp_gui_conout_two(char *alert1,char *alert2);

#ifdef __cplusplus
}
#endif


#endif /* __RTPGUI_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
