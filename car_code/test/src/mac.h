/****************************************************************
 * file:	mac.h
 * date:	2011-11-28
 * author:      peach
 * description:	The MAC layer, define the parameters.
 ***************************************************************/
#ifndef MAC_H
#define MAC_H

// PHY FSM state enumeration
typedef enum _PHY_STATE_ENUM {
	PHY_STATE_IDLE,
	PHY_STATE_COMMAND_START,
	PHY_STATE_TX_WAIT
}PHY_STATE_ENUM;


#endif