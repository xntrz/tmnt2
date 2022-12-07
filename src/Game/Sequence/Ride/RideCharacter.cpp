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
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


/*static*/ CRideCharacter* CRideCharacter::New(PLAYERID::VALUE idChr, CRidePlayer* pRidePlayer, GAMETYPES::COSTUME costume)
{
    char szPlayerName[GAMEOBJECTTYPES::NAME_MAX];
    szPlayerName[0] = '\0';

    std::strcpy(szPlayerName, PLAYERID::GetName(idChr));
    std::strcat(szPlayerName, "_r");

    CRideCharacter* pRideCharacter = new CRideCharacter(szPlayerName, idChr, pRidePlayer, costume);
    ASSERT(pRideCharacter);

    return pRideCharacter;
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
        m_pModuleMan->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, false));

    CRideCharacterEffectModule* pRideEffetMod = nullptr;
    if (CRideStage::m_bSpace)
    {
        pRideEffetMod = CRideCharacterEffectModule::New(this, CRideCharacterEffectModule::RIDERTYPE_SHIP);
    }
    else
    {
        pRideEffetMod = CRideCharacterEffectModule::New(this, CRideCharacterEffectModule::RIDERTYPE_BOARD);
    };

    if (pRideEffetMod)
    {
        pRideEffetMod->SetDrawEnable(false);
        m_pModuleMan->Include(pRideEffetMod);
    };

    InitBandana();

    SetGravityEnable(true);
	SetStartPosition(pRidePlayer->GetNo(), false);
};


CRideCharacter::~CRideCharacter(void)
{
    ;
};


void CRideCharacter::Run(void)
{
    UpdateBandana();
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetPosition(&vPosition);
    
    if (IsEnableMove())
    {
        CalcControlRate();
    }
    else
    {
        RwV3d vBasisVector = Math::VECTOR3_ZERO;
        CRideStage::GetBasisVector(&vBasisVector);

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        vVelocity.z = vBasisVector.z;
        SetVelocity(&vVelocity);
    };

    CPlayerCharacter::Run();

    if (!IsEnableMove())
    {
        CBandanaModule* pBandanaMod = (CBandanaModule*)GetModule(MODULETYPE::BANDANA);
        if (pBandanaMod)
            pBandanaMod->Reset(&m_vRevivePos);
        
        RwV3d vPosition = Math::VECTOR3_ZERO;
        GetPosition(&vPosition);;

		SetPosition(&m_vRevivePos);
		UpdateModel();
		SetPosition(&vPosition);
    };

    MoveLimit();

    if ((vPosition.y > -6.0f) && (vPosition.y < -6.0f))
        CMessageManager::Request(SEGROUPID::VALUE(162), GetID());

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

    case RIDETYPES::MESSAGEID_SHOT_DESTROY:
        OnMessageShotDestroy();
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


void CRideCharacter::CheckCollisionForBody(RwV3d& rvVelocityPerFrame)
{
    RwV3d vVelocityPerFrameSave = rvVelocityPerFrame;
    
    CCharacter::CheckCollisionForBody(rvVelocityPerFrame);
    rvVelocityPerFrame.z = vVelocityPerFrameSave.z;

    if (!Math::Vec3_IsEqual(&vVelocityPerFrameSave, &rvVelocityPerFrame))
    {
        if (CGameProperty::GetElapsedTime() * 0.1f > Math::RandFloat())
            CMessageManager::Request(SEGROUPID::VALUE(165), GetID());
    };
};


void CRideCharacter::CheckCollisionForWall(RwV3d& rvVelocityPerFrame)
{
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
    };

    return bResult;
};


void CRideCharacter::OnMessageDoChangingAerial(void)
{
    ChangeStatus(PLAYERTYPES::RIDESTATUS_AERIAL);
};


void CRideCharacter::OnMessageTouchdown(float fFieldHeight)
{
    if (GetStatus() == PLAYERTYPES::RIDESTATUS_JUMP ||
        GetStatus() == PLAYERTYPES::RIDESTATUS_AERIAL ||
        GetStatus() == PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER)
    {
        ChangeStatus(PLAYERTYPES::RIDESTATUS_TOUCHDOWN);
    };
};


void CRideCharacter::OnMessageCatchAttack(CHitAttackData* pAttack)
{
    ASSERT(pAttack);
    
    if (pAttack->GetPower())
    {
        if (IsEnableMove())
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            GetPosition(&vPosition);

            if (GetStatus() != PLAYERTYPES::RIDESTATUS_REVIVE)
                CEffectManager::Play(EFFECTID::ID_ALL_PITAN, &vPosition);

            ChangeStatus(PLAYERTYPES::RIDESTATUS_CRASH_WALL);
        };
    };
};


