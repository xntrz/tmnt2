#include "Raphael.hpp"

#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace Raphael
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
        Character().ChangeMotion(Raphael::MOTIONNAMES::ATTACK_JUMP1);
        
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vVelocity);
        vVelocity.y = 0.0f;
        Character().SetVelocity(&vVelocity);

        CGameSound::PlayAttackSE(m_pPlayerChr);
        
        m_phase = PHASE_START;
    };


    void CAttackJump::OnDetach(void)
    {
        Character().ChangeMotion(Raphael::MOTIONNAMES::ATTACK_JUMP3);
    };


    void CAttackJump::OnRun(void)
    {
        if (m_phase == PHASE_START)
        {
            if (Character().IsMotionEnd())
            {
                Character().ChangeMotion(Raphael::MOTIONNAMES::ATTACK_JUMP2);
                m_phase = PHASE_REPEAT;
            };
        };
    };
    

    //
    // *********************************************************************************
    //


    void CTouchdown::OnAttach(void)
    {
        PlayerStatus::CTouchdown::OnAttach();

        if (Character().GetStatusPrev() == PLAYERTYPES::STATUS_ATTACK_JUMP)
            Character().ChangeMotion(Raphael::MOTIONNAMES::ATTACK_JUMP3);
    };

    
    //
    // *********************************************************************************
    //


    bool CAttackAAC::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        return (status == PLAYERTYPES::STATUS_ATTACK_AA);
    };


    void CAttackAAC::OnAttach(void)
    {
        Character().ChangeMotion(PLAYERTYPES::MOTIONNAMES::ATTACK_AAC);
    };


    //
    // *********************************************************************************
    //


    void CAttackAABBC::OnDischargeWave(void)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        Character().GetBodyPosition(&vPosition);

        MAGIC_GENERIC::ChargeAttackRaphael(&vPosition, Character().GetDirection(), m_pPlayerChr, MAGIC_GENERIC::STEP_THREE);
    };


    //
    // *********************************************************************************
    //


    void CAttackB::OnDischargeWave(MAGIC_GENERIC::STEP step)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        Character().GetBodyPosition(&vPosition);
        
        MAGIC_GENERIC::ChargeAttackRaphael(&vPosition, Character().GetDirection(), m_pPlayerChr, step);
    };


    //
    // *********************************************************************************
    //


    bool CPush::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::STATUS_IDLE,
            PLAYERTYPES::STATUS_WALK,
            PLAYERTYPES::STATUS_RUN,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CPush::OnAttach(void)
    {
        Character().ChangeMotion(Raphael::MOTIONNAMES::PUSH);
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
        
        CGimmickManager::PostEvent(
            pWallInfo->m_gimmickinfo.m_szGimmickObjName,
            Character().GetName(),
            GIMMICKTYPES::EVENTTYPE_ACTIVATE
        );
    };
};


CRaphael::CRaphael(GAMETYPES::COSTUME costume)
: CPlayerCharacter("raphael", PLAYERID::ID_RAP, costume)
{
    //
	//	Model parts:
	//		0 - model
	//		1 - right sword
	//		2 - left sword
    //
    
    CPlayerCharacter::PARAMETER parameter = { 0 };
    parameter.m_chrparameter.m_bToon            = true;
    parameter.m_chrparameter.m_pszModelName     = "raphael";
    parameter.m_chrparameter.m_pszMotionSetName = "raphael";
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

    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_TOUCHDOWN,      new Raphael::CTouchdown);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_PUSH,           new Raphael::CPush);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_AAC,     new Raphael::CAttackAAC);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_JUMP,    new Raphael::CAttackJump);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBC,   new Raphael::CAttackAABBC);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_B,       new Raphael::CAttackB);

    Initialize(&parameter);

    m_pModuleMan->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, true));

    if (costume != GAMETYPES::COSTUME_SAMURAI)
    {
        m_pModuleMan->Include(new CBandanaModule(
            this,
            m_pModel,
            CHARACTERTYPES::BONEID_HEAD,
            &Raphael::BANDANA_OFFSET,
            CBandanaModule::BANDANACOLOR_RED
        ));
    };
};


CRaphael::~CRaphael(void)
{
    ;
};