#include "WallJumpSignModule.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Effect/EffectManager.hpp"


CWallJumpSignModule::CWallJumpSignModule(CPlayerCharacter* pPlayerCharacter)
: IModule(MODULETYPE::WALL_JUMP_SIGN)
, m_pPlayerCharacter(pPlayerCharacter)
, m_hEffect(0)
, m_bEffectRun(false)
{
    ASSERT(m_pPlayerCharacter);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pPlayerCharacter->GetPosition(&vPosition);
    
    m_hEffect = CEffectManager::Play(EFFECTID::ID_ALL_JUMPFAIL, &vPosition, false);
    ASSERT(m_hEffect);

    StopEffect();
};


CWallJumpSignModule::~CWallJumpSignModule(void)
{
    if (m_hEffect)
    {
        CEffectManager::Finish(m_hEffect);
        m_hEffect = 0;
    };
};


void CWallJumpSignModule::Run(void)
{
    bool bStatus = false;
    
    if (m_pPlayerCharacter->IsEnableJumpWall())
        bStatus = m_pPlayerCharacter->IsEnableChangeStatus(PLAYERTYPES::STATUS_JUMP_WALL);

    if (bStatus && !m_bEffectRun)
    {
        StartEffect();
        m_bEffectRun = true;
    }
    else if (!bStatus && m_bEffectRun)
    {
        StopEffect();
        m_bEffectRun = false;
    };

    if (m_hEffect && m_bEffectRun)
    {
        const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = m_pPlayerCharacter->GetCollisionWall();
        ASSERT(pWallInfo);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pPlayerCharacter->GetFootPosition(&vPosition);

        vPosition.x = pWallInfo->m_vJumpWallPosition.x;
        vPosition.z = pWallInfo->m_vJumpWallPosition.z;

        CEffectManager::SetPosition(m_hEffect, &vPosition);
    };
};


void CWallJumpSignModule::Draw(void)
{
    ;
};


void CWallJumpSignModule::StartEffect(void)
{
    if (m_hEffect)
        CEffectManager::SetActive(m_hEffect, true);
};


void CWallJumpSignModule::StopEffect(void)
{
    if (m_hEffect)
        CEffectManager::SetActive(m_hEffect, false);
};