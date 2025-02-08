#include "RideCharacter.hpp"
#include "RideCharacterTracer.hpp"
#include "RidePlayerStatus.hpp"
#include "RideStage.hpp"

#include "Game/Component/Player/PlayerStateMachine.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Module/RideCharacterEffectModule.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"
#include "System/Common/Screen.hpp"


/*static*/ CRideCharacter* CRideCharacter::New(PLAYERID::VALUE idChr, CRidePlayer* pRidePlayer, GAMETYPES::COSTUME costume)
{
    char szPlayerName[GAMEOBJECTTYPES::NAME_MAX];
    szPlayerName[0] = '\0';

    std::strcpy(szPlayerName, PLAYERID::GetName(idChr));
    std::strcat(szPlayerName, "_r");

    CRideCharacter* pRideChr = new CRideCharacter(szPlayerName, idChr, pRidePlayer, costume);
    return pRideChr;
};


/*static*/ void CRideCharacter::Delete(CRideCharacter* pRideCharacter)
{
    ASSERT(pRideCharacter);
    delete pRideCharacter;
};


CRideCharacter::CRideCharacter(const char* pszName, PLAYERID::VALUE idPlayer, CRidePlayer* pRidePlayer, GAMETYPES::COSTUME costume)
: CPlayerCharacter(pszName, idPlayer, costume)
, m_fControlRate(1.0f)
, m_fTurnDirection(0.0f)
, m_fPlaytime(0.0f)
, m_pPlayer(pRidePlayer)
, m_nNumDispShot(0)
{
    m_aScore[RIDETYPES::SCOREKIND_GOLD] = 0;
    m_aScore[RIDETYPES::SCOREKIND_SILVER] = 0;

    CPlayerCharacter::PARAMETER parameter = { 0 };
    parameter.m_chrparameter.m_bToon = true;
    parameter.m_chrparameter.m_pszModelName = PLAYERID::GetName(idPlayer);
    parameter.m_chrparameter.m_pszMotionSetName = PLAYERID::GetName(idPlayer);
    parameter.m_pStateMachine = new CPlayerStateMachine(this, PLAYERTYPES::RIDEMAX);
    ASSERT(parameter.m_pStateMachine);

    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_RUN,                new RidePlayerStatus::CRun);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_RIGHT,              new RidePlayerStatus::CRight);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_LEFT,               new RidePlayerStatus::CLeft);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_SPEED_UP,           new RidePlayerStatus::CSpeedUp);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_SPEED_DOWN,         new RidePlayerStatus::CSpeedDown);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_CRASH_WALL,         new RidePlayerStatus::CCrashWall);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_REVIVE,             new RidePlayerStatus::CRevive);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_STAGGER,            new RidePlayerStatus::CStagger);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_JUMP,               new RidePlayerStatus::CJump);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_TOUCHDOWN,          new RidePlayerStatus::CTouchdown);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_AERIAL,             new RidePlayerStatus::CAerial);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER,     new RidePlayerStatus::CAerialStagger);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_SHIP_CRASH,         new RidePlayerStatus::CShipCrash);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT,    new RidePlayerStatus::CShipRightRoll);
    parameter.m_pStateMachine->RegistStatus(PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT,     new RidePlayerStatus::CShipLeftRoll);

    Initialize(&parameter);

    if (CRideStage::GetShadowFlag())
        m_pModuleMan->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, true));

    m_pModuleMan->Include(
        CRideCharacterEffectModule::New(this, IsShip() ? CRideCharacterEffectModule::RIDERTYPE_SHIP : CRideCharacterEffectModule::RIDERTYPE_BOARD)
    );

    SetEffectDrawEnable(false);
    SetGravityEnable(true);
	SetStartPosition(pRidePlayer->GetNo(), false);
};


CRideCharacter::~CRideCharacter(void)
{
    ;
};


