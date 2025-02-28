#pragma once

#include "Game/System/Map/MapTypes.hpp"


class CHitAttackData;


namespace EFFECT_GENERIC
{
    enum DEATHTYPE
    {
        DEATHTYPE_NORMAL = 0,
        DEATHTYPE_MACHINE,
        DEATHTYPE_MONSTER,
    };

    void CallHitEffect(CHitAttackData* pAttack, const RwV3d* pvPosition);
    void CallDeathEffect(DEATHTYPE type, const RwV3d* pvPosition, float fScale = 1.0f, bool bPlaySound = true);
    void CallTouchDownEffect(MAPTYPES::ATTRIBUTE attribute, const RwV3d* pvPosition, float fScale = 1.0f, bool bPlaySound = true);
    bool IsNeedWaterEffect(MAPTYPES::ATTRIBUTE attribute);
};