#include "CharacterEffectModule.hpp"
#include "Module.hpp"
#ifdef _DEBUG
#include "ModuleDebug.hpp"
#endif /* _DEBUG */

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/IceBlock.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Player/PlayerTracer.hpp"
#include "Game/System/Character/Character.hpp"
#ifdef _DEBUG
#include "Game/System/Misc/DebugShape.hpp"
#endif /* _DEBUG */


namespace EFFECTNAME
{
    static const char* STUN        = "all_piyori";
    static const char* SLEEP       = "all_sleep";
    static const char* WAKE        = "all_wake";
    static const char* BIND        = "all_nenshi1";
    static const char* BIND_OFF    = "all_nencut";
};


class CCharacterEffectModule : public IModule
{
public:
    CCharacterEffectModule(CCharacter* pCharacter);
    CCharacterEffectModule(MODULETYPE::VALUE type, CCharacter* pCharacter);
    virtual ~CCharacterEffectModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    virtual bool IsEnd(void) = 0;
    void PlayEffect(const char* pszEffectName, const RwV3d* pvPosition);
    void PlayEffect(const char* pszEffectName, CCharacter* pCharacter, const RwV3d* pvOffset = nullptr);
    void PlayEffect(EFFECTID::VALUE idEffect, CCharacter* pCharacter, const RwV3d* pvOffset = nullptr);
    void StopEffect(void);

protected:
    CCharacter* m_pCharacter;
    RwV3d m_vEffectPositon;
    RwV3d m_vEffectOffset;
    uint32 m_hEffect;
    bool m_bTracking;
    float m_fRunTime;
};


class CDindleEffectModule : public CCharacterEffectModule
{
public:
    CDindleEffectModule(CCharacter* pCharacter);
    virtual ~CDindleEffectModule(void);
    virtual bool IsEnd(void) override;
};


class CStunEffectModule : public CCharacterEffectModule
{
public:
    CStunEffectModule(CCharacter* pCharacter);
    virtual ~CStunEffectModule(void);
    virtual bool IsEnd(void) override;
};


class CSleepEffectModule : public CCharacterEffectModule
{
public:
    CSleepEffectModule(CCharacter* pCharacter);
    virtual ~CSleepEffectModule(void);
    virtual bool IsEnd(void) override;
};


class CBindEffectModule : public CCharacterEffectModule
{
public:
    CBindEffectModule(CCharacter* pCharacter);
    virtual ~CBindEffectModule(void);
    virtual bool IsEnd(void) override;
};


class CFreezeEffectModule : public CCharacterEffectModule
{
public:
    CFreezeEffectModule(CCharacter* pCharacter);
    virtual ~CFreezeEffectModule(void);
    virtual void Run(void) override;
    virtual bool IsEnd(void) override;
    void FinishIceBlock(void);

private:
    uint32 m_hIceblock;
};


class CItemEffectModule : public CCharacterEffectModule
{
public:
    CItemEffectModule(CCharacter* pCharacter, EFFECTID::VALUE idEffect, MODULETYPE::VALUE type);
    virtual ~CItemEffectModule(void);
    virtual void Run(void) override;
    virtual bool IsEnd(void) override;
    void Remove(void);
    void SetEnable(bool bEnable);
    void SetTime(float fTime);

private:
    float m_fTimer;
};


CCharacterEffectModule::CCharacterEffectModule(CCharacter* pCharacter)
: IModule(MODULETYPE::RIDE_EFFECT)
, m_pCharacter(pCharacter)
, m_vEffectPositon(Math::VECTOR3_ZERO)
, m_vEffectOffset(Math::VECTOR3_ZERO)
, m_hEffect(0)
, m_bTracking(false)
, m_fRunTime(0.0f)
{
    ASSERT(m_pCharacter);
};


CCharacterEffectModule::CCharacterEffectModule(MODULETYPE::VALUE type, CCharacter* pCharacter)
: IModule(type)
, m_pCharacter(pCharacter)
, m_vEffectPositon(Math::VECTOR3_ZERO)
, m_vEffectOffset(Math::VECTOR3_ZERO)
, m_hEffect(0)
, m_bTracking(false)
{
    ASSERT(m_pCharacter);
};


