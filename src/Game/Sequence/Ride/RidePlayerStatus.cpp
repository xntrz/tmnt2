#include "RidePlayerStatus.hpp"
#include "RideCharacter.hpp"

#include "Game/Component/GameMain/GameStage.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/System/Sound/MessageManager.hpp"
#include "Game/System/Misc/Gamepad.hpp"


namespace RidePlayerStatus
{
    void CRideStatus::ChangeMotion(PLAYERTYPES::STATUS status)
    {
        const char* pszMotionName = nullptr;
        bool bForce = false;
        
        switch (status)
        {
        case PLAYERTYPES::RIDESTATUS_RUN:
            pszMotionName = "Idle";
            break;
            
        case PLAYERTYPES::RIDESTATUS_RIGHT:
            pszMotionName = "Right";
            break;
            
        case PLAYERTYPES::RIDESTATUS_LEFT:
            pszMotionName = "Left";
            break;
            
        case PLAYERTYPES::RIDESTATUS_SPEED_UP:
            pszMotionName = "SpeedUp";
            break;
            
        case PLAYERTYPES::RIDESTATUS_SPEED_DOWN:
            pszMotionName = "SpeedDown";
            break;
            
        case PLAYERTYPES::RIDESTATUS_CRASH_WALL:
            pszMotionName = "YWall";
            break;
            
        case PLAYERTYPES::RIDESTATUS_REVIVE:
            pszMotionName = "Idle";
            break;
            
        case PLAYERTYPES::RIDESTATUS_STAGGER:
            pszMotionName = "Yurasare";
            break;
            
        case PLAYERTYPES::RIDESTATUS_JUMP:
            pszMotionName = "J1";
            break;
            
        case PLAYERTYPES::RIDESTATUS_TOUCHDOWN:
            pszMotionName = "J2";
            break;
            
        case PLAYERTYPES::RIDESTATUS_AERIAL:
            pszMotionName = "J1";
            break;
            
        case PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER:
            pszMotionName = "Yurasare";
            break;
            
        case PLAYERTYPES::RIDESTATUS_SHIP_CRASH:
            pszMotionName = "Yurasare";
            break;
            
        case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT:
            pszMotionName = "RRoll";
            break;
            
        case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT:
            pszMotionName = "LRoll";
            break;

        default:
            ASSERT(false);
            break;
        };

        Character().ChangeMotion(pszMotionName, bForce);
    };


    CRideCharacter& CRideStatus::RideCharacter(void)
    {
        return (CRideCharacter&)*m_pPlayerChr;
    };

    
    //
    // *********************************************************************************
    //

    
    void CGroundCommon::OnAttach(void)
    {
        ChangeMotion(GetID());
    };


    void CGroundCommon::OnDetach(void)
    {
        ;
    };


    void CGroundCommon::OnRun(void)
    {
        ;
    };


    //
    // *********************************************************************************
    //
    

    bool CRun::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_LEFT,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,
            PLAYERTYPES::RIDESTATUS_REVIVE,
            PLAYERTYPES::RIDESTATUS_STAGGER,
            PLAYERTYPES::RIDESTATUS_TOUCHDOWN,
            PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT,
            PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    //
    // *********************************************************************************
    //


    bool CRight::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_LEFT,            
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    //
    // *********************************************************************************
    //


    bool CLeft::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    //
    // *********************************************************************************
    //


    bool CSpeedUp::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_LEFT,        
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    //
    // *********************************************************************************
    //


    bool CSpeedDown::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_LEFT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    //
    // *********************************************************************************
    //


    bool CCrashWall::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_LEFT,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,
            PLAYERTYPES::RIDESTATUS_REVIVE,
            PLAYERTYPES::RIDESTATUS_STAGGER,
            PLAYERTYPES::RIDESTATUS_JUMP,
            PLAYERTYPES::RIDESTATUS_TOUCHDOWN,
            PLAYERTYPES::RIDESTATUS_AERIAL,
            PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CCrashWall::OnAttach(void)
    {
        RideCharacter().ResetAcceleration();
        RideCharacter().ResetVelocity();
        RideCharacter().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);
        RideCharacter().SetEnableBodyHit(false);
        RideCharacter().SetEnableCatchHit(false);
        RideCharacter().SetGravityEnable(false);
        RideCharacter().SetEffectDrawEnable(false);
        RideCharacter().ResetTurnDirection();
        RideCharacter().OnCrashWall();
        
