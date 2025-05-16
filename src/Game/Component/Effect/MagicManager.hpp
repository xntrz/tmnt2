#pragma once

#include "MagicID.hpp"
#include "MagicTypes.hpp"


class CGameObject;
class CTracer;
class CMagic;


class CMagicManager
{
public:
    class CParameter final
    {
    private:
        friend CMagicManager;
        
    public:
        CParameter(void);
        void SetPositon(const RwV3d* pvPosition);
        void SetDirection(const RwV3d* pvDirection);
        void SetDirection(float fDirection);
        void SetObject(CGameObject* pObj);
        void SetTarget(CTracer* pTarget);
        void SetTracer(CTracer* pTracer);
        void SetSoundPlay(bool bset);

    private:
        RwV3d m_vPosition;
        RwV3d m_vDirection;
        CGameObject* m_pParent;
        CTracer* m_pTarget;
        bool m_bPlaySound;
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Run(void);
    static void Draw(RwCamera* pCamera);
    static void ChangeToCommon(void);
    static void ChangeToAttached(void);
    static uint32 RegistMagic(CMagic* pMagic);
    static uint32 Play(MAGICID::VALUE idMagic, const CParameter* pParameter);
    static uint32 Play(const char* pszMagicName, const CParameter* pParameter);
    static uint32 Play(const char* pszMagicName, const RwV3d* pvPosition, const RwV3d* pvDirection = nullptr, CGameObject* pParent = nullptr, bool bPlaySound = true);
    static uint32 Play(const char* pszMagicName, const RwV3d* pvPosition, const RwV3d* pvDirection, CGameObject* pParent, CTracer* pTarget, bool bPlaySound);
    static void SetPosition(uint32 hMagic, const RwV3d* pvPosition);
    static void GetPosition(uint32 hMagic, RwV3d* pvPosition);
    static void SetDirection(uint32 hMagic, const RwV3d* pvDirection);
    static void SetDirection(uint32 hMagic, const RwMatrix* pMatRot);
    static void SetScale(uint32 hMagic, float fScale);
    static void SetLive(uint32 hMagic, float fLivetime);
    static void SetDamage(uint32 hMagic, int32 nPower);
    static void SetStatusTime(uint32 hMagic, float fTime);
    static void SetHitAttack(uint32 hMagic, MAGICTYPES::FEATURE feature);
    static void SetChangeStatus(uint32 hMagic, int32 nStatus);
    static void SetFlyawaySpeed(uint32 hMagic, float x, float y);
    static void SetSpeed(uint32 hMagic, const RwV3d* pvVector);
    static void SetSpeed(uint32 hMagic, float fDirection, float fSpeed);
    static void SetSpeed(uint32 hMagic, float fSpeed);
    static void Finish(uint32 hMagic);
    static void End(uint32 hMagic);
    static bool IsChargeAttack(CMagic* pMagic);
};