void CRideCharacter::Run(void)
{
    MakePadInfo();

    if (IsEnableMove())
    {
        CalcControlRate();
        if (IsShip())
        {
            Roll();
            MoveForShip();
        }
        else
        {
            Turn();
            Jump();
            Move();
        };
    }
    else
    {
        MoveByScroll();
    };
    
    CPlayerCharacter::Run();
    
    if (!IsEnableMove())
    {
        RwV3d vPos = Math::VECTOR3_ZERO;
        GetPosition(&vPos);

        SetPosition(&m_vRevivePos);
        UpdateModel();
        SetPosition(&vPos);

        if (IsShip())
            m_vRevivePos.z += (CGameProperty::GetElapsedTime() * -8.0f);
    };
    
    MoveLimit();

    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetPosition(&vPosition);
    if ((vPosition.y > -6.0f) && (vPosition.y < -6.0f))
        CMessageManager::Request(SEGROUPID::VALUE(162), GetID());
    
    Shot();

    m_fPlaytime += CGameProperty::GetElapsedTime();
};


void CRideCharacter::MessageProc(int32 nMessageID, void* pParam)
{
    switch (nMessageID)
    {
    case RIDETYPES::MESSAGEID_SCORE_INC:
        OnMessageScoreAdd(pParam);
        break;

    case RIDETYPES::MESSAGEID_SHOT_EOL:
        OnMessageShotEol();
        break;

    case RIDETYPES::MESSAGEID_SHOT_HIT:
        OnMessageShotHit();
        break;

    default:
        CPlayerCharacter::MessageProc(nMessageID, pParam);
        break;
    };
};


void CRideCharacter::Draw(void) const
{
    CCharacter::Draw();
};


void CRideCharacter::CalcVelocityPerFrame(RwV3d& rvVelocityPerFrame)
{
    CCharacter::CalcVelocityPerFrame(rvVelocityPerFrame);
};


void CRideCharacter::CheckCollisionForBody(RwV3d& rvVelocityPerFrame, bool bPeriod)
{
    if (!bPeriod)
        return;
    
    RwV3d vVelocityPerFrameSave = rvVelocityPerFrame;

    CCharacter::CheckCollisionForBody(rvVelocityPerFrame, bPeriod);
    rvVelocityPerFrame.z = vVelocityPerFrameSave.z;

    if (!Math::Vec3_IsEqual(&vVelocityPerFrameSave, &rvVelocityPerFrame))
    {
        if ((CGameProperty::GetElapsedTime() * 0.1f) > Math::RandFloat())
            CMessageManager::Request(SEGROUPID::VALUE(165), GetID());
    };
};


void CRideCharacter::CheckCollisionForWall(RwV3d& rvVelocityPerFrame, bool bPeriod)
{
    if (!bPeriod)
        return;
    
    if (CheckHit(rvVelocityPerFrame) != HIT_SPHERE_KIND_NONE)
        CCharacter::CalcVelocityPerFrame(rvVelocityPerFrame);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetPosition(&vPosition);
    
    Math::Vec3_Add(&vPosition, &vPosition, &rvVelocityPerFrame);
    
    SetPosition(&vPosition);
};


void CRideCharacter::OnMessageAerial(float fFieldHeight)
{
    if (!IsShip())
        CCharacter::OnMessageAerial(fFieldHeight);
};


bool CRideCharacter::OnMessageCanChangingAerial(float fFieldHeight)
{
    bool bResult = false;

    switch (GetStatus())
    {
    case PLAYERTYPES::RIDESTATUS_RUN:
    case PLAYERTYPES::RIDESTATUS_TOUCHDOWN:
    case PLAYERTYPES::RIDESTATUS_LEFT:
    case PLAYERTYPES::RIDESTATUS_RIGHT:
    case PLAYERTYPES::RIDESTATUS_SPEED_DOWN:
    case PLAYERTYPES::RIDESTATUS_SPEED_UP:
    case PLAYERTYPES::RIDESTATUS_STAGGER:
        bResult = true;
        break;

    default:
        break;
    };

    return bResult;
};


void CRideCharacter::OnMessageDoChangingAerial(void)
{
    ChangeStatus(PLAYERTYPES::RIDESTATUS_AERIAL);
};


