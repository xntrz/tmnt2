#pragma once

#include "EnemyTypes.hpp"

#include "Game/Component/GameMain/MapID.hpp"


class CGameObject;
class CEnemyCharacter;
class CCharacterCompositor;


class CEnemyUtils
{
public:
    static const float INVALID_WORLD_MAP_HEIGHT;
    
    class CModelColorControl final
    {
    private:
        struct COLOR_REAL
        {
            float r;
            float g;
            float b;
        };

    public:
        static float CalcColorChange(float src, float dst, float change);

        CModelColorControl(void);
        ~CModelColorControl(void);
        void Initialize(CCharacterCompositor* pChrCompositor);
        void Period(void);
        void SetModelColorChange(float r, float g, float b, float fTime);
        void GetBaseModelColor(float* r, float* g, float* b);
        void SetModelColorControlEnable(bool bEnable);
        void SetAmbientLightEnable(bool bEnable);
        
    private:
        bool                  m_bEnable;
        RwRGBA                m_colorBase;
        COLOR_REAL            m_colorSrc;
        COLOR_REAL            m_colorDst;
        COLOR_REAL            m_colorChange;
        bool                  m_bLight;
        CCharacterCompositor* m_pChrCompositor;
    };

    class CKnockBackControl final
    {
    public:
        CKnockBackControl(void);
        ~CKnockBackControl(void);
        void Initialize(CEnemyCharacter* pEnemyChr, int32 iDamageKnockBackControl, float fTimerKnockBackControl);
        void Period(void);
        void ReceivedDamage(int32 iDamage);
        void ClearKnockBackControl(void);
        void SetKnockBackControlEnable(bool bEnable);
        void SetDamageKnockBackControl(int32 iDamageKnockBackControl);
        void SetTimeKnockBackControl(float fTimerKnockBackControl);
        float GetPlayerNumRate(void) const;

    private:
        bool             m_bControlEnabled;
        bool             m_bNowControl;
        int32            m_iDamageHeap;
        int32            m_iDamageKnockBackControl;
        float            m_fTimer;
        float            m_fTimerKnockBackControl;
        CEnemyCharacter* m_pEnemyChr;
    };

public:
    static MAPID::VALUE GetNowMapID(void);
    static bool CheckNowMapForMapID(MAPID::VALUE idMap);
    static float GetEnemyHPRate(const CEnemyCharacter* pEnemyChr);
    static float RadianCorrect(float rad);
    static uint32 GetHandleFromHitObj(const CGameObject* pObj);
    static void BezierCalc(RwV3d* result, const RwV3d* p0, const RwV3d* p1, const RwV3d* p2, float t);
    static bool SetAmbientLightEnable(CCharacterCompositor* pChrCompositor, bool bEnable);
    static bool CheckInfoWater(const CCharacterCompositor* pChrCompositor);
    static int32 FindPlayerNo(uint32 hObject);
    static void ProcAerialMotion(CCharacterCompositor* pChrCompositor, float fJumpInitSpeed);
    static void GetJumpSpeedPosToPos(RwV3d* initSpeed, float fJumpInitSpeed, const RwV3d* jumpPos, const RwV3d* markPos, float fGravity);
    static float CalcJumpTouchdownTime(float fJumpInitSpeed, float fStartheight, float fEndHeight, float fGravity);
    static float GetJumpableHeight(float fJumpInitSpeed, float fGravity);
    static float GetDistance(const RwV3d* pos, const RwV3d* at);
    static float GetDistance(const CCharacterCompositor* pChrCompositor, const RwV3d* at);
    static float GetDistanceFromPlayer(int32 playerNo, const RwV3d* pos);
    static float GetDirection(const RwV3d* pos, const RwV3d* at);
    static float GetDirection(const CCharacterCompositor* pChrCompositor, const RwV3d* at);
    static float GetDirection(const RwV3d* at);
    static int32 RotateToPlayer(CCharacterCompositor* pChrCompositor, int32 playerNo, float fRotSpeed, float fRad);
    static int32 RotateToPosition(CCharacterCompositor* pChrCompositor, const RwV3d* pos, float fRotSpeed, float fRad);
    static bool IsAttackState(ENEMYTYPES::STATUS status);
    static bool IsStunState(ENEMYTYPES::STATUS status);
    static bool IsKnockState(ENEMYTYPES::STATUS status);
    static bool IsFlyawayState(ENEMYTYPES::STATUS status);
    static bool IsDownState(ENEMYTYPES::STATUS status);
    static bool IsLiftState(ENEMYTYPES::STATUS status);
    static bool IsMoveState(ENEMYTYPES::STATUS status);
    static void EntryTouchDownEffect(const CCharacterCompositor* pChrCompositor, float fScale, bool bPlayerSound = true);
    static void EntryTouchDownEffect(const CEnemyCharacter* pEnemyChr, bool bPlaySound = true);
    static void EntryNexusDeathEffect(const CEnemyCharacter* pEnemyChr, float fScale);
    static void PlayTouchdownSE(const CCharacterCompositor* pChrCompositor);
    static void EntryTouchDownEffectAndSE(const CEnemyCharacter* pEnemyChr);
    static void EntryFlyawayBoundEffectAndSE(const CEnemyCharacter* pEnemyChr);
};


class CEnemyUtils6045
{
public:
    // TODO enemy 6045 group
};