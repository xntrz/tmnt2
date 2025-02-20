#include "Michelangero.hpp"

#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Player/PlayerStatus.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"


namespace Michelangero
{
    DEFINE_ENABLED_STATUS_FOR(CAttackJump, { PLAYERTYPES::STATUS_JUMP,
                                             PLAYERTYPES::STATUS_JUMP_2ND,
                                             PLAYERTYPES::STATUS_JUMP_WALL,
                                             PLAYERTYPES::STATUS_AERIAL,
                                             PLAYERTYPES::STATUS_AERIAL_MOVE });


    void CAttackJump::OnAttach(void)
    {
        Character().ChangeMotion(Michelangero::MOTIONNAMES::ATTACK_JUMP);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vVelocity);
        if (vVelocity.y > 0.0f)
            vVelocity.y = 0.0f;
        Character().SetVelocity(&vVelocity);

        RwV3d vAcceleration = Math::VECTOR3_ZERO;
        Character().GetAcceleration(&vAcceleration);
        vAcceleration.y = 19.0f;
        Character().SetAcceleration(&vAcceleration);

        CHARACTERTYPES::FLAG cflag = CHARACTERTYPES::FLAG_FIXED_DIRECTION
                                   | CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION;
        Character().ClearCharacterFlag(cflag);

        Character().ClearPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_MASK);

        ClearTimer();

        CGameSound::PlayAttackSE(m_pPlayerChr);
    };


    void CAttackJump::OnDetach(void)
    {
        Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
    };


    void CAttackJump::OnRun(void)
    {
        RwV3d vVelocity = { 0.0f, 0.0f, 0.3f };
        Character().RotateVectorByDirection(&vVelocity, &vVelocity);

        RwV3d vNowVelocity = Math::VECTOR3_ZERO;
        Character().GetVelocity(&vNowVelocity);
        Math::Vec3_Add(&vNowVelocity, &vNowVelocity, &vVelocity);
        Character().SetVelocity(&vNowVelocity);        

        if (Character().CheckPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_MASK))
        {
            ClearTimer();
            Character().ClearPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_MASK);

            RwV3d vAcceleration = Math::VECTOR3_ZERO;
            Character().GetAcceleration(&vAcceleration);
            vAcceleration.y = 19.0f;
            Character().SetAcceleration(&vAcceleration);

            Character().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
        };

        if (GetTimer() > 0.2f)
        {
            RwV3d vAcceleration = Math::VECTOR3_ZERO;
            Character().GetAcceleration(&vAcceleration);
            vAcceleration.y = 0.0f;
            Character().SetAcceleration(&vAcceleration);
            
            Character().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
            Character().SetMotionSpeed(0.4f);
        };
    };


    void CAttackJump::ClearTimer(void)
    {
        m_fStartTime = CGameProperty::GetTotalElapsedTime();
    };


    float CAttackJump::GetTimer(void) const
    {
        return (CGameProperty::GetTotalElapsedTime() - m_fStartTime);
    };


    //
    // *********************************************************************************
    //


    void CAttackAABBC::OnDischargeWave(void)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        Character().GetBodyPosition(&vPosition);

        MAGIC_GENERIC::ChargeAttackMichelangero(&vPosition, Character().GetDirection(), m_pPlayerChr, MAGIC_GENERIC::STEP_THREE);
    };

    
    //
    // *********************************************************************************
    //


    void CAttackB::OnDischargeWave(MAGIC_GENERIC::STEP step)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        Character().GetBodyPosition(&vPosition);

        MAGIC_GENERIC::ChargeAttackMichelangero(&vPosition, Character().GetDirection(), m_pPlayerChr, step);
    };
}; /* namespace Michelangero */


CMichelangero::CMichelangero(GAMETYPES::COSTUME costume)
: CPlayerCharacter("michelangero", PLAYERID::ID_MIC, costume)
{
    //
	//	Model parts:
	//		0 - model
	//		1 - right sword
	//		2 - left sword
    //
    
    CPlayerCharacter::PARAMETER parameter = { 0 };
    parameter.m_chrparameter.m_bToon            = true;
    parameter.m_chrparameter.m_pszModelName     = "michelangero";
    parameter.m_chrparameter.m_pszMotionSetName = "michelangero";
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

    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_JUMP,    new Michelangero::CAttackJump);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBC,   new Michelangero::CAttackAABBC);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::STATUS_ATTACK_B,       new Michelangero::CAttackB);

    Initialize(&parameter);

    m_pModuleMan->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, false));

    if (costume != GAMETYPES::COSTUME_SAMURAI)
    {
        m_pModuleMan->Include(new CBandanaModule(this,
                                                 m_pModel,
                                                 10,
                                                 &Michelangero::BANDANA_OFFSET,
                                                 CBandanaModule::BANDANACOLOR_ORANGE));
    };
};


CMichelangero::~CMichelangero(void)
{
    ;
};