void CRideCharacter::OnMessageReceivedDamage(int32 nDamage)
{
    if (nDamage)
    {
        if (IsEnableMove())
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            GetPosition(&vPosition);

            if (GetStatus() != PLAYERTYPES::RIDESTATUS_REVIVE)
                CEffectManager::Play(EFFECTID::ID_ALL_PITAN, &vPosition);

            ChangeStatus(PLAYERTYPES::RIDESTATUS_CRASH_WALL);
        };
    };
};


void CRideCharacter::OnSteppedDeathFloor(void)
{
    if (IsEnableMove())
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        GetPosition(&vPosition);

        if (GetStatus() != PLAYERTYPES::RIDESTATUS_REVIVE)
            CEffectManager::Play(EFFECTID::ID_ALL_PITAN, &vPosition);

        ChangeStatus(PLAYERTYPES::RIDESTATUS_CRASH_WALL);
    };
};


void CRideCharacter::ShootingKnife(void)
{
    if (IsEnableShot() && CheckDispShot())
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        GetBodyPosition(&vPosition);
        
        CShotManager::Shot(SHOTID::ID_RIDE, &vPosition, 0.0f, this, 0.0f, 3.0f);
        
        CGameSound::PlayObjectSE(this, SDCODE_SE(4145));

        ++m_nNumDispShot;
    };
};


PLAYERTYPES::STATUS CRideCharacter::RequesStatusMorphing(PLAYERTYPES::STATUS status)
{
    PLAYERTYPES::STATUS NowStatus = GetStatus();

    if (status == PLAYERTYPES::RIDESTATUS_STAGGER &&
        (NowStatus == PLAYERTYPES::RIDESTATUS_JUMP || NowStatus == PLAYERTYPES::RIDESTATUS_AERIAL_STAGGER))
    {
        status = PLAYERTYPES::RIDESTATUS_STAGGER;
    }
    else if (status == PLAYERTYPES::RIDESTATUS_CRASH_WALL && IsShip())
    {
        if (NowStatus == PLAYERTYPES::RIDESTATUS_REVIVE)
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


void CRideCharacter::OnMessageShotDestroy(void)
{
    CMessageManager::Request(SEGROUPID::VALUE(161), GetID());
    CGameEvent::SetPlayerRideAction(GetPlayerNo(), GAMETYPES::RIDEACT_KILL);
};


void CRideCharacter::OnMessageScoreAdd(void* param)
{
    RIDETYPES::SCOREKIND scorekind = RIDETYPES::SCOREKIND(int32(param));

    ASSERT(scorekind >= 0 && scorekind < RIDETYPES::SCOREKINDNUM);
    ASSERT(scorekind >= 0 && scorekind < COUNT_OF(m_aScore));

    ++m_aScore[scorekind];

    RwV3d vOffset = { 0.0f, 2.0f, 0.0f };
    if (!IsShip())
        vOffset.y = 3.0f;
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

    int32 nScoreTotal = GetScore(RIDETYPES::SCOREKIND_GOLD) + GetScore(RIDETYPES::SCOREKIND_SILVER);

    if(nScoreTotal == 1)
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
    CPlayerCharacter::OnAttach(pBeforeCharacter, bChangeEffectEnable);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    pBeforeCharacter->GetVelocity(&vVelocity);
    SetVelocity(&vVelocity);

    m_fControlRate = 1.0f;
    m_fTurnDirection = 0.0f;
    SetDirection(0.0f);

    CRideCharacterEffectModule* pRideEffectMod = (CRideCharacterEffectModule*)GetModule(MODULETYPE::RIDE_EFFECT);
    if (pRideEffectMod)
        pRideEffectMod->SetDrawEnable(true);

    CCircleShadowModule* pCircleShadowMod = (CCircleShadowModule*)GetModule(MODULETYPE::CIRCLE_SHADOW);
    if (pCircleShadowMod)
        pCircleShadowMod->SetEnable(true);
};


void CRideCharacter::OnDetach(CRideCharacter* pAfterCharacter)
{
    CPlayerCharacter::OnDetach(pAfterCharacter);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    GetVelocity(&vVelocity);
    pAfterCharacter->SetVelocity(&vVelocity);

    CRideCharacterEffectModule* pRideEffectMod = (CRideCharacterEffectModule*)GetModule(MODULETYPE::RIDE_EFFECT);
    if (pRideEffectMod)
        pRideEffectMod->SetDrawEnable(false);

    CCircleShadowModule* pCircleShadowMod = (CCircleShadowModule*)GetModule(MODULETYPE::CIRCLE_SHADOW);
    if (pCircleShadowMod)
        pCircleShadowMod->SetEnable(false);
};


bool CRideCharacter::IsEnableMove(void) const
{
    bool bResult = false;
    
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
        bResult = true;
        break;
    };

    return bResult;
};


bool CRideCharacter::IsEnableStagger(void) const
{
    bool bResult = false;

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
        bResult = true;
        break;
    };

    return bResult;
};


