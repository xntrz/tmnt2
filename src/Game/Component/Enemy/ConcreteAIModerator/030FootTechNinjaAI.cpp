#include "030FootTechNinjaAI.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"


C030FootTechNinjaAI::CTransparent::CTransparent(CEnemyCharacter* pEnemyChr)
: m_pEnemyChr(pEnemyChr)
, m_bPermission(true)
, m_bActive(false)
{
    ASSERT(m_pEnemyChr != nullptr);
};


/*virtual*/ C030FootTechNinjaAI::CTransparent::~CTransparent(void)
{
    ;
};


/*virtual*/ void C030FootTechNinjaAI::CTransparent::Run(void)
{
    if (m_bActive)
    {
        float fHP      = static_cast<float>(m_pEnemyChr->CharacterParameter().m_feature.m_iHP);
        float fHPMax   = static_cast<float>(m_pEnemyChr->CharacterParameter().m_feature.m_iHPMax);
        float fHPRatio = (fHP / fHPMax);

        float fRand = Math::RandFloat();

        float fBlinkChance = ((1.0f - fHPRatio) * fHPRatio) * 0.8f;

        if (fBlinkChance > fRand)
            SetModelDrawEnable(true);
        else
            SetModelDrawEnable(false);
    };
};


/*virtual*/ void C030FootTechNinjaAI::CTransparent::SetState(bool bActive)
{
    if (!m_bPermission)
        return;

    if (m_bActive == bActive)
        return;

    m_bActive = bActive;

    if (m_bActive)
    {
        SetModelDrawEnable(false);

        RwV3d vecOffset = Math::VECTOR3_ZERO;
        float fDir = m_pEnemyChr->Compositor().GetDirection();

        uint32 hEffect = CEffectManager::PlayTrace(EFFECTID::ID_WINDWARP_START, new CEnemyTracer(m_pEnemyChr), &vecOffset, fDir);
        ASSERT(hEffect);

        if (hEffect)
            CEffectManager::SetScale(hEffect, 0.4f);
    }
    else
    {
        SetModelDrawEnable(true);

        RwV3d vecOffset = Math::VECTOR3_ZERO;
        float fDir = m_pEnemyChr->Compositor().GetDirection();

        uint32 hEffect = CEffectManager::PlayTrace(EFFECTID::ID_WINDWARP_OUT, new CEnemyTracer(m_pEnemyChr), &vecOffset, fDir);
        ASSERT(hEffect);

        if (hEffect)
            CEffectManager::SetScale(hEffect, 0.7f);
    };

    CGameSound::PlayObjectSE(&m_pEnemyChr->Compositor(), SDCODE_SE(4324));
};


void C030FootTechNinjaAI::CTransparent::PermissionToTheStateChanging(bool bPermission)
{
    m_bPermission = bPermission;
};


void C030FootTechNinjaAI::CTransparent::SetModelDrawEnable(bool bEnable)
{
    CModel* pModel = m_pEnemyChr->Compositor().GetModel();
    ASSERT(pModel != nullptr);

    pModel->SetDrawEnable(bEnable);
};


//
// *********************************************************************************
//


/*static*/ CAIModerator* C030FootTechNinjaAI::Instance(CEnemyCharacter* pEnemyChr)
{
    return new C030FootTechNinjaAI(pEnemyChr);
};


C030FootTechNinjaAI::C030FootTechNinjaAI(CEnemyCharacter* pEnemyChr)
: CBaseFootNinjaAI(pEnemyChr)
, m_transparentControl(pEnemyChr)
, m_statusPrev(pEnemyChr->GetStatus())
{
    ;
};


/*virtual*/ C030FootTechNinjaAI::~C030FootTechNinjaAI(void)
{
    ;
};


/*virtual*/ void C030FootTechNinjaAI::Run(void) /*override*/
{
    ENEMYTYPES::STATUS statusCurr = m_pEnemyChr->GetStatus();

    if ((m_statusPrev != ENEMYTYPES::STATUS_WALK) &&
        (statusCurr == ENEMYTYPES::STATUS_WALK))
    {
        float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
        float fRand          = (Math::RandFloat() * 0.5f);

        if (fRand < fRatioOfActivity)
            SetInvisible(true);
    };

    if ((m_statusPrev != ENEMYTYPES::STATUS_IDLE)&&
        (statusCurr == ENEMYTYPES::STATUS_IDLE))
    {
        float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
        float fRand          = (Math::RandFloat() * 0.5f);

        if (fRand < fRatioOfActivity)
            SetInvisible(false);
    };

    m_statusPrev = m_pEnemyChr->GetStatus();
    m_transparentControl.Run();
    
    CBaseFootNinjaAI::Run();
};


void C030FootTechNinjaAI::SetInvisible(bool bState)
{
    m_transparentControl.SetState(bState);
};


void C030FootTechNinjaAI::CancelInvisible(void)
{
    SetInvisible(false);
    m_transparentControl.PermissionToTheStateChanging(false);
};