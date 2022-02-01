#pragma once

#include "Game/Component/GameData/GameData.hpp"


struct TMNT2_SAVE_DATA
{
    COptionData::RAWDATA m_option;
    CGameRecord::RAWDATA m_record;

    bool IsValid(void) const;
};