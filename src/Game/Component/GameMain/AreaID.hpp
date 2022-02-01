#pragma once


namespace AREAID
{
    enum VALUE
    {
        ID_NONE = 0,                //  0-0

        SELECTABLESTART,
        NORMALSTART = SELECTABLESTART,
        ID_AREA01 = NORMALSTART,    //  0-1     NY
        ID_AREA02,                  //  0-2     NY
        ID_AREA03,                  //  0-0     NY
        ID_AREA04,                  //  1-1     NY
        ID_AREA05,                  //  1-2     NY
        ID_AREA06,                  //  1-3     NY
        ID_AREA09,                  //  2-1     NY
        ID_AREA10,                  //  2-2     NY
        ID_AREA11,                  //  2-3     NY
        ID_AREA12,                  //  3-1     ZERO
        ID_AREA13,                  //  3-2     ZERO
        ID_AREA14,                  //  3-3     ZERO
        ID_AREA15,                  //  4-1     DHO
        ID_AREA17,                  //  4-2     DHO
        ID_AREA18,                  //  4-3     DHO
        ID_AREA19,                  //  4-4     DHO
        ID_AREA20,                  //  4-5     DHO
        ID_AREA21,                  //  4-6     DHO
        ID_AREA22,                  //  4-7     DHO
        ID_AREA23,                  //  4-8     DHO
        ID_AREA24,                  //  4-9     DHO
        ID_AREA25,                  //  5-1     TRI
        ID_AREA26,                  //  5-4     TRI
        ID_AREA27,                  //  5-5     TRI
        ID_AREA28,                  //  5-2     TRI
        ID_AREA29,                  //  5-3     TRI
        ID_AREA30,                  //  5-6     TRI
        ID_AREA31,                  //  5-7     TRI
        ID_AREA32,                  //  5-8     TRI
        ID_AREA33,                  //  6-1     JAPAN
        ID_AREA34,                  //  6-4     JAPAN
        ID_AREA35,                  //  6-5     JAPAN
        ID_AREA36,                  //  6-2     JAPAN
        ID_AREA37,                  //  6-3     JAPAN
        ID_AREA38,                  //  6-6     JAPAN
        ID_AREA39,                  //  6-7     NY
        ID_AREA40,                  //  6-8     NY
        ID_AREA41,                  //  7-1     NY
        ID_AREA42,                  //  7-2     NY
        ID_AREA43,                  //  8-1     NY
        ID_AREA44,                  //  8-2     NY
        ID_AREA45,                  //  9-1     NY
        ID_AREA46,                  //  9-2     NY
        ID_AREA47,                  //  9-3     NY
        ID_AREA48,                  //  10-1    FNY
        ID_AREA49,                  //  10-2    FNY
        ID_AREA50,                  //  10-3    FNY
        ID_AREA52,                  //  11-1    KURA
        ID_AREA53,                  //  11-2    KURA
        ID_AREA54,                  //  11-3    KURA
        ID_AREA55,                  //  11-4    KURA
        ID_AREA56,                  //  11-5    KURA
        ID_AREA57,                  //  11-6    KURA
        ID_AREA58,                  //  11-7    KURA
        NORMALMAX,

        //
        //  STN - station
        //  E - world warps
        //  J - branches in world
        //
        ID_MNY_J01 = NORMALMAX,
        ID_MNY_J02,
        ID_MNY_J03,
        ID_DHO_J01,
        ID_FNY_J01,
        ID_KUR_J01,
        
        STATIONSTART,
        ID_MNY_STN = STATIONSTART,
        ID_DHO_STN,
        ID_TRI_STN,
        ID_JPN_STN,
        ID_FNY_STN,
        ID_KUR_STN,
        STATIONMAX,
        
        WARPSTART = STATIONMAX,
        ID_MNY_E01 = WARPSTART,
        ID_MNY_E02,
        ID_MNY_E03,
        ID_MNY_E04,
        ID_MNY_E05,
        ID_DHO_E01,
        ID_DHO_E02,
        ID_DHO_E03,
        ID_TRI_E01,
        ID_TRI_E02,
        ID_JPN_E01,
        ID_JPN_E02,
        ID_FNY_E01,
        ID_KUR_E01,
        WARPMAX,
        SELECTABLEMAX = WARPMAX,

        ID_MNY_C01 = SELECTABLEMAX,
        ID_MNY_C02,
        ID_MNY_C03,
        ID_MNY_C04,
        ID_MNY_C05,
        ID_MNY_C06,
        ID_MNY_C07,
        ID_MNY_C08,
        ID_MNY_C09,
        ID_MNY_C10,
        ID_MNY_C11,
        ID_MNY_C12,
        ID_MNY_C13,
        ID_MNY_C14,
        ID_DHO_C01,
        ID_DHO_C02,
        ID_DHO_C03,
        ID_DHO_C04,
        ID_DHO_C05,
        ID_DHO_C06,
        ID_TRI_C01,
        ID_TRI_C02,
        ID_TRI_C03,
        ID_TRI_C04,
        ID_TRI_C05,
        ID_JPN_C01,
        ID_JPN_C02,
        ID_JPN_C03,
        ID_JPN_C04,
        ID_JPN_C05,
        ID_JPN_C06,
        ID_JPN_C07,
        ID_FNY_C01,
        ID_FNY_C02,
        ID_FNY_C03,
        ID_FNY_C04,
        ID_KUR_C01,
        ID_KUR_C02,
        ID_KUR_C03,
        ID_KUR_C04,

        ID_MAX,

        NEXUSSTART = ID_MAX,
        ID_AREA60_A = NEXUSSTART,
        ID_AREA60_B,
        ID_AREA60_C,
        ID_AREA60_D,
        NEXUSMAX,

        HOME = ID_AREA03,
    };
};