bool CRideCharacter::IsEnableShot(void) const
{
    bool bResult = false;

    if (!IsShip())
        return bResult;

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
        bResult = true;
        break;
    };

    return bResult;
};


bool CRideCharacter::IsEnableRoll(void) const
{
    bool bResult = false;

    if (!IsShip())
        return bResult;

    switch (GetStatus())
    {
    case PLAYERTYPES::RIDESTATUS_RUN:  
    case PLAYERTYPES::RIDESTATUS_LEFT:
    case PLAYERTYPES::RIDESTATUS_RIGHT:
    case PLAYERTYPES::RIDESTATUS_SPEED_UP:
    case PLAYERTYPES::RIDESTATUS_SPEED_DOWN:    
        bResult = true;
        break;
    };

    return bResult;
};


void CRideCharacter::SetStartPosition(int32 nPlayerNo, bool bRevive)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;

    if (nPlayerNo % 2)
    {
        if (nPlayerNo % 2 == 1)
            vPosition.x = -1.0f;
    }
    else
    {
        vPosition.x = 1.0f;
    };

    if (IsShip())
        vPosition.y += float(nPlayerNo / 2);
    else
        vPosition.z += float((nPlayerNo / 2) * 4);

    vPosition.z += CRideStage::GetMoveLimitZMin();

    if (bRevive)
    {
        if (IsShip())
            vPosition.y += 3.5f;
        else
            vPosition.y += 4.0f;
    }
    else
    {
        vPosition.y += 1.0f;
    };

    SetPosition(&vPosition);
};


void CRideCharacter::SetEffectDrawEnable(bool bEnable)
{
    CRideCharacterEffectModule* pRideEffectMod = (CRideCharacterEffectModule*)GetModule(MODULETYPE::RIDE_EFFECT);
    if (pRideEffectMod)
        pRideEffectMod->SetDrawEnable(bEnable);

    CCircleShadowModule* pCircleShadowMod = (CCircleShadowModule*)GetModule(MODULETYPE::CIRCLE_SHADOW);
    if (pCircleShadowMod)
        pCircleShadowMod->SetEnable(bEnable);
};