CCharacterEffectModule::~CCharacterEffectModule(void)
{
    StopEffect();
};


void CCharacterEffectModule::Run(void)
{
    m_fRunTime += CGameProperty::GetElapsedTime();

    if (IsEnd())
    {
        StopEffect();
        unlink();
        delete this;
    }
    else
    {
        if (m_hEffect && m_bTracking)
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_pCharacter->GetFootPosition(&vPosition);

            Math::Vec3_Add(&m_vEffectPositon, &vPosition, &m_vEffectOffset);
            CEffectManager::SetPosition(m_hEffect, &m_vEffectPositon);
        };
    };
};


void CCharacterEffectModule::Draw(void)
{
#ifdef _DEBUG
    if (!CModuleDebug::SHOW_CHR_STATUS_DURATION)
        return;

    /* init pos */
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    m_pCharacter->GetFootPosition(&vecMyPos);

    float fHeight = m_pCharacter->GetCollisionParameter().m_fHeight;

    vecMyPos.y += (fHeight * 1.75f);

    /* get status current & max time */
    char szStatusTime[128];
    szStatusTime[0] = '\0';

    float fNowTime = 0.0f;
    float fMaxTime = 0.0f;

    CCharacter::TYPE chrType = m_pCharacter->GetCharacterType();
    switch (chrType)
    {
    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(m_pCharacter);

            fNowTime = pPlayerChr->GetStatusDuration();
            fMaxTime = pPlayerChr->AttackParameter().m_fTroubleTime;

            std::sprintf(szStatusTime, "%.02f / %.02f", fNowTime, fMaxTime);
        }
        break;

    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(m_pCharacter);

            fNowTime = m_fRunTime; // enemy can't reduce effect duration (recovering) by input combination so runtime is enough there
            fMaxTime = pChrCompositor->AttackParameter().m_fTroubleTime;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    std::sprintf(szStatusTime, "%.02f / %.02f", fNowTime, fMaxTime);

    RwRGBA color = { 0xFF, 0x40, 0x40, 0xFF }; // red

    CDebugShape::m_fLabelHeight = 18.0f;
    CDebugShape::m_fLabelOffsetY = 0.0f;
    CDebugShape::ShowLabel(&vecMyPos, szStatusTime, color);
#endif /* _DEBUG */    
};


void CCharacterEffectModule::PlayEffect(const char* pszEffectName, const RwV3d* pvPosition)
{
    ASSERT(pszEffectName);
    ASSERT(m_hEffect == 0);
    ASSERT(pvPosition);
    
    m_hEffect = CEffectManager::Play(pszEffectName, pvPosition);
    m_vEffectPositon = *pvPosition;

    m_pCharacter->GetFootPosition(&m_vEffectOffset);
    Math::Vec3_Sub(&m_vEffectOffset, &m_vEffectPositon, &m_vEffectOffset);
};


void CCharacterEffectModule::PlayEffect(const char* pszEffectName, CCharacter* pCharacter, const RwV3d* pvOffset)
{
    ASSERT(pszEffectName);
    ASSERT(pCharacter);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    pCharacter->GetPosition(&vPosition);
    if (pvOffset)
    {
        Math::Vec3_Add(&vPosition, &vPosition, pvOffset);
    }
    else
    {
        m_vEffectOffset = Math::VECTOR3_ZERO;
    };

    PlayEffect(pszEffectName, &vPosition);
    
    if (pvOffset)
        m_vEffectOffset = *pvOffset;
};