        if (Math::RandFloat() < 0.5f)
            CMessageManager::Request(SEGROUPID::VALUE(161), RideCharacter().GetID());

        CGameEvent::SetPlayerRideAction(RideCharacter().GetPlayerNo(), GAMETYPES::RIDEACT_WALL_CRASH);
        CGamepad::StartVibration(RideCharacter().GetPadID(), CGamepad::VIBRATIONTYPE_HARD, 0.2f);
        ChangeMotion(GetID());
    };


    void CCrashWall::OnDetach(void)
    {
        RideCharacter().SetEffectDrawEnable(true);
    };


    void CCrashWall::OnRun(void)
    {
        if (StateMachine().GetStatusDuration() > 1.5f)
            StateMachine().ChangeStatus(PLAYERTYPES::RIDESTATUS_REVIVE);
    };


    //
    // *********************************************************************************
    //


    bool CRevive::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_CRASH_WALL,
            PLAYERTYPES::RIDESTATUS_SHIP_CRASH,
            PLAYERTYPES::RIDESTATUS_REVIVE,            
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CRevive::OnAttach(void)
    {
        RideCharacter().ResetAcceleration();
        RideCharacter().ResetVelocity();
        RideCharacter().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);
        RideCharacter().SetEnableBodyHit(true);
        RideCharacter().SetEnableCatchHit(true);
        RideCharacter().SetGravityEnable(false);
        RideCharacter().SetBlinkEnable(true, 1.0f);
        RideCharacter().SetTurnDirection(0.0f);
        RideCharacter().SetDirection(0.0f);
        RideCharacter().OnRevive();
        ChangeMotion(GetID());
    };


    void CRevive::OnDetach(void)
    {
        RideCharacter().SetBlinkEnable(false, 0.0f);
        RideCharacter().SetGravityEnable(true);
    };


    void CRevive::OnRun(void)
    {
        if (StateMachine().GetStatusDuration() > 1.0f)
            StateMachine().ChangeStatus(PLAYERTYPES::RIDESTATUS_RUN);
    };
    

    //
    // *********************************************************************************
    //


    bool CStagger::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_LEFT,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,
            PLAYERTYPES::RIDESTATUS_STAGGER,
            PLAYERTYPES::RIDESTATUS_TOUCHDOWN,
            PLAYERTYPES::RIDESTATUS_AERIAL,
            PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER,
            PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT,
            PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CStagger::OnAttach(void)
    {
        ChangeMotion(GetID());
    };


    void CStagger::OnDetach(void)
    {
        ;
    };


    void CStagger::OnRun(void)
    {
        if (RideCharacter().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::RIDESTATUS_RUN);
    };


    //
    // *********************************************************************************
    //

    
    bool CJump::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_LEFT,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,        
            PLAYERTYPES::RIDESTATUS_STAGGER,
            PLAYERTYPES::RIDESTATUS_TOUCHDOWN,            
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CJump::OnAttach(void)
    {
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        RideCharacter().GetVelocity(&vVelocity);
        vVelocity.y = 11.0f;
        RideCharacter().SetVelocity(&vVelocity);

        RideCharacter().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);

        if (Math::RandFloat() < 0.5f)
            CMessageManager::Request(SEGROUPID::VALUE(157), RideCharacter().GetID());

        CGameEvent::SetPlayerRideAction(RideCharacter().GetPlayerNo(), GAMETYPES::RIDEACT_JUMP);
        ChangeMotion(GetID());
    };


    void CJump::OnDetach(void)
    {
        ;
    };


    void CJump::OnRun(void)
    {
        RideCharacter().SetAerialMotionTime(11.0f);
    };

    
    //
    // *********************************************************************************
    //

    
    bool CTouchdown::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_JUMP,
            PLAYERTYPES::RIDESTATUS_AERIAL,
            PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER,            
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CTouchdown::OnAttach(void)
    {
        RideCharacter().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, false);

        if (!RideCharacter().IsShip())
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            RideCharacter().GetFootPosition(&vPosition);

            const CRideCharacter::COLLISIONGROUNDINFO* pGroundInfo = RideCharacter().GetCollisionGround();
            ASSERT(pGroundInfo);

            EFFECT_GENERIC::CallTouchDownEffect(pGroundInfo->m_attribute, &vPosition);
        };

        ChangeMotion(GetID());
    };


    void CTouchdown::OnDetach(void)
    {
        ;
    };


    void CTouchdown::OnRun(void)
    {
        if (RideCharacter().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::RIDESTATUS_RUN);
    };


    //
    // *********************************************************************************
    //

    
    bool CAerial::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_LEFT,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,
            PLAYERTYPES::RIDESTATUS_STAGGER,
            PLAYERTYPES::RIDESTATUS_TOUCHDOWN,            
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CAerial::OnAttach(void)
    {
        RideCharacter().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);
    };


    void CAerial::OnDetach(void)
    {
        ;
    };


    void CAerial::OnRun(void)
    {
        ;
    };

    
    //
    // *********************************************************************************
    //

    
    bool CAerialStagger::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_JUMP,
            PLAYERTYPES::RIDESTATUS_AERIAL,            
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CAerialStagger::OnAttach(void)
    {
        ChangeMotion(GetID());
    };


    void CAerialStagger::OnDetach(void)
    {
        ;
    };


    void CAerialStagger::OnRun(void)
    {
        ;
    };

    
    //
    // *********************************************************************************
    //

    
    bool CShipCrash::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_LEFT,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,
            PLAYERTYPES::RIDESTATUS_STAGGER,
            PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT,
            PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT,
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CShipCrash::OnAttach(void)
    {
        RideCharacter().ResetAcceleration();
        RideCharacter().ResetVelocity();
        RideCharacter().SetPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS, true);
        RideCharacter().SetEnableBodyHit(false);
        RideCharacter().SetEnableCatchHit(false);
        RideCharacter().SetGravityEnable(false);
        RideCharacter().SetEffectDrawEnable(false);
        RideCharacter().ResetTurnDirection();
        RideCharacter().OnCrashWall();

        if (Math::RandFloat() < 0.5f)
            CMessageManager::Request(SEGROUPID::VALUE(161), RideCharacter().GetID());

        CGameEvent::SetPlayerRideAction(RideCharacter().GetPlayerNo(), GAMETYPES::RIDEACT_WALL_CRASH);
        CGamepad::StartVibration(RideCharacter().GetPadID(), CGamepad::VIBRATIONTYPE_HARD, 0.2f);
        ChangeMotion(GetID());
    };


    void CShipCrash::OnDetach(void)
    {
        RideCharacter().SetEffectDrawEnable(true);
    };


    void CShipCrash::OnRun(void)
    {
        if (StateMachine().GetStatusDuration() > 1.5f)
            StateMachine().ChangeStatus(PLAYERTYPES::RIDESTATUS_REVIVE);
    };

    
    //
    // *********************************************************************************
    //

    
    bool CShipRightRoll::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_LEFT,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,            
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CShipRightRoll::OnAttach(void)
    {
        ChangeMotion(GetID());
    };


    void CShipRightRoll::OnDetach(void)
    {
        ;
    };


    void CShipRightRoll::OnRun(void)
    {
        if (RideCharacter().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::RIDESTATUS_RUN);
    };

    
    //
    // *********************************************************************************
    //

    
    bool CShipLeftRoll::IsEnableChangeStatus(PLAYERTYPES::STATUS status)
    {
        PLAYERTYPES::STATUS aStatusArray[] =
        {
            PLAYERTYPES::RIDESTATUS_RUN,
            PLAYERTYPES::RIDESTATUS_LEFT,
            PLAYERTYPES::RIDESTATUS_RIGHT,
            PLAYERTYPES::RIDESTATUS_SPEED_UP,
            PLAYERTYPES::RIDESTATUS_SPEED_DOWN,            
        };

        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));
    };


    void CShipLeftRoll::OnAttach(void)
    {
        ChangeMotion(GetID());
    };


    void CShipLeftRoll::OnDetach(void)
    {
        ;
    };


    void CShipLeftRoll::OnRun(void)
    {
        if (RideCharacter().IsMotionEnd())
            StateMachine().ChangeStatus(PLAYERTYPES::RIDESTATUS_RUN);
    };
};