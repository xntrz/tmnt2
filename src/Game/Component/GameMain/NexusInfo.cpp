#include "NexusInfo.hpp"

#include "Game/System/Text/GameText.hpp"


struct NEXUSINFO
{
    const char* name;
    GAMETEXT    textIdStart;
};


static const NEXUSINFO s_aNexusInfo[] =
{
    { "Kitty",          GAMETEXT_NEXUS_A_TEAM_1 },
    { "Monster",        GAMETEXT_NEXUS_B_TEAM_1 },
    { "Foot",           GAMETEXT_NEXUS_C_TEAM_1 },
    { "Battle Nexus",   GAMETEXT_NEXUS_D_TEAM_1 },
};


static_assert(COUNT_OF(s_aNexusInfo) == GAMETYPES::NEXUSID_NUM, "update me");


static inline const NEXUSINFO& NexusInfo(GAMETYPES::NEXUSID idNexus)
{
    ASSERT(idNexus >= 0);
    ASSERT(idNexus < COUNT_OF(s_aNexusInfo));

    return s_aNexusInfo[idNexus];
};


/*static*/ const wchar* CNexusInfo::GetTeamName(GAMETYPES::NEXUSID idNexus, int32 nRoundNo)
{
    ASSERT(nRoundNo >= 0);
    ASSERT(nRoundNo < GAMETYPES::STAGE_MAX);

    GAMETEXT idText = GAMETEXT(NexusInfo(idNexus).textIdStart + nRoundNo);
    
    const wchar* pwszText = CGameText::GetText(idText);
    ASSERT(pwszText);
    
    return pwszText;
};