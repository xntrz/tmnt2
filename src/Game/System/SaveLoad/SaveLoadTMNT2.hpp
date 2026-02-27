#pragma once

#include "Game/Component/GameData/GameData.hpp"


struct TMNT2_SAVE_DATA
{
    COptionData::RAWDATA m_option;
    CGameRecord::RAWDATA m_record;

    bool IsValid(void) const;
};


static_assert(std::is_pod<TMNT2_SAVE_DATA>::value, "TMNT2_SAVE_DATA should be POD type");


#if defined(TARGET_PC)

/*
 *  data size checkouts for retail PC game saveload compatibility
 */

// game option
CHECK_SIZE(CDisplayOptionData::RAWDATA,  0x8);
CHECK_SIZE(CGamepadOptionData::RAWDATA,  0x20);
CHECK_SIZE(CKeyboardOptionData::RAWDATA, 0x34);
CHECK_SIZE(CPlayOptionData::RAWDATA,     0x8);
CHECK_SIZE(CSoundOptionData::RAWDATA,    0x10);
CHECK_SIZE(COptionData::RAWDATA,         0x114);

// game record
CHECK_SIZE(CItemRecord::RAWDATA,         0x18);
CHECK_SIZE(CAntiqueRecord::RAWDATA,      0x11C);
CHECK_SIZE(CDatabaseRecord::RAWDATA,     0x2c);
CHECK_SIZE(CNexusRecord::RAWDATA,        0x164);
CHECK_SIZE(CAreaRecord::RAWDATA,         0x174);
CHECK_SIZE(CSecretRecord::RAWDATA,       0x28);
CHECK_SIZE(CGameRecord::RAWDATA,         0x464);

// save data
CHECK_SIZE(TMNT2_SAVE_DATA, 0x578); // 1400

#endif /* defined(TARGET_PC) */