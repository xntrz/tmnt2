#pragma once

#include "ModuleType.hpp"

#include "Game/Component/Effect/EffectID.hpp"


class IModule;
class CCharacter;

namespace CharacterItemEffectModule
{
    MODULETYPE::VALUE GetModuleType(EFFECTID::VALUE idEffect);
    void SetItemEffectModuleEnable(CCharacter* pCharacter, MODULETYPE::VALUE type, bool bEnable);
    IModule* CreateItemEffectModule(CCharacter* pCharacter, EFFECTID::VALUE idEffect, float fTime);
};

namespace CharacterEffectModule
{
    IModule* CreateFreezeEffectModule(CCharacter* pCharacter);
    IModule* CreateBindEffectModule(CCharacter* pCharacter);
    IModule* CreateSleepEffectModule(CCharacter* pCharacter);
    IModule* CreateStunEffectModule(CCharacter* pCharacter);
    IModule* CreateDindleEffectModule(CCharacter* pCharacter);
};