#include "NexusInfo.hpp"

#include "Game/System/Text/GameText.hpp"


namespace
{
    struct NEXUSINFO
    {
        const char* m_pszLabel;
        GAMETEXT m_idTextStart;
    };


    static const NEXUSINFO s_aNexusInfoList[] =
    {
        { "Kitty",          GAMETEXT(1157) },
        { "Monster",        GAMETEXT(1167) },
        { "Foot",           GAMETEXT(1177) },
        { "Battle Nexus",   GAMETEXT(1187) },
    };

    
    static_assert(COUNT_OF(s_aNexusInfoList) == GAMETYPES::NEXUSID_NUM, "update me");

    
    static inline const NEXUSINFO NexusInfo(GAMETYPES::NEXUSID idNexus)
    {
        ASSERT(idNexus >= 0);
        ASSERT(idNexus < COUNT_OF(s_aNexusInfoList));

        return s_aNexusInfoList[idNexus];
    };
};


/*static*/ const wchar* CNexusInfo::GetTeamName(GAMETYPES::NEXUSID idNexus, int32 nRoundNo)
{
    ASSERT(nRoundNo >= 0);
    ASSERT(nRoundNo < GAMETYPES::STAGE_MAX);

    GAMETEXT idText = GAMETEXT(NexusInfo(idNexus).m_idTextStart + nRoundNo);
    
    const wchar* pwszText = CGameText::GetText(idText);
    ASSERT(pwszText);
    
    return pwszText;
};