void CRideCharacter::SetGravityEnable(bool bEnable)
{
    if (!IsShip())
        SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, !bEnable);
    else
        SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, true);
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

            ReplaceVelocityX(-8.33f);
        }
        break;

    case HIT_SPHERE_KIND_RIGHT:
        {
            if (IsEnableStagger())
            {
                ChangeStatus(PLAYERTYPES::RIDESTATUS_STAGGER);
                m_fControlRate = 0.0f;
            };

            ReplaceVelocityX(8.33f);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CRideCharacter::GetHitSphere(RwSphere* pSphere, HIT_SPHERE_KIND hitspherekind)
{
    ASSERT(hitspherekind >= 0 && hitspherekind < HIT_SPHERE_KIND_NUM);
    ASSERT(pSphere);

    static const RwSphere s_aHitSphere[HIT_SPHERE_KIND_NUM] =
    {
        { { 0.0f, 0.8f, 0.7f }, 0.3f },
        { { 0.4f, 0.8f, 0.7f }, 0.3f },
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


void CRideCharacter::CalcControlRate(void)
{
    if (m_fControlRate < 1.0f)
    {
        m_fControlRate += (CGameProperty::GetElapsedTime() * 0.33f);
        if (m_fControlRate > 1.0f)
            m_fControlRate = 1.0f;
    };
};


void CRideCharacter::MoveLimit(void)
{
    if (!IsEnableMove())
        return;

    float fZMax = CRideStage::GetMoveLimitZMax();
    float fZMin = CRideStage::GetMoveLimitZMin();
    float fXMax = CRideStage::GetMoveLimitXMax();
    float fXMin = CRideStage::GetMoveLimitXMin();
    float fYMax = CRideStage::GetMoveLimitYMax();
    float fYMin = CRideStage::GetMoveLimitYMin();

    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetPosition(&vPosition);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    GetVelocity(&vVelocity);

    if (vPosition.x < fXMin || vPosition.x > fXMax)
        vVelocity.x = 0.0f;
    
    vPosition.x = Math::Clamp(vPosition.x, fXMin, fXMax);

    if (IsShip())
    {
        if (vPosition.y < fYMin || vPosition.y > fYMax)
            vVelocity.y = 0.0f;

        vPosition.y = Math::Clamp(vPosition.y, fYMin, fYMax);

        vPosition.z = fZMin;
        SetPosition(&vPosition);
    }
    else
    {
        if (vPosition.z < fZMin || vPosition.z > fZMax)
            vVelocity.z = 0.0f;

        vPosition.z = Math::Clamp(vPosition.z, fZMin, fZMax);
        SetPosition(&vPosition);
    };

    SetVelocity(&vVelocity);
};


void CRideCharacter::SetTurnDirection(float fDir)
{
    m_fTurnDirection = fDir;
};


float CRideCharacter::GetTurnDirection(void) const
{
    return m_fTurnDirection;
};


float CRideCharacter::GetPlaytime(void) const
{
    return m_fPlaytime;
};


float CRideCharacter::GetControlRate(void) const
{
    return m_fControlRate;
};


bool CRideCharacter::IsShip(void) const
{
    return CRideStage::m_bSpace;
};


void CRideCharacter::ResetTurnDirection(void)
{
    SetTurnDirection(0.0f);
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


void CRideCharacter::InitBandana(void)
{
	if (IsShip())
		return;

    RwV3d vBandanaOffset = Math::VECTOR3_ZERO;
    CBandanaModule::BANDANACOLOR BandanaColor = CBandanaModule::BANDANACOLOR_RED;
    bool bBandanaFlag = false;

    switch (GetID())
    {
    case PLAYERID::ID_LEO:
        {
            bBandanaFlag = true;
            vBandanaOffset = { 0.0f, 0.15f, 0.05f };
            BandanaColor = CBandanaModule::BANDANACOLOR_BLUE;
        }
        break;

    case PLAYERID::ID_RAP:
        {
            bBandanaFlag = true;
            vBandanaOffset = { 0.0f, 0.15f, 0.05f };
            BandanaColor = CBandanaModule::BANDANACOLOR_RED;
        }
        break;

    case PLAYERID::ID_MIC:
        {
            bBandanaFlag = true;
            vBandanaOffset = { 0.0f, 0.15f, 0.05f };
            BandanaColor = CBandanaModule::BANDANACOLOR_ORANGE;
        }
        break;

    case PLAYERID::ID_DON:
        {
            bBandanaFlag = true;
            vBandanaOffset = { 0.0f, 0.15f, 0.05f };
            BandanaColor = CBandanaModule::BANDANACOLOR_PURPLE;
        }
        break;

    case PLAYERID::ID_SLA:
        {
            bBandanaFlag = true;
            vBandanaOffset = { 0.0f, 0.15f, 0.05f };
            BandanaColor = CBandanaModule::BANDANACOLOR_SLASHUURGREY;
        }
        break;
    };

    if (bBandanaFlag)
    {
        m_pModuleMan->Include(new CBandanaModule(
            this,
            m_pModel,
            CHARACTERTYPES::BONEID_HEAD,
            &vBandanaOffset,
            BandanaColor
        ));
    };
};


void CRideCharacter::UpdateBandana(void)
{
    CBandanaModule* pBandanaMod = (CBandanaModule*)GetModule(MODULETYPE::BANDANA);
    if (pBandanaMod)
    {
        float fDir = GetDirection();
        RwV3d vWind = Math::VECTOR3_ZERO;

        vWind.x = Math::Sin(m_fPlaytime * Math::PI2) * 12.0f;
        vWind.y = 1.5f + (Math::Sin(m_fPlaytime * 30.0f * Math::PI) * 30.0f);
        vWind.z = Math::Cos(fDir) * -1.0f * 3.0f;

        pBandanaMod->SetWind(&vWind);
    };
};


bool CRideCharacter::CheckDispShot(void) const
{
    return (m_nNumDispShot < 4);
};


int32 CRideCharacter::GetScore(RIDETYPES::SCOREKIND scorekind) const
{
    ASSERT(scorekind >= 0 && scorekind < RIDETYPES::SCOREKINDNUM);
    ASSERT(scorekind >= 0 && scorekind < COUNT_OF(m_aScore));
    return m_aScore[scorekind];
};