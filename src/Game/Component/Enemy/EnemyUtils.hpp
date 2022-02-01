#pragma once

#include "EnemyTypes.hpp"

#include "Game/Component/GameMain/MapID.hpp"


class CEnemyCharacter;
class CCharacterCompositor;


class CEnemyUtils
{
public:
    class CModelColorControl
    {
    public:
        CModelColorControl(void);
        ~CModelColorControl(void);
        void Initialize(CCharacterCompositor* pCharacterCompositor);
        void Period(void);
        void SetModelColor(float r, float g, float b, float a);
        void GetBaseModelColor(float& r, float& g, float& b);
        void SetModelCOlorControlEnable(bool bEnable);
        void CalcColorChange(float& r, float& g, float& b);
        void SetAmbientLightEnable(bool bEnable);
        
    private:
        
    };

    class CKnockBackControl
    {
    public:

    private:
        
    };

public:
    static MAPID::VALUE GetNowMapID(void);
    static bool CheckNowMapForMapID(MAPID::VALUE idMap);
    static float GetEnemyHPRate(const CEnemyCharacter& rEnemyCharacter);
    static void EntryFlyawayBoundEffectAndSE(const CEnemyCharacter& rEnemyCharacter);
    static void EntryTouchdownEffectAndSE(const CEnemyCharacter& rEnemyCharacter);
    static void PlayTouchdownSE(CCharacterCompositor& rCharacterCompositor);
    static void EntryNexusDeathEffect(const CEnemyCharacter& rEnemyCharacter, float fScale);
};