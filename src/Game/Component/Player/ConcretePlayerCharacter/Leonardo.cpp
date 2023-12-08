#include "Leonardo.hpp"

#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace Leonardo
{
    bool CAttackJump::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_JUMP,
            PLAYERTYPES::STATUS_JUMP_2ND,
            PLAYERTYPES::STATUS_JUMP_WALL,
            PLAYERTYPES::STATUS_AERIAL,
            PLAYERTYPES::STATUS_AERIAL_MOVE,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CAttackJump::OnAttach(void)
    {
        Character().ChangeMotion(Leonardo::MOTIONNAMES::ATTACK_JUMP);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Character().RotateVectorByDirection(&vVelocity, &Leonardo::JUMPATTACK_VELOCITY);
		Character().SetVelocity(&vVelocity);

        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION, false);
        Character().SetCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ, false);
        CGameSound::PlayAttackSE(m_pPlayerChr);
    };


    void CAttackJump::OnDetach(void)
    {
        ;
    };


    void CAttackJump::OnRun(void)
    {
        const CCharacter::COLLISIONWALLINFO* pWallInfo = Character().GetCollisionWall();
        ASSERT(pWallInfo);

        if (pWallInfo->m_bHit)
        {
            RwV3d vVelocity = Math::VECTOR3_ZERO;
            
            Character().GetVelocity(&vVelocity);
            
            vVelocity.x = 0.0f;
            vVelocity.z = 0.0f;
            
            Character().SetVelocity(&vVelocity);
        };
    };


    //
    // *********************************************************************************
    //


    void CAttackAABBC::OnDischargeWave(void)
    {
        RwV3d vPosBody = Math::VECTOR3_ZERO;
        RwV3d vDir = Math::VECTOR3_ZERO;
        RwV3d vPos = Math::VECTOR3_ZERO;
        
        Character().GetBodyPosition(&vPosBody);
        Character().RotateVectorByDirection(&vDir, &Leonardo::CHARGE_ATTACK_LOCAL_POSITION);

        Math::Vec3_Add(&vPos, &vDir, &vPosBody);
        
        MAGIC_GENERIC::ChargeAttackLeonardo(
            &vPos,
            Character().GetDirection(),
            m_pPlayerChr,
            MAGIC_GENERIC::STEP_THREE
        );
    };


    //
    // *********************************************************************************
    //


    void CAttackB::OnDischargeWave(MAGIC_GENERIC::STEP step)
    {
        RwV3d vPosBody = Math::VECTOR3_ZERO;
        RwV3d vDir = Math::VECTOR3_ZERO;
        RwV3d vPos = Math::VECTOR3_ZERO;

        Character().GetBodyPosition(&vPosBody);
        Character().RotateVectorByDirection(&vDir, &Leonardo::CHARGE_ATTACK_LOCAL_POSITION);

        Math::Vec3_Add(&vPos, &vDir, &vPosBody);

        Character().GetBodyPosition(&vPos);

        MAGIC_GENERIC::ChargeAttackLeonardo(
            &vPos,
            Character().GetDirection(),
            m_pPlayerChr,
            step
        );
    };
};



CLeonardo::CLeonardo(GAMETYPES::COSTUME costume)
: CPlayerCharacter("leonardo", PLAYERID::ID_LEO, costume)
{
    //
	//	Model parts:
	//		0 - model
	//		1 - right sword
	//		2 - left sword
    //		3 - bag
    //		4 - bag right sword handle
    //		5 - bag left sowrd handle
    //
    
    CPlayerCharacter::PARAMETER parameter = { 0 };
    parameter.m_chrparameter.m_bToon            = true;
    parameter.m_chrparameter.m_pszModelName     = "leonardo";
    parameter.m_chrparameter.m_pszMotionSetName = "leonardo";
    parameter.m_feature.m_fWalkMoveSpeed        = 2.0f;
    parameter.m_feature.m_fLiftWalkMoveSpeed    = 3.6f;
    parameter.m_feature.m_fRunMoveSpeed         = 5.2f;
    parameter.m_feature.m_fDashMoveSpeed        = 16.0f;
    parameter.m_feature.m_fDashTime             = 0.2f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 7.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 5.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_nKnifeAttachBoneID    = CHARACTERTYPES::BONEID_RIGHT_WRIST;

    parameter.m_pStateMachine = new CPlayerStateMachine(this, PLAYERTYPES::NORMALMAX);
    ASSERT(parameter.m_pStateMachine);

    CStatus::RegistDefaultForStateMachine(*parameter.m_pStateMachine);

    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_JUMP,    new Leonardo::CAttackJump);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBC,   new Leonardo::CAttackAABBC);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_B,       new Leonardo::CAttackB);

    Initialize(&parameter);

    m_pModuleMan->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, true));

    if (costume != GAMETYPES::COSTUME_SAMURAI)
    {
        m_pModuleMan->Include(new CBandanaModule(
            this,
            m_pModel,
            CHARACTERTYPES::BONEID_HEAD,
            &Leonardo::BANDANA_OFFSET,
            CBandanaModule::BANDANACOLOR_BLUE
        ));
    };    
};


CLeonardo::~CLeonardo(void)
{
    ;
};