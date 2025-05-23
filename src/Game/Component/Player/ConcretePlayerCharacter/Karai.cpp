#include "Karai.hpp"

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


namespace Karai
{
    DEFINE_ENABLED_STATUS_FOR(CAttackJump, { PLAYERTYPES::STATUS_JUMP,
                                             PLAYERTYPES::STATUS_JUMP_2ND,
                                             PLAYERTYPES::STATUS_JUMP_WALL,
                                             PLAYERTYPES::STATUS_AERIAL,
                                             PLAYERTYPES::STATUS_AERIAL_MOVE });


    void CAttackJump::OnAttach(void)
    {
        Character().ChangeMotion(Karai::MOTIONNAMES::ATTACK_JUMP);
        
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vVelocity);
        vVelocity.y = 0.0f;
        Character().SetVelocity(&vVelocity);
        
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

        RwV3d vPositionLocal = Math::VECTOR3_ZERO;        
        Character().RotateVectorByDirection(&vPositionLocal, &Karai::CHARGE_ATTACK_LOCAL_POSITION);

        Math::Vec3_Add(&vPosition, &vPosition, &vPositionLocal);

        MAGIC_GENERIC::ChargeAttackKarai(&vPosition, Character().GetDirection(), m_pPlayerChr, MAGIC_GENERIC::STEP_THREE);
    };

    
    //
    // *********************************************************************************
    //


    void CAttackB::OnDischargeWave(MAGIC_GENERIC::STEP step)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        Character().GetBodyPosition(&vPosition);

        RwV3d vPositionLocal = Math::VECTOR3_ZERO;
        Character().RotateVectorByDirection(&vPositionLocal, &Karai::CHARGE_ATTACK_LOCAL_POSITION);

        Math::Vec3_Add(&vPosition, &vPosition, &vPositionLocal);

        MAGIC_GENERIC::ChargeAttackKarai(&vPosition, Character().GetDirection(), m_pPlayerChr, step);
    };
}; /* namespace Karai */


CKarai::CKarai(GAMETYPES::COSTUME costume)
: CPlayerCharacter("karai", PLAYERID::ID_KAR, costume)
{
    //
	//	Model parts:
	//		0 - model
	//		1 - skirt
	//		2 - left sword
	//		3 - right sword
    //
    
    CPlayerCharacter::PARAMETER parameter = { 0 };
    parameter.m_chrparameter.m_bToon            = true;
    parameter.m_chrparameter.m_pszModelName     = "karai";
    parameter.m_chrparameter.m_pszMotionSetName = "karai";
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

    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_JUMP,    new Karai::CAttackJump);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBC,   new Karai::CAttackAABBC);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_B,       new Karai::CAttackB);

    Initialize(&parameter);

    m_pModuleMan->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, false));
};


CKarai::~CKarai(void)
{
    ;
};


void CKarai::ShootingKnife(void)
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

    CShotManager::Shot(SHOTID::ID_KUNAI_PLAYER, &vPosition, m_fDirection, this, MATH_DEG2RAD(15.0f), 5.0f);

    if (!TestAttribute(PLAYERTYPES::ATTRIBUTE_INNUMERABLE_KNIFE))
        CGameProperty::Player(GetPlayerNo())->AddShurikenNum(-1);
};