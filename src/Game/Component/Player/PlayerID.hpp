#pragma once


namespace PLAYERID
{
    enum VALUE
    {
        ID_INVALID = -1,

		/**
		 *	NOTE:
		 *	Order is important and not should be changed!!!
		 *	A lot of code tables depends on this order (SE code tables in PlayerUtil module for example)
		 *	It's a bit different than in retail game - SLA and SPL were swapped
		 */
		ID_LEO = 0,
        ID_RAP,
        ID_MIC,
        ID_DON,
        ID_SLA,
        ID_CAS,
        ID_KAR,
        ID_SPL,

        ID_MAX,
        ID_START = ID_LEO,
    };

    const char* GetName(VALUE idPlayer);    
    bool IsImplemented(VALUE idPlayer);
    bool IsEnabled(VALUE idPlayer);
    RwRGBA GetColor(VALUE idPlayer);
};