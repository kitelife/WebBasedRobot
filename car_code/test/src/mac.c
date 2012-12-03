/****************************************************************
 * file:	mac.c
 * date:	2011-11-28
 * author:      peach
 * description: Implement the mechanism required by MAC
 ***************************************************************/


#include "mac.h"


/************************************************
 	**	Local variables		**
 ***********************************************/
static PHY_STATE_ENUM phy_state;


/************************************************
 * Description: If phy layer is idle
 *
 * Arguments:
 * 	None.
 * Return:
 * 	TRUE -- If phy in idle state
 * 	FALSE -- If tx in progress
 *
 * Date: 2010-05-21
 ***********************************************/
BOOL phy_idle()
{
	return (phy_state == PHY_STATE_IDLE);
}

