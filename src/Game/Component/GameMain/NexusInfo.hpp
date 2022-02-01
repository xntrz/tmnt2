#pragma once

#include "GameTypes.hpp"


class CNexusInfo
{
public:
    static const wchar* GetTeamName(GAMETYPES::NEXUSID idNexus, int32 nRoundNo);
};