#include "Casey.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace Casey
{
    DEFINE_ENABLED_STATUS_FOR(CAttackJump, { PLAYERTYPES::STATUS_JUMP,
                                             PLAYERTYPES::STATUS_JUMP_2ND,
                                             PLAYERTYPES::STATUS_JUMP_WALL,
                                             PLAYERTYPES::STATUS_AERIAL,
                                             PLAYERTYPES::STATUS_AERIAL_MOVE });    


    void CAttackJump::OnAttach(void)
    {
        Character().ChangeMotion(Casey::MOTIONNAMES::ATTACK_JUMP);
        CGameSound::PlayAttackSE(m_pPlayerChr);
    };


    void CAttackJump::OnDetach(void)
    {
        ;
    };


    void CAttackJump::OnRun(void)
    {
        ;
    };

    
    //
    // *********************************************************************************
    //


    void CAttackAABBC::OnDischargeWave(void)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        Character().GetBodyPosition(&vPosition);

        MAGIC_GENERIC::ChargeAttackCasey(&vPosition, Character().GetDirection(), m_pPlayerChr, MAGIC_GENERIC::STEP_THREE);
    };

    
    //
    // *********************************************************************************
    //


    void CAttackB::OnDischargeWave(MAGIC_GENERIC::STEP step)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        Character().GetBodyPosition(&vPosition);

        MAGIC_GENERIC::ChargeAttackCasey(&vPosition, Character().GetDirection(), m_pPlayerChr, step);
    };


    //
    // *********************************************************************************
    //


    DEFINE_ENABLED_STATUS_FOR(CPush, { PLAYERTYPES::STATUS_IDLE,
                                       PLAYERTYPES::STATUS_WALK,
                                       PLAYERTYPES::STATUS_RUN });


    void CPush::OnAttach(void)
    {
        Character().ChangeMotion(Casey::MOTIONNAMES::PUSH);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vVelocity);
        vVelocity.y = 0.0f;
        Character().SetVelocity(&vVelocity);
    };


    void CPush::OnDetach(void)
    {
        ;
    };


    void CPush::OnRun(void)
    {
        RwV3d vVelocity = { 0.0f, 0.0f, Character().Feature().m_fRunMoveSpeed };
        Character().RotateVectorByDirection(&vVelocity, &vVelocity);
        Character().SetVelocity(&vVelocity);

        const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = Character().GetCollisionWall();
        ASSERT(pWallInfo);
        ASSERT(pWallInfo->m_gimmickinfo.m_szGimmickObjName[0] != '\0');

        CGimmickManager::PostEvent(pWallInfo->m_gimmickinfo.m_szGimmickObjName,
                                   Character().GetName(),
                                   GIMMICKTYPES::EVENTTYPE_ACTIVATE);
    };
}; /* namespace Casey */


CCasey::CCasey(GAMETYPES::COSTUME costume)
: CPlayerCharacter("casey", PLAYERID::ID_CAS, costume)
{
    //
	//	Model parts:
	//		0 - model
	//		1 - weapon
	//		2 - bag
	//		3 - mask
    //

    CPlayerCharacter::PARAMETER parameter = { 0 };
    parameter.m_chrparameter.m_bToon            = true;
    parameter.m_chrparameter.m_pszModelName     = "casey";
    parameter.m_chrparameter.m_pszMotionSetName = "casey";
    parameter.m_feature.m_fWalkMoveSpeed        = 2.0f;
    parameter.m_feature.m_fLiftWalkMoveSpeed    = 3.6f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.2f;
    parameter.m_feature.m_fDashMoveSpeed        = 16.0f;
    parameter.m_feature.m_fDashTime             = 0.2f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_nKnifeAttachBoneID    = 3;

    parameter.m_pStateMachine = new CPlayerStateMachine(this, PLAYERTYPES::STATUS::NORMALMAX);

    CStatus::RegistDefaultForStateMachine(*parameter.m_pStateMachine);

    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_PUSH,           new Casey::CPush);    
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_JUMP,    new Casey::CAttackJump);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBC,   new Casey::CAttackAABBC);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_B,       new Casey::CAttackB);

    Initialize(&parameter);

    m_pModuleMan->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, false));
};


CCasey::~CCasey(void)
{
    ;
};


void CCasey::OnChangeMotion(void)
{
    CPlayerCharacter::OnChangeMotion();
    
    //m_pModel->SetPartsDrawEnable(3, true);
};


void CCasey::ShootingKnife(void)
{
    RwV3d vPosition = *m_pModel->GetBonePositionFromID(Feature().m_nKnifeAttachBoneID);
    RwV3d vLocalPos = Math::VECTOR3_ZERO;

    Math::Vec3_Sub(&vLocalPos, &vPosition, &m_vPosition);
    float fBuffY = vLocalPos.y;
    vLocalPos.y = 0.0f;

    float fLen = Math::Vec3_Length(&vLocalPos);
    if (fLen > 0.6f)
    {
        vLocalPos.x *= (0.5f / fLen);
        vLocalPos.z *= (0.5f / fLen);
        vLocalPos.y = fBuffY;

        Math::Vec3_Add(&vPosition, &vLocalPos, &m_vPosition);
    };

    uint32 hShot = CShotManager::Shot(SHOTID::ID_PACK, &vPosition, m_fDirection, this, MATH_DEG2RAD(15.0f), 5.0f);
    ASSERT(hShot);

    if (!TestAttribute(PLAYERTYPES::ATTRIBUTE_INNUMERABLE_KNIFE))
        CGameProperty::Player(GetPlayerNo())->AddShurikenNum(-1);
};