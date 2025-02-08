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
    for (int32 i = 0; i < COUNT_OF(m_aEffectInfo); ++i)
    {
        if (m_aEffectInfo[i].m_hEffect)
            CEffectManager::Finish(m_aEffectInfo[i].m_hEffect);
    };
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
    ASSERT(hEffect0);

    if (hEffect0)
    {
        CEffectManager::SetScale(hEffect0, (fRadius / 1.5f));
        CEffectManager::SetActive(hEffect0, false);

        m_aEffectInfo[0].m_hEffect = hEffect0;
        m_aEffectInfo[0].m_bEnable = false;
    };

    uint32 hEffect1 = CEffectManager::Play(EFFECTID::ID_ALL_W_SPLASH, &vPosition, false);
    ASSERT(hEffect1);

    if (hEffect1)
    {
        CEffectManager::SetScale(hEffect1, (fRadius / 1.5f));
        CEffectManager::SetActive(hEffect1, false);

        m_aEffectInfo[1].m_hEffect = hEffect1;
        m_aEffectInfo[1].m_bEnable = false;
    };    
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

    switch (m_pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pEnemyChr = static_cast<CCharacterCompositor*>(m_pCharacter);
            ENEMYTYPES::STATUS enemyStatus = pEnemyChr->GetStatus();

            if ((enemyStatus == ENEMYTYPES::STATUS_RUN) ||
                (enemyStatus == ENEMYTYPES::STATUS_DASH))
            {
                bResult = true;
            };
        }
        break;
        
    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(m_pCharacter);
            PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
            
            if ((playerStatus == PLAYERTYPES::STATUS_RUN)         ||
                (playerStatus == PLAYERTYPES::STATUS_WALK)        ||
                (playerStatus == PLAYERTYPES::STATUS_DASH)        ||
                (playerStatus == PLAYERTYPES::STATUS_DASH_CANCEL) ||
                (playerStatus == PLAYERTYPES::STATUS_DASH_FINISH))
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