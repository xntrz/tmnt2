#include "RippleEffectModule.hpp"

#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Map/WorldMap.hpp"


CRippleEffectModule::CRippleEffectModule(CCharacter* pCharacter, float fRadius)
: IModule(MODULETYPE::RIPPLE)
, m_pCharacter(pCharacter)
{
    ASSERT(m_pCharacter);

    InitializeEffect(fRadius);
};


CRippleEffectModule::~CRippleEffectModule(void)
{
    ;
};


void CRippleEffectModule::Run(void)
{
    const CCharacter::COLLISIONGROUNDINFO* pGroundInfo = m_pCharacter->GetCollisionGround();
    ASSERT(pGroundInfo);

    if (pGroundInfo->m_bHit &&
        EFFECT_GENERIC::IsNeedWaterEffect(pGroundInfo->m_attribute))
    {
        if (IsSubjectMoving())
            StartEffect(&m_aEffectInfo[1]);
        else
            StopEffect(&m_aEffectInfo[1]);

        StartEffect(&m_aEffectInfo[0]);
    }
    else
    {
        StopEffect(&m_aEffectInfo[0]);
        StopEffect(&m_aEffectInfo[1]);
    };
};


void CRippleEffectModule::Draw(void)
{
    ;
};


void CRippleEffectModule::SetEnable(bool bEnable)
{
    if (!bEnable)
    {
        StopEffect(&m_aEffectInfo[0]);
        StopEffect(&m_aEffectInfo[1]);
    };
};


void CRippleEffectModule::InitializeEffect(float fRadius)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCharacter->GetFootPosition(&vPosition);

    uint32 hEffect0 = CEffectManager::Play(EFFECTID::ID_ALL_W_HAMON, &vPosition, false);
    uint32 hEffect1 = CEffectManager::Play(EFFECTID::ID_ALL_W_SPLASH, &vPosition, false);

    CEffectManager::SetScale(hEffect0, (fRadius * 0.6f));
    CEffectManager::SetScale(hEffect1, (fRadius * 0.6f));
    
    CEffectManager::SetActive(hEffect0, false);
    CEffectManager::SetActive(hEffect1, false);

    m_aEffectInfo[0].m_hEffect = hEffect0;
    m_aEffectInfo[0].m_bEnable = false;
    m_aEffectInfo[1].m_hEffect = hEffect1;
    m_aEffectInfo[1].m_bEnable = false;
};


void CRippleEffectModule::StartEffect(RIPPLE_EFFECT_INFO* pInfo)
{
    ASSERT(pInfo);
    ASSERT(pInfo->m_hEffect);

    if (!pInfo->m_bEnable)
    {
        CEffectManager::SetActive(pInfo->m_hEffect, true);
        pInfo->m_bEnable = true;
    };

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCharacter->GetFootPosition(&vPosition);
    
    CWorldMap::GetWaterSurfacePosition(&vPosition);
    
    CEffectManager::SetPosition(pInfo->m_hEffect, &vPosition);
};


void CRippleEffectModule::StopEffect(RIPPLE_EFFECT_INFO* pInfo)
{
    ASSERT(pInfo);
    ASSERT(pInfo->m_hEffect);
    
    if (pInfo->m_bEnable)
    {
        CEffectManager::SetActive(pInfo->m_hEffect, false);
        pInfo->m_bEnable = false;
    };
};


bool CRippleEffectModule::IsSubjectMoving(void) const
{
    bool bResult = false;

    ASSERT(m_pCharacter);
    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pEnemyChr = (CCharacterCompositor*)m_pCharacter;
            if (pEnemyChr->GetStatus() == ENEMYTYPES::STATUS_RUN ||
                pEnemyChr->GetStatus() == ENEMYTYPES::STATUS_DASH)
            {
                bResult = true;
            };
        }
        break;
        
    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = (CPlayerCharacter*)m_pCharacter;
            if (pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_RUN ||
                pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_WALK ||
                pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_DASH ||
                pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_DASH_CANCEL ||
                pPlayerChr->GetStatus() == PLAYERTYPES::STATUS_DASH_FINISH)
            {
                bResult = true;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};