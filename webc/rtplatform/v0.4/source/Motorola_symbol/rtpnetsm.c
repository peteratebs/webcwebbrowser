 /*
 | RTPNETSM.C -
 |
 | EBSnet -
 |
 |  $Author: vmalaiya $
 |  $Date: 2005/11/28 17:12:48 $
 |  $Name:  $
 |  $Revision: 1.4 $
 |
 | Copyright EBSnet Inc. , 2004
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
*/

/*****************************************************************************
 * Header files
 *****************************************************************************/

#include "rtpnetsm.h"

/*****************************************************************************
 * Macros
 *****************************************************************************/

/*****************************************************************************
 * Types
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Data
 *****************************************************************************/

/*****************************************************************************
 * Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
/* Register a callback and initialize a list for this individual state
   machine */
int rtp_net_sm_init (
		RTP_NET_SM* sm,
		RTP_NET_SM_CALLBACK cb
	)
{
	rtp_dllist_init(&sm->node);
	sm->callbackFn = cb;
	return (0);
}

/*---------------------------------------------------------------------------*/
/* Initialize a master list for holding all state machines, initialise the
   base state machine for this master list. */
int rtp_net_aggregate_sm_init (
		RTP_NET_AGGREGATE_SM* sm,
		RTP_NET_SM_CALLBACK cb
	)
{
	rtp_dllist_init(&sm->smList);
	return (rtp_net_sm_init(&sm->base, cb));
}

/*---------------------------------------------------------------------------*/
/* Master Callback Routine. This routine handles the 4 State Machine events
   1. RTP_NET_SM_EVENT_ADD_TO_SELECT_LIST
      - add all node of the master list to the select list
   2. RTP_NET_SM_EVENT_PROCESS_STATE
      - process all nodes in the aggregate list, remove the nodes from the list
        free up these nodes.
   3. RTP_NET_SM_EVENT_DESTROY
      - delete all nodes from aggregate list and release all nodes.
*/

void rtp_net_aggregate_sm_callback (
		RTP_NET_SM* sm,
		RTP_NET_SM_EVENT* event,
		RTP_NET_SM_RESULT* result
	)
{
	RTP_NET_AGGREGATE_SM* aggSm = (RTP_NET_AGGREGATE_SM*) sm;

	switch (event->type)
	{
		case RTP_NET_SM_EVENT_ADD_TO_SELECT_LIST:
		{
			RTP_INT32 childTimeout;

			result->maxTimeoutMsec = RTP_TIMEOUT_INFINITE;

			RTP_DLLIST_FOREACH (RTP_NET_SM, childSm, aggSm->smList,

				childTimeout = rtp_net_sm_add_to_select_list (
						childSm,
						event->arg.addToSelectList.readList,
						event->arg.addToSelectList.writeList,
						event->arg.addToSelectList.errList
					);

				if (result->maxTimeoutMsec == RTP_TIMEOUT_INFINITE ||
				    (childTimeout != RTP_TIMEOUT_INFINITE && childTimeout < result->maxTimeoutMsec))
				{
					result->maxTimeoutMsec = childTimeout;
				}
			);

			break;
		}

		case RTP_NET_SM_EVENT_PROCESS_STATE:
		{
			RTP_BOOL done;

			RTP_DLLIST_FOREACH (RTP_NET_SM, childSm, aggSm->smList,

				done = rtp_net_sm_process_state (
						childSm,
						event->arg.processState.readList,
						event->arg.processState.writeList,
						event->arg.processState.errList
					);

				if (done)
				{
					rtp_dllist_remove(&childSm->node);
					rtp_net_sm_delete(childSm);
				}
			);

			result->done = (aggSm->smList.next == &aggSm->smList);

			break;
		}

		case RTP_NET_SM_EVENT_DESTROY:
			RTP_DLLIST_FOREACH (RTP_NET_SM, childSm, aggSm->smList,
				rtp_dllist_remove(&childSm->node);
				rtp_net_sm_delete(childSm);
			);
			break;
	}
}

/*---------------------------------------------------------------------------*/
/* Add the supplied individual state machine to the master list of state
   machines. */
void rtp_net_aggregate_sm_add (
		RTP_NET_AGGREGATE_SM* aggsm,
		RTP_NET_SM* sm
	)
{
	rtp_dllist_insert_before(&aggsm->smList, &sm->node);
}

/*---------------------------------------------------------------------------*/
/* Invoke the sm callback, with state set to - Add to select list. Return the
   timeout in milisec for select */
RTP_INT32 _rtp_net_sm_add_to_select_list (
		RTP_NET_SM* sm,
		RTP_FD_SET* readList,
		RTP_FD_SET* writeList,
		RTP_FD_SET* errList
	)
{
	RTP_NET_SM_EVENT event;
	RTP_NET_SM_RESULT result;

	event.type = RTP_NET_SM_EVENT_ADD_TO_SELECT_LIST;
	event.arg.addToSelectList.readList = readList;
	event.arg.addToSelectList.writeList = writeList;
	event.arg.addToSelectList.errList = errList;

	sm->callbackFn(sm, &event, &result);

	return (result.maxTimeoutMsec);
}

/*---------------------------------------------------------------------------*/
/* invoke the SM callback with state set for processing. Return true if
   completed                                                                 */
RTP_BOOL _rtp_net_sm_process_state (
		RTP_NET_SM* sm,
		RTP_FD_SET* readList,
		RTP_FD_SET* writeList,
		RTP_FD_SET* errList
	)
{
	RTP_NET_SM_EVENT event;
	RTP_NET_SM_RESULT result;

	event.type = RTP_NET_SM_EVENT_PROCESS_STATE;
	event.arg.processState.readList = readList;
	event.arg.processState.writeList = writeList;
	event.arg.processState.errList = errList;

	sm->callbackFn(sm, &event, &result);

	return (result.done);
}

/*---------------------------------------------------------------------------*/
/* release resources held by the state machine and free                      */
void _rtp_net_sm_delete (
		RTP_NET_SM* sm
	)
{
	RTP_NET_SM_EVENT event;
	RTP_NET_SM_RESULT result;

	event.type = RTP_NET_SM_EVENT_DESTROY;
	sm->callbackFn(sm, &event, &result);

	event.type = RTP_NET_SM_EVENT_FREE;
	sm->callbackFn(sm, &event, &result);
}
