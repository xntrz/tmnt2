#include "GameToon.hpp"

#include "Game/System/Model/ToonManager.hpp"


static const char* s_apszToonTextureSetName[]
{
    "toonPC",
    "toonPC",
    "toonPC",
};


namespace GameToon
{
    void SetTextureSetOfStage(STAGEID::VALUE idStage)
    {
        ASSERT(s_apszToonTextureSetName[0]);

        CToonManager::SetTextureSet(s_apszToonTextureSetName[0]);
    };
};