void CRideCharacter::OnMessageTouchdown(float fFieldHeight)
{
    PLAYERTYPES::STATUS currentStatus = GetStatus();

    if ((currentStatus == PLAYERTYPES::RIDESTATUS_JUMP) ||
        (currentStatus == PLAYERTYPES::RIDESTATUS_AERIAL) ||
        (currentStatus == PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER))
    {
        ChangeStatus(PLAYERTYPES::RIDESTATUS_TOUCHDOWN);
    };
};


void CRideCharacter::OnMessageCatchAttack(CHitAttackData* pAttack)
{
    if (pAttack->GetPower())
        HitWall(HIT_SPHERE_KIND_CENTER);
};


void CRideCharacter::OnMessageReceivedDamage(int32 nDamage)
{
    if (nDamage)
        HitWall(HIT_SPHERE_KIND_CENTER);
};


void CRideCharacter::OnSteppedDeathFloor(void)
{
    HitWall(HIT_SPHERE_KIND_CENTER);
};


PLAYERTYPES::STATUS CRideCharacter::RequestStatusMorphing(PLAYERTYPES::STATUS status)
{
    PLAYERTYPES::STATUS currentStatus = GetStatus();

    if (status == PLAYERTYPES::RIDESTATUS_STAGGER &&
        ((currentStatus == PLAYERTYPES::RIDESTATUS_JUMP) || (currentStatus == PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER)))
    {
        status = PLAYERTYPES::RIDESTATUS_STAGGER;
    }
    else if ((status == PLAYERTYPES::RIDESTATUS_CRASH_WALL) && IsShip())
    {
        if (currentStatus == PLAYERTYPES::RIDESTATUS_REVIVE)
            status = PLAYERTYPES::RIDESTATUS_REVIVE;
        else
            status = PLAYERTYPES::RIDESTATUS_SHIP_CRASH;
    };

    return status;
};


void CRideCharacter::OnMessageShotEol(void)
{
    ASSERT(m_nNumDispShot > 0);
    --m_nNumDispShot;    
};


void CRideCharacter::OnMessageShotHit(void)
{
    CMessageManager::Request(SEGROUPID::VALUE(164), GetID());
    CGameEvent::SetPlayerRideAction(GetPlayerNo(), GAMETYPES::RIDEACT_KILL);
};


