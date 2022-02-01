#include "NexusInfo.hpp"

#include "Game/System/2d/GameText.hpp"


namespace
{
    struct NEXUSINFO
    {
        const char* m_pszLabel;
        GAMETEXT::VALUE m_idTextStart;
    };


    static const NEXUSINFO s_aNexusInfoList[] =
    {
        { "Kitty",          GAMETEXT::VALUE(1157) },
        { "Monster",        GAMETEXT::VALUE(1167) },
        { "Foot",           GAMETEXT::VALUE(1177) },
        { "Battle Nexus",   GAMETEXT::VALUE(1187) },
    };

    
    static_assert(COUNT_OF(s_aNexusInfoList) == GAMETYPES::NEXUSID_NUM, "update me");

    
    static inline const NEXUSINFO NexusInfo(GAMETYPES::NEXUSID idNexus)
    {
        ASSERT(idNexus >= 0 && idNexus < COUNT_OF(s_aNexusInfoList));
        return s_aNexusInfoList[idNexus];
    };
};


/*static*/ const wchar* CNexusInfo::GetTeamName(GAMETYPES::NEXUSID idNexus, int32 nRoundNo)
{
    ASSERT(nRoundNo >= 0 && nRoundNo < GAMETYPES::STAGE_MAX);

    GAMETEXT::VALUE idText = GAMETEXT::VALUE(NexusInfo(idNexus).m_idTextStart + nRoundNo);
    
    const wchar* pwszText = CGameText::GetText(idText);
    ASSERT(pwszText);
    
    return pwszText;
};