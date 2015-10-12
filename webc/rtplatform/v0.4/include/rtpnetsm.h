 /*
 | RTPNETSM.H -
 |
 | EBSnet -
 |
 |  $Author: vmalaiya $
 |  $Date: 2005/02/08 16:06:15 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBSnet Inc. , 2004
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
*/

#ifndef __RTPNETSM_H__
#define __RTPNETSM_H__

typedef struct s_RTP_NET_SM_EVENT      RTP_NET_SM_EVENT;
typedef struct s_RTP_NET_SM            RTP_NET_SM;
typedef struct s_RTP_NET_AGGREGATE_SM  RTP_NET_AGGREGATE_SM;
typedef union  u_RTP_NET_SM_RESULT     RTP_NET_SM_RESULT;

#include "rtpdllist.h"
#include "rtptypes.h"
#include "rtpnet.h"

typedef void (*RTP_NET_SM_CALLBACK) (
		RTP_NET_SM* sm,
		RTP_NET_SM_EVENT* event,
		RTP_NET_SM_RESULT* result
	);

typedef enum e_RTP_NET_SM_EVENT_TYPE
{
	RTP_NET_SM_EVENT_ADD_TO_SELECT_LIST = 0,
	RTP_NET_SM_EVENT_PROCESS_STATE,
	RTP_NET_SM_EVENT_DESTROY,
	RTP_NET_SM_EVENT_FREE,
	RTP_NET_SM_NUM_EVENT_TYPES
}
RTP_NET_SM_EVENT_TYPE;

struct s_RTP_NET_SM_EVENT
{
	RTP_NET_SM_EVENT_TYPE type;

	union
	{
		struct
		{
			RTP_FD_SET* readList;
			RTP_FD_SET* writeList;
			RTP_FD_SET* errList;
		}
		addToSelectList;

		struct
		{
			RTP_FD_SET* readList;
			RTP_FD_SET* writeList;
			RTP_FD_SET* errList;
		}
		processState;
	}
	arg;
};

union u_RTP_NET_SM_RESULT
{
	RTP_INT32 maxTimeoutMsec;
	RTP_BOOL  done;
};

struct s_RTP_NET_SM
{
	RTP_DLLIST_NODE      node;
	RTP_NET_SM_CALLBACK  callbackFn;
};

struct s_RTP_NET_AGGREGATE_SM
{
	RTP_NET_SM        base;
	RTP_DLLIST_NODE   smList;
};

#define rtp_net_sm_handle_event(M,E,R)         _rtp_net_sm_handle_event((RTP_NET_SM*)M,E,R)
#define rtp_net_sm_add_to_select_list(M,R,W,E) _rtp_net_sm_add_to_select_list((RTP_NET_SM*)M,R,W,E)
#define rtp_net_sm_process_state(M,R,W,E)      _rtp_net_sm_process_state((RTP_NET_SM*)M,R,W,E)
#define rtp_net_sm_delete(M)                   _rtp_net_sm_delete((RTP_NET_SM*)M)

#ifdef __cplusplus
extern "C" {
#endif

int rtp_net_sm_init (
		RTP_NET_SM* sm,
		RTP_NET_SM_CALLBACK cb
	);

/*------------------------------------------------------------------*/
/* related to RTP_NET_AGGREGATE_SM                                  */
int rtp_net_aggregate_sm_init (
		RTP_NET_AGGREGATE_SM* sm,
		RTP_NET_SM_CALLBACK cb
	);

void rtp_net_aggregate_sm_callback (
		RTP_NET_SM* sm,
		RTP_NET_SM_EVENT* event,
		RTP_NET_SM_RESULT* result
	);

void rtp_net_aggregate_sm_add (
		RTP_NET_AGGREGATE_SM* aggsm,
		RTP_NET_SM* sm
	);

/*------------------------------------------------------------------*/
/* functions below this line are for internal use only              */

void _rtp_net_sm_handle_event (
		RTP_NET_SM* sm,
		RTP_NET_SM_EVENT* event,
		RTP_NET_SM_RESULT* result
	);

RTP_INT32 _rtp_net_sm_add_to_select_list (
		RTP_NET_SM* sm,
		RTP_FD_SET* readList,
		RTP_FD_SET* writeList,
		RTP_FD_SET* errList
	);

// return true if completed
RTP_BOOL _rtp_net_sm_process_state (
		RTP_NET_SM* sm,
		RTP_FD_SET* readList,
		RTP_FD_SET* writeList,
		RTP_FD_SET* errList
	);

// release resources held by the state machine and free
void _rtp_net_sm_delete (
		RTP_NET_SM* sm
	);

#ifdef __cplusplus
}
#endif

#endif /* __TEMPLATE_H__ */