void CRideCharacter::OnMessageScoreAdd(void* param)
{
    RIDETYPES::SCOREKIND scorekind = static_cast<RIDETYPES::SCOREKIND>(reinterpret_cast<int32>(param));

    ASSERT(scorekind >= 0);
    ASSERT(scorekind < COUNT_OF(m_aScore));

    ++m_aScore[scorekind];

    RwV3d vOffset = { 0.0f, (IsShip() ? 1.0f : 2.0f), 0.0f };
    CEffectManager::PlayTrace(EFFECTID::ID_I_GET, new CRideCharacterTracer(this), &vOffset);

    switch (scorekind)
    {
    case RIDETYPES::SCOREKIND_GOLD:
        CGameEvent::SetPlayerRideAction(GetPlayerNo(), GAMETYPES::RIDEACT_SCORE_GOLD);
        break;

    case RIDETYPES::SCOREKIND_SILVER:
        CGameEvent::SetPlayerRideAction(GetPlayerNo(), GAMETYPES::RIDEACT_SCORE_SILVER);
        break;

    default:
        ASSERT(false);
        break;
    };

    int32 scoreGold = CRideStage::GetScore(RIDETYPES::SCOREKIND_GOLD);
    int32 scoreSilver = CRideStage::GetScore(RIDETYPES::SCOREKIND_SILVER);
    int32 scoreTotal = (scoreGold + scoreSilver);
    if (scoreTotal == 1)
    {
        switch (GetID())
        {
        case PLAYERID::ID_LEO:
            CMessageManager::DirectCall(SDCODE_VOICE(20705));
            break;

        case PLAYERID::ID_RAP:
            CMessageManager::DirectCall(SDCODE_VOICE(20707));
            break;

        case PLAYERID::ID_MIC:
            CMessageManager::DirectCall(SDCODE_VOICE(20706));
            break;

        case PLAYERID::ID_DON:
            CMessageManager::DirectCall(SDCODE_VOICE(20703));
            break;

        case PLAYERID::ID_SLA:
            CMessageManager::DirectCall(SDCODE_VOICE(20708));
            break;

        case PLAYERID::ID_CAS:
            CMessageManager::DirectCall(SDCODE_VOICE(20702));
            break;

        case PLAYERID::ID_KAR:
            CMessageManager::DirectCall(SDCODE_VOICE(20704));
            break;

        case PLAYERID::ID_SPL:
            CMessageManager::DirectCall(SDCODE_VOICE(20709));
            break;

        default:
            ASSERT(false);
            break;
        };

        CMessageManager::OnlyTextRequest(SEGROUPID::VALUE(163));
    }
    else if (Math::RandFloat() < 0.3f)
    {
        switch (GetID())
        {
        case PLAYERID::ID_LEO:
            CMessageManager::DirectCall(SDCODE_VOICE(20713));
            break;

        case PLAYERID::ID_RAP:
            CMessageManager::DirectCall(SDCODE_VOICE(20715));
            break;

        case PLAYERID::ID_MIC:
            CMessageManager::DirectCall(SDCODE_VOICE(20714));
            break;

        case PLAYERID::ID_DON:
            CMessageManager::DirectCall(SDCODE_VOICE(20711));
            break;

        case PLAYERID::ID_SLA:
            CMessageManager::DirectCall(SDCODE_VOICE(20716));
            break;

        case PLAYERID::ID_CAS:
            CMessageManager::DirectCall(SDCODE_VOICE(20710));
            break;

        case PLAYERID::ID_KAR:
            CMessageManager::DirectCall(SDCODE_VOICE(20712));
            break;

        case PLAYERID::ID_SPL:
            CMessageManager::DirectCall(SDCODE_VOICE(20717));
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CRideCharacter::OnAttach(CRideCharacter* pBeforeCharacter, bool bChangeEffectEnable)
{
    CPlayerCharacter::OnAttach(pBeforeCharacter, false);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    pBeforeCharacter->GetVelocity(&vVelocity);

    SetVelocity(&vVelocity);

    m_fControlRate = 1.0f;
    m_fTurnDirection = 0.0f;
    SetDirection(0.0f);
    SetEffectDrawEnable(true);

    if (bChangeEffectEnable)        
    {
        RwV3d vOffset = { 0.0f, 0.7f, 0.0f };
        CEffectManager::PlayTrace(EFFECTID::ID_BARRIER_START, new CRideCharacterTracer(this), &vOffset, true);
    };
};


void CRideCharacter::OnDetach(CRideCharacter* pAfterCharacter)
{
    CPlayerCharacter::OnDetach(pAfterCharacter);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    GetVelocity(&vVelocity);

    pAfterCharacter->SetVelocity(&vVelocity);

    SetEffectDrawEnable(false);
};


void CRideCharacter::OnCrashWall(void)
{
    GetPosition(&m_vRevivePos);
};


void CRideCharacter::OnRevive(void)
{
    m_fControlRate = 1.0f;
    SetStartPosition(GetPlayerNo(), true);
};


void CRideCharacter::MakePadInfo(void)
{
    std::memset(&m_padinfo, 0x00, sizeof(m_padinfo));
    m_padinfo.pad = GetPadID();

    float x = static_cast<float>(IPad::GetAnalog(m_padinfo.pad, IPad::ANALOG_LSTICK_X));
    float y = static_cast<float>(IPad::GetAnalog(m_padinfo.pad, IPad::ANALOG_LSTICK_Y));

    x = (x >= 0.0f ? (x / static_cast<float>(TYPEDEF::SINT16_MAX)) : -(x / static_cast<float>(TYPEDEF::SINT16_MIN)));
    y = (y >= 0.0f ? (y / static_cast<float>(TYPEDEF::SINT16_MAX)) : -(y / static_cast<float>(TYPEDEF::SINT16_MIN)));

    const float DEADZONE = 0.4f;

    m_padinfo.fStickX = (std::fabs(x) >= DEADZONE ? x : 0.0f);
    m_padinfo.fStickY = (std::fabs(y) >= DEADZONE ? y : 0.0f);

#ifdef TARGET_PC
    uint32 uMoveMask = CController::DIGITAL_LUP
                     | CController::DIGITAL_LDOWN
                     | CController::DIGITAL_LLEFT
                     | CController::DIGITAL_LRIGHT;

    uint32 uDigital = IPad::GetDigital(m_padinfo.pad);
    if (uDigital & uMoveMask)
    {
        float xDigital = 0.0f;
        float yDigital = 0.0f;

        if (uDigital & CController::DIGITAL_LLEFT)
            xDigital = -1.0f;
        else if (uDigital & CController::DIGITAL_LRIGHT)
            xDigital = 1.0f;

        if (uDigital & CController::DIGITAL_LUP)
            yDigital = 1.0f;
        else if (uDigital & CController::DIGITAL_LDOWN)
            yDigital = -1.0f;

        m_padinfo.fStickX = xDigital;
        m_padinfo.fStickY = yDigital;
    };
#endif    

    uint32 uDigitalTrigger = IPad::GetDigitalTrigger(m_padinfo.pad);

    m_padinfo.bShot       = IPad::CheckFunction(uDigitalTrigger, IPad::FUNCTION_SHOT);
    m_padinfo.bJump       = IPad::CheckFunction(uDigitalTrigger, IPad::FUNCTION_JUMP);
    m_padinfo.bLeftTurn   = IPad::CheckFunction(uDigitalTrigger, IPad::FUNCTION_DASH);
    m_padinfo.bRightTurn  = IPad::CheckFunction(uDigitalTrigger, IPad::FUNCTION_GUARD);
};


void CRideCharacter::CalcControlRate(void)
{
    if (m_fControlRate < 1.0f)
    {
        m_fControlRate += (CGameProperty::GetElapsedTime() / 3.0f);
        if (m_fControlRate > 1.0f)
            m_fControlRate = 1.0f;
    };
};


void CRideCharacter::Roll(void)
{
    if (m_padinfo.bLeftTurn)
        ChangeStatus(PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT);
    else if (m_padinfo.bRightTurn)
        ChangeStatus(PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT);
};


void CRideCharacter::MoveForShip(void)
{
    float dt = CGameProperty::GetElapsedTime();
    float x_max = CRideStage::GetMoveLimitXMax();
    float x_min = CRideStage::GetMoveLimitXMin();
    float y_max = CRideStage::GetMoveLimitYMax();
    float y_min = CRideStage::GetMoveLimitYMin();

    //
    //	x vel
    //
    float damping_x = m_vVelocity.x * 0.97f;
    float dx = damping_x - (dt * (m_padinfo.fStickX * m_fControlRate) * 18.0f);
    float px = m_vPosition.x + dx * dt;

    dx = (px >= x_max ? 0.0f : dx);
    dx = (px <= x_min ? 0.0f : dx);

    //
    //	y vel
    //
    float damping_y = m_vVelocity.y * 0.97f;
    float dy = damping_y - (dt * (m_padinfo.fStickY * m_fControlRate) * 18.0f);
    float py = m_vPosition.y + dy * dt;

    dy = (py >= y_max ? 0.0f : dy);
    dy = (py <= y_min ? 0.0f : dy);

    //
    //	apply vel
    //
    RwV3d vBasisVec = Math::VECTOR3_ZERO;
    CRideStage::GetBasisVector(&vBasisVec);

    m_vVelocity.x = dx;
    m_vVelocity.y = dy;
    m_vVelocity.z = vBasisVec.z;
};


void CRideCharacter::Turn(void)
{
    float fTurnDirection = GetTurnDirection();

    if (fTurnDirection > 0.0f)
    {
        fTurnDirection += CGameProperty::GetElapsedTime() * (Math::PI * 3.0f);
        if (fTurnDirection > Math::PI2)
            fTurnDirection = 0.0f;

        SetTurnDirection(fTurnDirection);
        SetDirection(fTurnDirection);
        return;
    }
    else if (fTurnDirection < 0.0f)
    {
        fTurnDirection -= CGameProperty::GetElapsedTime() * (Math::PI * 3.0f);
        if (fTurnDirection < -Math::PI2)
            fTurnDirection = 0.0f;

        SetTurnDirection(fTurnDirection);
        SetDirection(fTurnDirection);
        return;
    };

    if (GetStatus() == PLAYERTYPES::RIDESTATUS_JUMP)
    {
        if (m_padinfo.bLeftTurn)
        {
            fTurnDirection += CGameProperty::GetElapsedTime() * (Math::PI * 3.0f);
            CGameEvent::SetPlayerRideAction(GetPlayerNo(), GAMETYPES::RIDEACT_TRICK);
        }
        else if (m_padinfo.bRightTurn)
        {
            fTurnDirection -= CGameProperty::GetElapsedTime() * (Math::PI * 3.0f);
            CGameEvent::SetPlayerRideAction(GetPlayerNo(), GAMETYPES::RIDEACT_TRICK);
        };

        SetTurnDirection(fTurnDirection);
        SetDirection(fTurnDirection);
    };
};


void CRideCharacter::Jump(void)
{
    if (m_padinfo.bJump && !TestPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS))
        ChangeStatus(PLAYERTYPES::RIDESTATUS_JUMP);
};


void CRideCharacter::Move(void)
{
    float dt = CGameProperty::GetElapsedTime();
    float x_max = CRideStage::GetMoveLimitXMax();
    float x_min = CRideStage::GetMoveLimitXMin();
	float z_max = CRideStage::GetMoveLimitZMax();
	float z_min = CRideStage::GetMoveLimitZMin();

	//
	//	x vel
	//
    float damping = m_vVelocity.x * 0.97f;
    float dx = damping - (dt * (m_padinfo.fStickX * m_fControlRate) * 18.0f);
    float px = m_vPosition.x + dx * dt;

    dx = (px >= x_max ? 0.0f : dx);
    dx = (px <= x_min ? 0.0f : dx);

	//
	//	y vel
	//
    float dy = (m_padinfo.fStickY * m_fControlRate) * 1.9444444f;

	float py = m_vPosition.z + dy * dt;

	dy = (py >= z_max ? 0.0f : dy);
	dy = (py <= z_min ? 0.0f : dy);

	//
	//	apply vel
	//
	RwV3d vBasisVec = Math::VECTOR3_ZERO;
	CRideStage::GetBasisVector(&vBasisVec);

    m_vVelocity.x = dx;
    m_vVelocity.z = (vBasisVec.z + dy);
};


void CRideCharacter::MoveByScroll(void)
{
    RwV3d vBasisVec = Math::VECTOR3_ZERO;
    CRideStage::GetBasisVector(&vBasisVec);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    vVelocity.z = vBasisVec.z;
    SetVelocity(&vVelocity);
};


void CRideCharacter::MoveLimit(void)
{
    if (!IsEnableMove())
        return;

    float fMaxZ = CRideStage::GetMoveLimitZMax();
    float fMinZ = CRideStage::GetMoveLimitZMin();

    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetPosition(&vPosition);

    if (IsShip())
    {
        vPosition.z = fMinZ;
        SetPosition(&vPosition);
    }
    else
    {
        if (vPosition.z < fMinZ)
            vPosition.z = fMinZ;

        if (vPosition.z > fMaxZ)
            vPosition.z = fMaxZ;

        SetPosition(&vPosition);
    };
};


void CRideCharacter::Shot(void)
{
    if (m_padinfo.bShot && IsEnableShot() && CheckDispShot())
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        GetBodyPosition(&vPosition);

        CShotManager::Shot(SHOTID::ID_RIDE, &vPosition, 0.0f, this, 0.0f, 3.0f);

        CGameSound::PlayObjectSE(this, SDCODE_SE(4145));

        ++m_nNumDispShot;
    };
};


void CRideCharacter::HitWall(HIT_SPHERE_KIND hitspherekind)
{
    switch (hitspherekind)
    {
    case HIT_SPHERE_KIND_CENTER:
        {
            if (IsEnableMove())
            {
                RwV3d vPosition = Math::VECTOR3_ZERO;
                GetPosition(&vPosition);

                if (GetStatus() != PLAYERTYPES::RIDESTATUS_REVIVE)
                    CEffectManager::Play(EFFECTID::ID_ALL_PITAN, &vPosition);

                ChangeStatus(PLAYERTYPES::RIDESTATUS_CRASH_WALL);
            };
        }
        break;

    case HIT_SPHERE_KIND_LEFT:
        {
            if (IsEnableStagger())
            {
                ChangeStatus(PLAYERTYPES::RIDESTATUS_STAGGER);
                m_fControlRate = 0.0f;
            };

            ReplaceVelocityX(-8.333333f);
        }
        break;

    case HIT_SPHERE_KIND_RIGHT:
        {
            if (IsEnableStagger())
            {
                ChangeStatus(PLAYERTYPES::RIDESTATUS_STAGGER);
                m_fControlRate = 0.0f;
            };

            ReplaceVelocityX(8.3333333f);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CRideCharacter::GetHitSphere(RwSphere* pSphere, HIT_SPHERE_KIND hitspherekind)
{
    ASSERT(pSphere);
    ASSERT(hitspherekind >= 0);
    ASSERT(hitspherekind < HIT_SPHERE_KIND_NUM);

    static const RwSphere s_aHitSphere[HIT_SPHERE_KIND_NUM] =
    {
        { {  0.0f, 0.8f, 0.7f }, 0.3f },
        { {  0.4f, 0.8f, 0.7f }, 0.3f },
        { { -0.4f, 0.8f, 0.7f }, 0.3f },
    };

    GetFootPosition(&pSphere->center);
    Math::Vec3_Add(&pSphere->center, &pSphere->center, &s_aHitSphere[hitspherekind].center);

    pSphere->radius = s_aHitSphere[hitspherekind].radius;
};


CRideCharacter::HIT_SPHERE_KIND CRideCharacter::CheckHit(RwV3d& rvVelocityPerFrame)
{
    HIT_SPHERE_KIND Result = HIT_SPHERE_KIND_NONE;

    if (!IsEnableMove())
        return Result;

    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetPosition(&vPosition);

    if (vPosition.y >= -10.0f)
    {
        for (int32 i = 0; i < HIT_SPHERE_KIND_NUM; ++i)
        {
            RwSphere sphere = { 0 };
            GetHitSphere(&sphere, HIT_SPHERE_KIND(i));

            if (CWorldMap::CheckCollisionSphereMove(&sphere.center, sphere.radius, &rvVelocityPerFrame, CWorldMap::CHECKFLAG_NONE))
            {
                Result = HIT_SPHERE_KIND(i);
                HitWall(Result);
                break;
            };
        };
    }
    else
    {
        HitWall(HIT_SPHERE_KIND_CENTER);
    };

    return Result;
};


void CRideCharacter::ReplaceVelocityX(float fVelocityX)
{
    RwV3d vVelocity = Math::VECTOR3_ZERO;
    GetVelocity(&vVelocity);    
    vVelocity.x = fVelocityX;
    SetVelocity(&vVelocity);
};


void CRideCharacter::SetStartPosition(int32 nPlayerNo, bool bRevive)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;

    vPosition.x = ((nPlayerNo % 2) == 1) ? -1.0f : 1.0f;

    if (IsShip())
        vPosition.y += static_cast<float>(nPlayerNo / 2);
    else
        vPosition.z += static_cast<float>((nPlayerNo / 2) * 4);

    vPosition.z += CRideStage::GetMoveLimitZMin();

    if (bRevive)
        vPosition.y += (IsShip() ? 3.5f : 4.0f);
    else
        vPosition.y += 1.0f;

    SetPosition(&vPosition);
};


void CRideCharacter::SetEffectDrawEnable(bool bEnable)
{
    CRideCharacterEffectModule* pRideEffectMod = static_cast<CRideCharacterEffectModule*>(GetModule(MODULETYPE::RIDE_EFFECT));
    if (pRideEffectMod)
        pRideEffectMod->SetDrawEnable(bEnable);

    CCircleShadowModule* pCircleShadowMod = static_cast<CCircleShadowModule*>(GetModule(MODULETYPE::CIRCLE_SHADOW));
    if (pCircleShadowMod)
        pCircleShadowMod->SetEnable(bEnable);
};


void CRideCharacter::SetGravityEnable(bool bEnable)
{
    if (IsShip())
        bEnable = false;

    if (bEnable)
        ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    else
        SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
};


bool CRideCharacter::IsEnableMove(void) const
{
    switch (GetStatus())
    {
    case PLAYERTYPES::RIDESTATUS_RUN:
    case PLAYERTYPES::RIDESTATUS_JUMP:
    case PLAYERTYPES::RIDESTATUS_AERIAL:
    case PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER:
    case PLAYERTYPES::RIDESTATUS_TOUCHDOWN:
    case PLAYERTYPES::RIDESTATUS_LEFT:
    case PLAYERTYPES::RIDESTATUS_RIGHT:
    case PLAYERTYPES::RIDESTATUS_SPEED_UP:
    case PLAYERTYPES::RIDESTATUS_SPEED_DOWN:
    case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT:
    case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT:
    case PLAYERTYPES::RIDESTATUS_REVIVE:
    case PLAYERTYPES::RIDESTATUS_STAGGER:
        return true;

    default:
        return false;
    };
};


bool CRideCharacter::IsEnableStagger(void) const
{
    switch (GetStatus())
    {
    case PLAYERTYPES::RIDESTATUS_RUN:
    case PLAYERTYPES::RIDESTATUS_JUMP:
    case PLAYERTYPES::RIDESTATUS_AERIAL:
    case PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER:
    case PLAYERTYPES::RIDESTATUS_TOUCHDOWN:
    case PLAYERTYPES::RIDESTATUS_LEFT:
    case PLAYERTYPES::RIDESTATUS_RIGHT:
    case PLAYERTYPES::RIDESTATUS_SPEED_UP:
    case PLAYERTYPES::RIDESTATUS_SPEED_DOWN:
    case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT:
    case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT:
    case PLAYERTYPES::RIDESTATUS_STAGGER:
        return true;
        
    default:
        return false;
    };
};


bool CRideCharacter::IsEnableShot(void) const
{
    if (!IsShip())
        return false;

    switch (GetStatus())
    {
    case PLAYERTYPES::RIDESTATUS_RUN:
    case PLAYERTYPES::RIDESTATUS_JUMP:
    case PLAYERTYPES::RIDESTATUS_AERIAL:
    case PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER:
    case PLAYERTYPES::RIDESTATUS_TOUCHDOWN:
    case PLAYERTYPES::RIDESTATUS_LEFT:
    case PLAYERTYPES::RIDESTATUS_RIGHT:
    case PLAYERTYPES::RIDESTATUS_SPEED_UP:
    case PLAYERTYPES::RIDESTATUS_SPEED_DOWN:
    case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_LEFT:
    case PLAYERTYPES::RIDESTATUS_SHIP_ROLL_RIGHT:
    case PLAYERTYPES::RIDESTATUS_STAGGER:
        return true;

    default:
        return false;
    };
};


bool CRideCharacter::IsEnableRoll(void) const
{
    if (!IsShip())
        return false;

    switch (GetStatus())
    {
    case PLAYERTYPES::RIDESTATUS_RUN:
    case PLAYERTYPES::RIDESTATUS_LEFT:
    case PLAYERTYPES::RIDESTATUS_RIGHT:
    case PLAYERTYPES::RIDESTATUS_SPEED_UP:
    case PLAYERTYPES::RIDESTATUS_SPEED_DOWN:
        return true;

    default:
        return false;
    };
};