void CCharacterEffectModule::PlayEffect(EFFECTID::VALUE idEffect, CCharacter* pCharacter, const RwV3d* pvOffset)
{
    RwV3d vOffset = Math::VECTOR3_ZERO;

    if (pvOffset)
        vOffset = *pvOffset;

    CTracer* pTracer = nullptr;
    
    switch (pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pEnemyChr = static_cast<CCharacterCompositor*>(pCharacter);
            pTracer = new CEnemyTracer(pEnemyChr);
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(pCharacter);
            pTracer = new CPlayerTracer(pPlayerChr);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    m_hEffect = CEffectManager::PlayTrace(idEffect, pTracer, &vOffset);
    ASSERT(m_hEffect);    
};


void CCharacterEffectModule::StopEffect(void)
{
    if (m_hEffect && !CEffectManager::IsEnd(m_hEffect))
    {
        CEffectManager::Finish(m_hEffect);
        m_hEffect = 0;
    };
};


//
// *********************************************************************************
//


CDindleEffectModule::CDindleEffectModule(CCharacter* pCharacter)
: CCharacterEffectModule(pCharacter)
{
    RwV3d vOffset = { 0.0f, 0.0f, 0.0f };
    PlayEffect(EFFECTID::ID_BIRIBIRI, m_pCharacter, &vOffset);

    m_bTracking = true;
};


CDindleEffectModule::~CDindleEffectModule(void)
{
    StopEffect();
};


bool CDindleEffectModule::IsEnd(void)
{
    bool bResult = true;

    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pEnemyChr = static_cast<CCharacterCompositor*>(m_pCharacter);
            if (pEnemyChr->GetStatus() == ENEMYTYPES::STATUS_DINDLE)
                bResult = false;
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(m_pCharacter);
            if (pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_DINDLE)
                bResult = false;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


//
// *********************************************************************************
//


CStunEffectModule::CStunEffectModule(CCharacter* pCharacter)
: CCharacterEffectModule(pCharacter)
{
    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            RwV3d vFootPos = Math::VECTOR3_ZERO;
            m_pCharacter->GetFootPosition(&vFootPos);

            RwV3d vBodyPos = Math::VECTOR3_ZERO;
            m_pCharacter->GetBodyPosition(&vBodyPos);

            RwV3d vOffset = { 0.0f, (vBodyPos.y - vFootPos.y), 0.0f };
            vOffset.y = std::fabs(vOffset.y);
            vOffset.y *= 2.0f;

            PlayEffect(EFFECTNAME::STUN, m_pCharacter, &vOffset);
            m_bTracking = true;
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            RwV3d vOffset = { 0.0f, 1.5f, 0.0f };
            PlayEffect(EFFECTNAME::STUN, m_pCharacter, &vOffset);
            m_bTracking = true;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


CStunEffectModule::~CStunEffectModule(void)
{
    StopEffect();    
};


bool CStunEffectModule::IsEnd(void)
{
    bool bResult = true;

    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pEnemyChr = static_cast<CCharacterCompositor*>(m_pCharacter);
            if (pEnemyChr->GetStatus() == ENEMYTYPES::STATUS_STUN)
                bResult = false;
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(m_pCharacter);
            if (pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_STUN)
                bResult = false;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


//
// *********************************************************************************
//


CSleepEffectModule::CSleepEffectModule(CCharacter* pCharacter)
: CCharacterEffectModule(pCharacter)
{
    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            RwV3d vFootPos = Math::VECTOR3_ZERO;
            m_pCharacter->GetFootPosition(&vFootPos);

            RwV3d vBodyPos = Math::VECTOR3_ZERO;
            m_pCharacter->GetBodyPosition(&vBodyPos);

            RwV3d vOffset = { 0.0f, (vBodyPos.y - vFootPos.y), 0.0f };
            vOffset.y = std::fabs(vOffset.y);
            vOffset.y *= 2.0f;

            PlayEffect(EFFECTNAME::SLEEP, m_pCharacter, &vOffset);
            m_bTracking = true;
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            RwV3d vOffset = { 0.0f, 1.5f, 0.0f };
            PlayEffect(EFFECTNAME::SLEEP, m_pCharacter, &vOffset);
            m_bTracking = true;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


CSleepEffectModule::~CSleepEffectModule(void)
{
    StopEffect();
};


bool CSleepEffectModule::IsEnd(void)
{
    bool bResult = true;

    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pEnemyChr = static_cast<CCharacterCompositor*>(m_pCharacter);
            if (pEnemyChr->GetStatus() == ENEMYTYPES::STATUS_SLEEP)
                bResult = false;
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(m_pCharacter);
            if (pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_SLEEP)
                bResult = false;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (bResult)
        CEffectManager::Play(EFFECTNAME::WAKE, &m_vEffectPositon);

    return bResult;
};


//
// *********************************************************************************
//


CBindEffectModule::CBindEffectModule(CCharacter* pCharacter)
: CCharacterEffectModule(pCharacter)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCharacter->GetFootPosition(&vPosition);
    
    PlayEffect(EFFECTNAME::BIND, &vPosition);
    
    m_bTracking = true;
};


CBindEffectModule::~CBindEffectModule(void)
{
    StopEffect();
};


bool CBindEffectModule::IsEnd(void)
{
    bool bResult = true;

    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pEnemyChr = static_cast<CCharacterCompositor*>(m_pCharacter);
            if (pEnemyChr->GetStatus() == ENEMYTYPES::STATUS_BIND)
                bResult = false;
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(m_pCharacter);
            if (pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_BIND)
                bResult = false;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (bResult)
        CEffectManager::Play(EFFECTNAME::BIND_OFF, &m_vEffectPositon);

    return bResult;
};


//
// *********************************************************************************
//


CFreezeEffectModule::CFreezeEffectModule(CCharacter* pCharacter)
: CCharacterEffectModule(pCharacter)
, m_hIceblock(0)
{
    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_pCharacter->GetFootPosition(&vPosition);
            
            m_hIceblock = CIceBlockManager::Play(&vPosition);
            ASSERT(m_hIceblock);

            if (m_hIceblock)
            {
                m_vEffectPositon = vPosition;

                float fRadius = pCharacter->GetCollisionParameter().m_fRadius;
                fRadius *= 2.0f;

				RwV3d vScaling = { fRadius, fRadius * 1.5f, fRadius };
                CIceBlockManager::SetScale(m_hIceblock, &vScaling);
            };
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_pCharacter->GetFootPosition(&vPosition);
            
            m_hIceblock = CIceBlockManager::Play(&vPosition);
            ASSERT(m_hIceblock);

            if (m_hIceblock)
                m_vEffectPositon = vPosition;            
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


CFreezeEffectModule::~CFreezeEffectModule(void)
{
    if (m_hIceblock)
    {
        CIceBlockManager::Finish(m_hIceblock);
        m_hIceblock = 0;
    };
};


void CFreezeEffectModule::Run(void)
{
    if (m_hIceblock)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pCharacter->GetFootPosition(&vPosition);

        CIceBlockManager::SetPosition(m_hIceblock, &vPosition);

        CCharacterEffectModule::Run();
    };
};


bool CFreezeEffectModule::IsEnd(void)
{
    bool bResult = true;

    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pEnemyChr = static_cast<CCharacterCompositor*>(m_pCharacter);
            if (pEnemyChr->GetStatus() == ENEMYTYPES::STATUS_FREEZE)
                bResult = false;
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(m_pCharacter);
            if (pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_FREEZE)
                bResult = false;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (bResult)
        FinishIceBlock();

    return bResult;
};


void CFreezeEffectModule::FinishIceBlock(void)
{
    ASSERT(m_hIceblock);

    if (m_hIceblock)
    {
        CIceBlockManager::Finish(m_hIceblock);
        m_hIceblock = 0;

        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pCharacter->GetFootPosition(&vPosition);

        CEffectManager::Play(EFFECTID::ID_ALL_FREEZEND, &vPosition);
    };
};


//
// *********************************************************************************
//


CItemEffectModule::CItemEffectModule(CCharacter* pCharacter, EFFECTID::VALUE idEffect, MODULETYPE::VALUE type)
: CCharacterEffectModule(type, pCharacter)
, m_fTimer(CGameProperty::GetTotalElapsedTime())
{
    ASSERT(m_pCharacter);

    PlayEffect(idEffect, m_pCharacter);
};


CItemEffectModule::~CItemEffectModule(void)
{
    ;
};


void CItemEffectModule::Run(void)
{
    if (IsEnd())
        Remove();
};


bool CItemEffectModule::IsEnd(void)
{
    return ((CGameProperty::GetTotalElapsedTime() - m_fTimer) > 15.0f);
};


void CItemEffectModule::Remove(void)
{
    StopEffect();
    unlink();
    delete this;
};


void CItemEffectModule::SetEnable(bool bEnable)
{
    if (!bEnable)
        Remove();
};


void CItemEffectModule::SetTime(float fTime)
{
    m_fTimer = (fTime + (CGameProperty::GetTotalElapsedTime() - 15.0f));
};


//
// *********************************************************************************
//


namespace CharacterItemEffectModule
{
    MODULETYPE::VALUE GetModuleType(EFFECTID::VALUE idEffect)
    {
        MODULETYPE::VALUE Result = MODULETYPE::INVALID;

        switch (idEffect)
        {
        case EFFECTID::ID_SETUP_METUKEI:
            Result = MODULETYPE::ITEM_EFFECT_INVINCIBLE;
            break;
            
        case EFFECTID::ID_SETUP_ATTACK:
            Result = MODULETYPE::ITEM_EFFECT_ATTACK;
            break;
            
        case EFFECTID::ID_SETUP_CHARGE:
            Result = MODULETYPE::ITEM_EFFECT_CHARGE;
            break;
            
        case EFFECTID::ID_SETUP_DEFENCE:
            Result = MODULETYPE::ITEM_EFFECT_DEFENCE;
            break;
            
        case EFFECTID::ID_SETUP_KNIFE:
            Result = MODULETYPE::ITEM_EFFECT_DART;
            break;

        default:
            break;
        };

        return Result;
    };


    void SetItemEffectModuleEnable(CCharacter* pCharacter, MODULETYPE::VALUE type, bool bEnable)
    {
        CItemEffectModule* pEffectMod = static_cast<CItemEffectModule*>(pCharacter->GetModule(type));
        if (pEffectMod)
            pEffectMod->SetEnable(bEnable);
    };


    IModule* CreateItemEffectModule(CCharacter* pCharacter, EFFECTID::VALUE idEffect, float fTime)
    {
        MODULETYPE::VALUE moduleType = GetModuleType(idEffect);
        ASSERT(moduleType != MODULETYPE::INVALID);

        if (moduleType == MODULETYPE::INVALID)
            return nullptr;

        if (pCharacter->IsModuleIncluded(moduleType))
        {
            CItemEffectModule* pEffectMod = static_cast<CItemEffectModule*>(pCharacter->GetModule(moduleType));
            pEffectMod->Remove();
        };

        CItemEffectModule* pModule = new CItemEffectModule(pCharacter, idEffect, moduleType);
        if (pModule)
            pModule->SetTime(fTime);

        return pModule;
    };
} /* namespace CharacterItemEffectModule */


//
// *********************************************************************************
//


namespace CharacterEffectModule
{
    IModule* CreateFreezeEffectModule(CCharacter* pCharacter)
    {
        return new CFreezeEffectModule(pCharacter);
    };

    
    IModule* CreateBindEffectModule(CCharacter* pCharacter)
    {
        return new CBindEffectModule(pCharacter);
    };

    
    IModule* CreateSleepEffectModule(CCharacter* pCharacter)
    {
        return new CSleepEffectModule(pCharacter);
    };
    

    IModule* CreateStunEffectModule(CCharacter* pCharacter)
    {
        return new CStunEffectModule(pCharacter);
    };
    

    IModule* CreateDindleEffectModule(CCharacter* pCharacter)
    {
        return new CDindleEffectModule(pCharacter);
    };
} /* namespace CharacterEffectModule */